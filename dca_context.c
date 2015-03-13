/*
 * This file is part of dcadec.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "common.h"
#include "core_decoder.h"
#include "exss_parser.h"
#include "xll_decoder.h"
#include "fixed_math.h"
#include "dmix_tables.h"

#define DCADEC_PACKET_CORE  0x01
#define DCADEC_PACKET_EXSS  0x02
#define DCADEC_PACKET_XLL   0x04

struct dcadec_context {
    int flags;
    int packet;

    struct core_decoder *core;
    struct exss_parser *exss;
    struct xll_decoder *xll;

    int nframesamples;
    int sample_rate;
    int bits_per_sample;
    int profile;
    int channel_mask;
    int *samples[SPEAKER_COUNT];
};

static const uint8_t dca2wav[] = {
    WAVESPKR_FC,  WAVESPKR_FL,  WAVESPKR_FR,  WAVESPKR_SL,
    WAVESPKR_SR,  WAVESPKR_LFE, WAVESPKR_BC,  WAVESPKR_BL,
    WAVESPKR_BR,  WAVESPKR_SL,  WAVESPKR_SR,  WAVESPKR_FLC,
    WAVESPKR_FRC, WAVESPKR_TFL, WAVESPKR_TFC, WAVESPKR_TFR,
    WAVESPKR_LFE, WAVESPKR_FLC, WAVESPKR_FRC, WAVESPKR_TC,
    WAVESPKR_TFL, WAVESPKR_TFR, WAVESPKR_TBC, WAVESPKR_TBL,
    WAVESPKR_TBR, WAVESPKR_BC,  WAVESPKR_BL,  WAVESPKR_BR
};

static int reorder_samples(struct dcadec_context *dca, int **dca_samples, int dca_mask)
{
    int nchannels = 0;

    if (dca->flags & DCADEC_FLAG_NATIVE_LAYOUT) {
        for (int dca_ch = 0; dca_ch < SPEAKER_COUNT; dca_ch++) {
            if (dca_mask & (1 << dca_ch)) {
                dca->samples[nchannels++] = dca_samples[dca_ch];
            }
        }
        dca->channel_mask = dca_mask;
    } else {
        int wav_mask = 0;
        int *wav_samples[WAVESPKR_COUNT] = { NULL };
        for (size_t dca_ch = 0; dca_ch < sizeof(dca2wav); dca_ch++) {
            if (dca_mask & (1 << dca_ch)) {
                int wav_ch = dca2wav[dca_ch];
                if (!wav_samples[wav_ch]) {
                    wav_samples[wav_ch] = dca_samples[dca_ch];
                    wav_mask |= 1 << wav_ch;
                }
            }
        }
        for (int wav_ch = 0; wav_ch < WAVESPKR_COUNT; wav_ch++) {
            if (wav_mask & (1 << wav_ch)) {
                dca->samples[nchannels++] = wav_samples[wav_ch];
            }
        }
        dca->channel_mask = wav_mask;
    }

    return nchannels;
}

static int filter_core_frame(struct dcadec_context *dca)
{
    struct core_decoder *core = dca->core;

    // Filter core frame
    int ret;
    if ((ret = core_filter(core, dca->flags)) < 0)
        return ret;

    // Reorder sample buffer pointers
    if (reorder_samples(dca, core->output_samples, core->ch_mask) <= 0)
        return -DCADEC_EINVAL;

    dca->nframesamples = core->npcmsamples;
    dca->sample_rate = core->output_rate;
    dca->bits_per_sample = core->bits_per_sample;
    if (core->xbr_present || core->xxch_present)
        dca->profile = DCADEC_PROFILE_HD_HRA;
    else if (core->es_format && core->xch_present)
        dca->profile = DCADEC_PROFILE_DS_ES;
    else
        dca->profile = DCADEC_PROFILE_DS;
    return 0;
}

static int map_spkr_to_core_ch(struct core_decoder *core, int spkr)
{
    if (core->ch_mask & (1 << spkr))
        return spkr;
    if (spkr == SPEAKER_Lss && (core->ch_mask & SPEAKER_MASK_Ls))
        return SPEAKER_Ls;
    if (spkr == SPEAKER_Rss && (core->ch_mask & SPEAKER_MASK_Rs))
        return SPEAKER_Rs;
    return -1;
}

static struct xll_chset *find_hier_dmix_chset(struct xll_decoder *xll, struct xll_chset *c)
{
    c++;
    while (c != &xll->chset[xll->nchsets]) {
        if (!c->primary_chset
            && c->dmix_embedded
            && c->hier_chset
            && !c->replace_set_index
            && c->ch_mask_enabled)
            return c;
        c++;
    }
    return NULL;
}

static int conv_dmix_scale(int code)
{
    unsigned int index = (code & 0xff) - 1;
    if (index < dca_countof(dmix_table)) {
        int sign = (code >> 8) - 1;
        int coeff = dmix_table[index];
        return (coeff ^ sign) - sign;
    }
    return 0;
}

static int conv_dmix_scale_inv(int code)
{
    unsigned int index = (code & 0xff) - 41;
    if (index < dca_countof(dmix_table_inv)) {
        int sign = (code >> 8) - 1;
        int coeff = dmix_table_inv[index];
        return (coeff ^ sign) - sign;
    }
    return 0;
}

static int undo_down_mix(struct xll_decoder *xll,
                         struct xll_chset *c,
                         struct xll_chset *o,
                         int **samples, int nchannels)
{
    if (c->freq != o->freq)
        return -DCADEC_ENOSUP;

    int shift = c->pcm_bit_res - o->pcm_bit_res;
    if (shift < 0)
        return -DCADEC_EINVAL;

    int *coeff_ptr = o->dmix_coeff;
    for (int i = 0; i < nchannels; i++) {
        // Get |InvDmixScale|
        int scale_inv = conv_dmix_scale_inv(*coeff_ptr++ | 0x100);
        for (int j = 0; j < o->nchannels; j++) {
            // Multiply by |InvDmixScale| to get UndoDmixScale
            int coeff = mul16(conv_dmix_scale(*coeff_ptr++), scale_inv);
            if (coeff) {
                int *src = o->msb_sample_buffer[j];
                int *dst = samples[i];
                for (int k = 0; k < xll->nframesamples; k++)
                    dst[k] -= mul15(src[k] << shift, coeff);
            }
        }
    }

    return 0;
}

static int filter_hd_ma_frame(struct dcadec_context *dca)
{
    struct core_decoder *core = dca->core;
    struct xll_decoder *xll = dca->xll;
    int ret;

    // Select the first (primary) channel set
    struct xll_chset *p = &xll->chset[0];
    if (!p->primary_chset || p->replace_set_index)
        return -DCADEC_ENOSUP;

    // Filter core frame if present
    if (dca->packet & DCADEC_PACKET_CORE) {
        int flags = DCADEC_FLAG_CORE_BIT_EXACT | DCADEC_FLAG_KEEP_DMIX_6CH;
        if (p->freq == 96000 && core->sample_rate == 48000)
            flags |= DCADEC_FLAG_CORE_SYNTH_X96;
        if ((ret = core_filter(core, flags)) < 0)
            return ret;
    }

    // Process channel sets
    for_each_chset(xll, c) {
        if (c->replace_set_index)
            continue;

        xll_filter_band_data(c);

        // Check for residual encoded channel set
        if (c->residual_encode != (1 << c->nchannels) - 1) {
            if (!(dca->packet & DCADEC_PACKET_CORE))
                return -DCADEC_EINVAL;

            if (c->freq != core->output_rate)
                return -DCADEC_ENOSUP;

            if (xll->nframesamples != core->npcmsamples)
                return -DCADEC_EINVAL;

            // See if this channel set is downmixed and find the source
            // channel set. If downmixed, undo core pre-scaling before
            // combining with residual (residual is not scaled).
            struct xll_chset *o = find_hier_dmix_chset(xll, c);

            // Reduce core bit width and combine with residual
            for (int ch = 0; ch < c->nchannels; ch++) {
                if (c->residual_encode & (1 << ch))
                    continue;

                int spkr = xll_map_ch_to_spkr(c, ch);
                if (spkr < 0)
                    return -DCADEC_EINVAL;

                int core_ch = map_spkr_to_core_ch(core, spkr);
                if (core_ch < 0)
                    return -DCADEC_EINVAL;

                int shift = 24 - c->pcm_bit_res;
                // Account for LSB width
                if (xll->scalable_lsbs)
                    shift += xll_get_lsb_width(c, ch);
                int round = shift > 0 ? 1 << (shift - 1) : 0;

                int *dst = c->msb_sample_buffer[ch];
                int *src = core->output_samples[core_ch];
                if (o) {
                    // Undo embedded core downmix pre-scaling
                    int scale_inv = conv_dmix_scale_inv(o->dmix_coeff[ch * (o->nchannels + 1)]);
                    for (int n = 0; n < xll->nframesamples; n++)
                        dst[n] += clip23((mul16(src[n], scale_inv) + round) >> shift);
                } else {
                    // No downmix scaling
                    for (int n = 0; n < xll->nframesamples; n++)
                        dst[n] += (src[n] + round) >> shift;
                }
            }
        }

        // Assemble MSB and LSB parts after combining with core
        if (xll->scalable_lsbs)
            xll_assemble_msbs_lsbs(c);
    }

    int *spkr_map[SPEAKER_COUNT] = { NULL };
    int ch_mask;

    if (p->ch_mask_enabled)
        ch_mask = p->ch_mask;
    else if (p->nchannels == 2)
        ch_mask = SPEAKER_MASK_L | SPEAKER_MASK_R;
    else
        return -DCADEC_ENOSUP;

    int *samples[256];
    int nchannels = 0;

    // Start with the primary channel set
    for (int ch = 0; ch < p->nchannels; ch++) {
        int spkr = xll_map_ch_to_spkr(p, ch);
        if (spkr >= 0)
            spkr_map[spkr] = p->msb_sample_buffer[ch];
        samples[nchannels++] = p->msb_sample_buffer[ch];
    }

    // Undo embedded hierarchial downmix
    for_each_chset(xll, c) {
        if (c->replace_set_index)
            continue;
        if (!c->ch_mask_enabled)
            break;
        struct xll_chset *o = find_hier_dmix_chset(xll, c);
        if (!o)
            break;
        if ((ret = undo_down_mix(xll, c, o, samples, nchannels)) < 0)
            return ret;
        for (int ch = 0; ch < o->nchannels; ch++) {
            int spkr = xll_map_ch_to_spkr(o, ch);
            if (spkr >= 0)
                spkr_map[spkr] = o->msb_sample_buffer[ch];
            samples[nchannels++] = o->msb_sample_buffer[ch];
        }
        ch_mask |= o->ch_mask;
    }

    // Reorder sample buffer pointers
    if (reorder_samples(dca, spkr_map, ch_mask) <= 0)
        return -DCADEC_EINVAL;

    dca->nframesamples = xll->nframesamples;
    dca->sample_rate = p->freq;
    dca->bits_per_sample = p->pcm_bit_res;
    dca->profile = DCADEC_PROFILE_HD_MA;
    return 0;
}

int dcadec_context_parse(struct dcadec_context *dca, uint8_t *data, size_t size)
{
    int ret;

    if (!dca || !data || size < 4 || ((uintptr_t)data & 3))
        return -DCADEC_EINVAL;

    dca->packet = 0;

    uint32_t sync;
    sync  = data[0] << 24;
    sync |= data[1] << 16;
    sync |= data[2] <<  8;
    sync |= data[3] <<  0;

    if (sync == SYNC_WORD_CORE) {
        if (!dca->core)
            if (!(dca->core = ta_znew(dca, struct core_decoder)))
                return -DCADEC_ENOMEM;

        if ((ret = core_parse(dca->core, data, size, dca->flags, NULL)) < 0)
            return ret;

        dca->packet |= DCADEC_PACKET_CORE;

        // EXXS data must be aligned on 4-byte boundary by the caller
        size_t frame_size = (dca->core->frame_size + 3) & ~3;
        if (size - 4 > frame_size) {
            data += frame_size;
            size -= frame_size;
            sync  = data[0] << 24;
            sync |= data[1] << 16;
            sync |= data[2] <<  8;
            sync |= data[3] <<  0;
        }
    }

    ret = -DCADEC_ENOSYNC;

    if (sync == SYNC_WORD_EXSS) {
        if (!dca->exss)
            if (!(dca->exss = ta_znew(dca, struct exss_parser)))
                return -DCADEC_ENOMEM;

        if ((ret = exss_parse(dca->exss, data, size)) < 0)
            goto fail;

        dca->packet |= DCADEC_PACKET_EXSS;

        struct exss_asset *asset = &dca->exss->assets[0];

        if (!(dca->packet & DCADEC_PACKET_CORE) && (asset->extension_mask & EXSS_CORE)) {
            if (!dca->core)
                if (!(dca->core = ta_znew(dca, struct core_decoder)))
                    return -DCADEC_ENOMEM;

            if ((ret = core_parse(dca->core, data, size, dca->flags, asset)) < 0)
                return ret;

            dca->packet |= DCADEC_PACKET_CORE;
        }

        if (!(dca->flags & DCADEC_FLAG_CORE_ONLY)) {
            if ((dca->packet & DCADEC_PACKET_CORE) && (asset->extension_mask & (EXSS_XBR | EXSS_XXCH | EXSS_X96)))
                core_parse_exss(dca->core, data, size, dca->flags, asset);

            if (asset->extension_mask & EXSS_XLL) {
                if (!dca->xll)
                    if (!(dca->xll = ta_znew(dca, struct xll_decoder)))
                        return -DCADEC_ENOMEM;

                if ((ret = xll_parse(dca->xll, data, size, asset)) < 0)
                    goto fail;

                dca->packet |= DCADEC_PACKET_XLL;
            }
        }
    }

fail:
    return dca->packet ? 0 : ret;
}

int dcadec_context_core_info(struct dcadec_context *dca, int *nchannels,
                             int *lfe_present, int *sample_rate,
                             int *source_pcm_res, int *es_format,
                             int *bit_rate)
{
    if (!dca)
        return -DCADEC_EINVAL;
    if (!(dca->packet & DCADEC_PACKET_CORE))
        return -DCADEC_EINVAL;
    struct core_decoder *core = dca->core;
    if (nchannels)
        *nchannels = core->nchannels;
    if (lfe_present)
        *lfe_present = core->lfe_present;
    if (sample_rate)
        *sample_rate = core->sample_rate;
    if (source_pcm_res)
        *source_pcm_res = core->source_pcm_res;
    if (es_format)
        *es_format = core->es_format;
    if (bit_rate)
        *bit_rate = core->bit_rate;
    return 0;
}

int dcadec_context_exss_info(struct dcadec_context *dca, int *nchannels,
                             int *sample_rate, int *bits_per_sample,
                             int *profile)
{
    if (!dca)
        return -DCADEC_EINVAL;
    if (!(dca->packet & DCADEC_PACKET_EXSS))
        return -DCADEC_EINVAL;
    struct exss_asset *asset = &dca->exss->assets[0];
    if (nchannels)
        *nchannels = asset->nchannels_total;
    if (sample_rate)
        *sample_rate = asset->max_sample_rate;
    if (bits_per_sample)
        *bits_per_sample = asset->pcm_bit_res;
    if (profile) {
        if (asset->extension_mask & EXSS_XLL)
            *profile = DCADEC_PROFILE_HD_MA;
        else if (asset->extension_mask & (EXSS_XBR | EXSS_XXCH | EXSS_X96))
            *profile = DCADEC_PROFILE_HD_HRA;
        else if (asset->extension_mask & EXSS_LBR)
            *profile = DCADEC_PROFILE_EXPRESS;
        else
            *profile = DCADEC_PROFILE_UNKNOWN;
    }
    return 0;
}

int dcadec_context_filter(struct dcadec_context *dca, int ***samples,
                          int *nsamples, int *channel_mask, int *sample_rate,
                          int *bits_per_sample, int *profile)
{
    int ret;

    if (!dca)
        return -DCADEC_EINVAL;

    if (dca->packet & DCADEC_PACKET_XLL) {
        if ((ret = filter_hd_ma_frame(dca)) < 0)
            return ret;
    } else if (dca->packet & DCADEC_PACKET_CORE) {
        if ((ret = filter_core_frame(dca)) < 0)
            return ret;
    } else {
        return -DCADEC_EINVAL;
    }

    if (samples)
        *samples = dca->samples;
    if (nsamples)
        *nsamples = dca->nframesamples;
    if (channel_mask)
        *channel_mask = dca->channel_mask;
    if (sample_rate)
        *sample_rate = dca->sample_rate;
    if (bits_per_sample)
        *bits_per_sample = dca->bits_per_sample;
    if (profile)
        *profile = dca->profile;
    return 0;
}

void dcadec_context_clear(struct dcadec_context *dca)
{
    if (dca) {
        core_clear(dca->core);
        xll_clear(dca->xll);
    }
}

struct dcadec_context *dcadec_context_create(int flags)
{
    struct dcadec_context *dca = ta_znew(NULL, struct dcadec_context);
    if (dca)
        dca->flags = flags;
    return dca;
}

void dcadec_context_destroy(struct dcadec_context *dca)
{
    ta_free(dca);
}