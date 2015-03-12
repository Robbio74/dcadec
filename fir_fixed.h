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

//  floor(fir_float.band_fir_perfect[i] * SQRT2 * (1 << 28) + 0.5), (i & 63) <  16
// -floor(fir_float.band_fir_perfect[i] * SQRT2 * (1 << 28) + 0.5), (i & 63) >= 16
static const int32_t band_fir_perfect[512] = {
           0,        0,       -3,      -10,
         -35,     -105,     -218,     -141,
        -170,     -216,     -239,     -254,
        -257,     -251,     -235,     -212,
        -267,     -317,     -362,     -400,
        -425,     -434,     -427,     -373,
        -339,     -593,     -321,     -120,
         -39,      -16,        0,        1,
           1,        1,       -3,       -1,
          -6,      -38,      -93,     -496,
        -723,     -970,    -1235,    -1501,
       -1753,    -1978,    -2163,    -2295,
       -2891,    -2915,    -2860,    -2726,
       -2517,    -2243,    -1915,    -1590,
       -1192,     -252,     -117,      -22,
          -6,      -13,       12,       14,
          32,       25,      469,      942,
        1403,     1421,     1239,     2838,
        3539,     4259,     5002,     5716,
        6365,     6908,     7311,     7545,
       11680,    12355,    12785,    12951,
       12841,    12453,    11803,    10864,
        9762,     7099,     6725,     5954,
        4284,     2584,      215,      379,
         557,      701,      -29,     -687,
       -1578,    -2749,    -4076,    -7933,
      -10049,   -12133,   -14039,   -15752,
      -17213,   -18400,   -19291,   -19878,
       -1444,    -3005,    -4523,    -5927,
       -7143,    -8093,    -8713,    -8939,
       -8700,    -9481,    -7515,    -5279,
       -2725,       61,     5763,     6113,
        7571,     6735,    17126,    20165,
       23328,    26775,    30310,    32639,
       35464,    38064,    40423,    42512,
       44261,    45632,    46578,    46974,
      -45572,   -45008,   -43753,   -41661,
      -38655,   -34660,   -29587,   -23375,
      -15998,    -7631,     2472,    13757,
       26188,    39942,    49789,    67293,
       84699,   101701,   127325,   148404,
      170391,   193280,   217044,   241451,
      266537,   292144,   318161,   344417,
      370786,   397082,   423133,   448757,
      475085,   499136,   522007,   543516,
      563424,   581467,   597422,   611005,
      621975,   630083,   634996,   636457,
      634311,   628147,   619871,   604524,
      585077,   561301,   529204,   494129,
      453552,   407189,   354920,   296502,
      231916,   161012,    83700,      -86,
      -90377,  -187193,  -290528,  -400329,
      516487,   639054,   767835,   902710,
     1043512,  1190048,  1342100,  1499418,
     1661729,  1828700,  2000071,  2175433,
     2354437,  2536630,  2721120,  2908704,
     3098059,  3288764,  3480801,  3672922,
     3864970,  4056432,  4246767,  4435454,
     4621921,  4805642,  4986073,  5162677,
     5334921,  5502279,  5664239,  5820300,
     5969913,  6112723,  6248225,  6375985,
     6495593,  6606663,  6708832,  6801769,
     6885168,  6958762,  7022294,  7075566,
     7118382,  7150633,  7172314,  7183082,
     7183082,  7172314,  7150633,  7118382,
     7075566,  7022294,  6958762,  6885168,
     6801769,  6708832,  6606663,  6495593,
     6375985,  6248225,  6112723,  5969913,
    -5820300, -5664239, -5502279, -5334921,
    -5162677, -4986073, -4805642, -4621921,
    -4435454, -4246767, -4056432, -3864970,
    -3672922, -3480801, -3288764, -3098059,
    -2908704, -2721120, -2536630, -2354437,
    -2175433, -2000071, -1828700, -1661729,
    -1499418, -1342100, -1190048, -1043512,
     -902710,  -767835,  -639054,  -516487,
     -400329,  -290528,  -187193,   -90377,
         -86,    83700,   161012,   231916,
      296502,   354920,   407189,   453552,
      494129,   529204,   561301,   585077,
      604524,   619871,   628147,   634311,
      636457,   634996,   630083,   621975,
      611005,   597422,   581467,   563424,
      543516,   522007,   499136,   475085,
     -448757,  -423133,  -397082,  -370786,
     -344417,  -318161,  -292144,  -266537,
     -241451,  -217044,  -193280,  -170391,
     -148404,  -127325,  -101701,   -84699,
      -67293,   -49789,   -39942,   -26188,
      -13757,    -2472,     7631,    15998,
       23375,    29587,    34660,    38655,
       41661,    43753,    45008,    45572,
       46974,    46578,    45632,    44261,
       42512,    40423,    38064,    35464,
       32639,    30310,    26775,    23328,
       20165,    17126,     6735,     7571,
        6113,     5763,       61,    -2725,
       -5279,    -7515,    -9481,    -8700,
       -8939,    -8713,    -8093,    -7143,
       -5927,    -4523,    -3005,    -1444,
       19878,    19291,    18400,    17213,
       15752,    14039,    12133,    10049,
        7933,     4076,     2749,     1578,
         687,       29,     -701,     -557,
        -379,     -215,    -2584,    -4284,
       -5954,    -6725,    -7099,    -9762,
      -10864,   -11803,   -12453,   -12841,
      -12951,   -12785,   -12355,   -11680,
        7545,     7311,     6908,     6365,
        5716,     5002,     4259,     3539,
        2838,     1239,     1421,     1403,
         942,      469,       25,       32,
          14,       12,      -13,       -6,
         -22,     -117,     -252,    -1192,
       -1590,    -1915,    -2243,    -2517,
       -2726,    -2860,    -2915,    -2891,
        2295,     2163,     1978,     1753,
        1501,     1235,      970,      723,
         496,       93,       38,        6,
           1,        3,       -1,       -1,
          -1,        0,       16,       39,
         120,      321,      593,      339,
         373,      427,      434,      425,
         400,      362,      317,      267,
        -212,     -235,     -251,     -257,
        -254,     -239,     -216,     -170,
        -141,     -218,     -105,      -35,
         -10,       -3,        0,        0
};

//  floor(fir_float.band_fir_nonperfect[i] * SQRT2 * (1 << 28) + 0.5), (i & 63) <  16
// -floor(fir_float.band_fir_nonperfect[i] * SQRT2 * (1 << 28) + 0.5), (i & 63) >= 16
static const int32_t band_fir_nonperfect[512] = {
         -53,      -64,      -77,      -91,
        -107,     -124,     -144,     -165,
        -189,     -215,     -244,     -277,
        -313,     -353,     -397,     -447,
         502,      563,      631,      706,
         789,      881,      983,     1095,
        1218,     1354,     1502,     1665,
        1843,     2036,     2247,     2475,
        2723,     2990,     3277,     3586,
        3916,     4270,     4646,     5046,
        5470,     5918,     6390,     6886,
        7405,     7947,     8510,     9094,
        9698,    10319,    10955,    11605,
       12265,    12933,    13605,    14277,
       14945,    15604,    16250,    16877,
       17480,    18051,    18585,    19075,
       19513,    19891,    20202,    20436,
       20587,    20643,    20597,    20439,
       20160,    19749,    19198,    18496,
       17634,    16603,    15393,    13996,
      -12403,   -10605,    -8595,    -6366,
       -3911,    -1225,     1697,     4860,
        8265,    11916,    15812,    19953,
       24337,    28961,    33819,    38904,
       44210,    49725,    55437,    61334,
       67398,    73614,    79961,    86417,
       92960,    99563,   106198,   112837,
      119446,   125994,   132443,   138758,
      144898,   150823,   156491,   161858,
      166879,   171507,   175697,   179400,
      182566,   185149,   187097,   188363,
      188899,   188654,   187581,   185635,
      182770,   178943,   174112,   168238,
      161285,   153218,   144007,   133624,
      122046,   109254,    95232,    79969,
       63462,    45709,    26715,     6492,
       14943,    37567,    61350,    86256,
      112242,   139258,   167246,   196143,
      225877,   256368,   287532,   319275,
      351496,   384088,   416936,   449919,
      482909,   515770,   548362,   580539,
      612148,   643030,   673024,   701963,
      729674,   755985,   780717,   803690,
      824721,   843628,   860226,   874332,
      885761,   894330,   899861,   902174,
      901096,   896456,   888088,   875832,
      859535,   839050,   814237,   784966,
      751116,   712574,   669239,   621021,
      567840,   509632,   446341,   377927,
      304365,   225641,   141757,    52732,
      -41403,  -140599,  -244793,  -353905,
     -467840,  -586486,  -709716,  -837385,
      969336,  1105393,  1245366,  1389049,
     1536224,  1686655,  1840096,  1996285,
     2154949,  2315802,  2478547,  2642877,
     2808475,  2975015,  3142163,  3309579,
     3476914,  3643818,  3809934,  3974901,
     4138360,  4299948,  4459303,  4616064,
     4769873,  4920374,  5067219,  5210063,
     5348569,  5482406,  5611255,  5734805,
     5852757,  5964823,  6070729,  6170216,
     6263037,  6348961,  6427777,  6499286,
     6563310,  6619688,  6668279,  6708963,
     6741632,  6766206,  6782623,  6790843,
     6790843,  6782623,  6766206,  6741632,
     6708963,  6668279,  6619688,  6563310,
     6499286,  6427777,  6348961,  6263037,
     6170216,  6070729,  5964823,  5852757,
    -5734805, -5611255, -5482406, -5348569,
    -5210063, -5067219, -4920374, -4769873,
    -4616064, -4459303, -4299948, -4138360,
    -3974901, -3809934, -3643818, -3476914,
    -3309579, -3142163, -2975015, -2808475,
    -2642877, -2478547, -2315802, -2154949,
    -1996285, -1840096, -1686655, -1536224,
    -1389049, -1245366, -1105393,  -969336,
     -837385,  -709716,  -586486,  -467840,
     -353905,  -244793,  -140599,   -41403,
       52732,   141757,   225641,   304365,
      377927,   446341,   509632,   567840,
      621021,   669239,   712574,   751116,
      784966,   814237,   839050,   859535,
      875832,   888088,   896456,   901096,
      902174,   899861,   894330,   885761,
     -874332,  -860226,  -843628,  -824721,
     -803690,  -780717,  -755985,  -729674,
     -701963,  -673024,  -643030,  -612148,
     -580539,  -548362,  -515770,  -482909,
     -449919,  -416936,  -384088,  -351496,
     -319275,  -287532,  -256368,  -225877,
     -196143,  -167246,  -139258,  -112242,
      -86256,   -61350,   -37567,   -14943,
        6492,    26715,    45709,    63462,
       79969,    95232,   109254,   122046,
      133624,   144007,   153218,   161285,
      168238,   174112,   178943,   182770,
      185635,   187581,   188654,   188899,
      188363,   187097,   185149,   182566,
      179400,   175697,   171507,   166879,
      161858,   156491,   150823,   144898,
     -138758,  -132443,  -125994,  -119446,
     -112837,  -106198,   -99563,   -92960,
      -86417,   -79961,   -73614,   -67398,
      -61334,   -55437,   -49725,   -44210,
      -38904,   -33819,   -28961,   -24337,
      -19953,   -15812,   -11916,    -8265,
       -4860,    -1697,     1225,     3911,
        6366,     8595,    10605,    12403,
       13996,    15393,    16603,    17634,
       18496,    19198,    19749,    20160,
       20439,    20597,    20643,    20587,
       20436,    20202,    19891,    19513,
       19075,    18585,    18051,    17480,
       16877,    16250,    15604,    14945,
       14277,    13605,    12933,    12265,
       11605,    10955,    10319,     9698,
       -9094,    -8510,    -7947,    -7405,
       -6886,    -6390,    -5918,    -5470,
       -5046,    -4646,    -4270,    -3916,
       -3586,    -3277,    -2990,    -2723,
       -2475,    -2247,    -2036,    -1843,
       -1665,    -1502,    -1354,    -1218,
       -1095,     -983,     -881,     -789,
        -706,     -631,     -563,     -502,
        -447,     -397,     -353,     -313,
        -277,     -244,     -215,     -189,
        -165,     -144,     -124,     -107,
         -91,      -77,      -64,      -53
};

static const int32_t lfe_fir_64[512] = {
       6103,    1930,    2228,    2552,    2904,    3287,    3704,    4152,
       4631,    5139,    5682,    6264,    6886,    7531,    8230,    8959,
       9727,   10535,   11381,   12267,   13190,   14152,   15153,   16192,
      17267,   18377,   19525,   20704,   21915,   23157,   24426,   25721,
      27038,   28376,   29732,   31101,   32480,   33866,   35256,   36642,
      38022,   39391,   40743,   42072,   43373,   44640,   45866,   47044,
      48168,   49231,   50224,   51140,   51972,   52711,   53348,   53876,
      54285,   54566,   54710,   54708,   54550,   54228,   53729,   53047,
      52170,   51089,   49794,   48275,   46523,   44529,   42282,   39774,
      36995,   33937,   30591,   26948,   23001,   18741,   14162,    9257,
       4018,   -1558,   -7480,  -13750,  -20372,  -27352,  -34691,  -42390,
     -50453,  -58879,  -67667,  -76817,  -86327,  -96193, -106412, -116977,
    -127884, -139125, -150692, -162575, -174764, -187246, -200010, -213041,
    -226323, -239840, -253574, -267505, -281613, -295877, -310273, -324777,
    -339362, -354003, -368671, -383335, -397966, -412531, -426998, -441330,
    -455494, -469451, -483164, -496595, -509702, -522445, -534782, -546669,
    -558064, -568920, -579194, -588839, -597808, -606054, -613529, -620186,
    -625976, -630850, -634759, -637655, -639488, -640210, -639772, -638125,
    -635222, -631014, -625455, -618499, -610098, -600209, -588788, -575791,
    -561178, -544906, -526937, -507233, -485757, -462476, -437356, -410365,
    -381475, -350658, -317890, -283146, -246406, -207653, -166869, -124041,
     -79159,  -32214,   16800,   67886,  121042,  176267,  233555,  292897,
     354281,  417692,  483113,  550521,  619894,  691203,  764419,  839507,
     916430,  995149, 1075621, 1157798, 1241632, 1327070, 1414057, 1502534,
    1592440, 1683709, 1776276, 1870070, 1965017, 2061044, 2158071, 2256019,
    2354805, 2454343, 2554547, 2655326, 2756591, 2858248, 2960201, 3062355,
    3164612, 3266872, 3369035, 3471000, 3572664, 3673924, 3774676, 3874816,
    3974239, 4072841, 4170517, 4267162, 4362672, 4456942, 4549871, 4641355,
    4731293, 4819584, 4906129, 4990831, 5073593, 5154321, 5232922, 5309305,
    5383383, 5455069, 5524280, 5590933, 5654952, 5716260, 5774785, 5830457,
    5883210, 5932981, 5979711, 6023343, 6063824, 6101107, 6135146, 6165900,
    6193332, 6217408, 6238099, 6255380, 6269231, 6279634, 6286575, 6290049,
    6290049, 6286575, 6279634, 6269231, 6255380, 6238099, 6217408, 6193332,
    6165900, 6135146, 6101107, 6063824, 6023343, 5979711, 5932981, 5883210,
    5830457, 5774785, 5716260, 5654952, 5590933, 5524280, 5455069, 5383383,
    5309305, 5232922, 5154321, 5073593, 4990831, 4906129, 4819584, 4731293,
    4641355, 4549871, 4456942, 4362672, 4267162, 4170517, 4072841, 3974239,
    3874816, 3774676, 3673924, 3572664, 3471000, 3369035, 3266872, 3164612,
    3062355, 2960201, 2858248, 2756591, 2655326, 2554547, 2454343, 2354805,
    2256019, 2158071, 2061044, 1965017, 1870070, 1776276, 1683709, 1592440,
    1502534, 1414057, 1327070, 1241632, 1157798, 1075621,  995149,  916430,
     839507,  764419,  691203,  619894,  550521,  483113,  417692,  354281,
     292897,  233555,  176267,  121042,   67886,   16800,  -32214,  -79159,
    -124041, -166869, -207653, -246406, -283146, -317890, -350658, -381475,
    -410365, -437356, -462476, -485757, -507233, -526937, -544906, -561178,
    -575791, -588788, -600209, -610098, -618499, -625455, -631014, -635222,
    -638125, -639772, -640210, -639488, -637655, -634759, -630850, -625976,
    -620186, -613529, -606054, -597808, -588839, -579194, -568920, -558064,
    -546669, -534782, -522445, -509702, -496595, -483164, -469451, -455494,
    -441330, -426998, -412531, -397966, -383335, -368671, -354003, -339362,
    -324777, -310273, -295877, -281613, -267505, -253574, -239840, -226323,
    -213041, -200010, -187246, -174764, -162575, -150692, -139125, -127884,
    -116977, -106412,  -96193,  -86327,  -76817,  -67667,  -58879,  -50453,
     -42390,  -34691,  -27352,  -20372,  -13750,   -7480,   -1558,    4018,
       9257,   14162,   18741,   23001,   26948,   30591,   33937,   36995,
      39774,   42282,   44529,   46523,   48275,   49794,   51089,   52170,
      53047,   53729,   54228,   54550,   54708,   54710,   54566,   54285,
      53876,   53348,   52711,   51972,   51140,   50224,   49231,   48168,
      47044,   45866,   44640,   43373,   42072,   40743,   39391,   38022,
      36642,   35256,   33866,   32480,   31101,   29732,   28376,   27038,
      25721,   24426,   23157,   21915,   20704,   19525,   18377,   17267,
      16192,   15153,   14152,   13190,   12267,   11381,   10535,    9727,
       8959,    8230,    7531,    6886,    6264,    5682,    5139,    4631,
       4152,    3704,    3287,    2904,    2552,    2228,    1930,    6103
};

//  floor(fir_float.band_fir_x96[i] * (1 << 29) + 0.5), (i & 127) <  32
// -floor(fir_float.band_fir_x96[i] * (1 << 29) + 0.5), (i & 127) >= 32
static const int32_t band_fir_x96[1024] = {
         -38,      -38,      -43,      -48,
         -52,      -57,      -62,      -67,
         -73,      -79,      -85,      -91,
         -98,     -105,     -113,     -121,
        -129,     -138,     -147,     -157,
        -167,     -178,     -190,     -202,
        -215,     -228,     -242,     -257,
        -273,     -289,     -307,     -325,
         345,      365,      387,      410,
         433,      459,      485,      513,
         543,      574,      606,      640,
         676,      714,      753,      795,
         839,      884,      932,      983,
        1035,     1090,     1148,     1208,
        1271,     1336,     1405,     1476,
        1550,     1628,     1709,     1793,
        1880,     1971,     2065,     2163,
        2265,     2370,     2479,     2592,
        2709,     2830,     2955,     3084,
        3217,     3354,     3496,     3642,
        3791,     3946,     4104,     4267,
        4433,     4604,     4780,     4959,
        5143,     5330,     5522,     5717,
        5916,     6119,     6326,     6536,
        6749,     6966,     7186,     7408,
        7633,     7861,     8090,     8322,
        8556,     8791,     9027,     9264,
        9501,     9739,     9977,    10214,
       10450,    10685,    10918,    11149,
       11377,    11603,    11825,    12042,
       12255,    12463,    12665,    12861,
       13050,    13231,    13405,    13569,
       13724,    13869,    14002,    14125,
       14235,    14331,    14415,    14483,
       14536,    14573,    14594,    14596,
       14580,    14544,    14488,    14412,
       14313,    14191,    14046,    13877,
       13682,    13461,    13213,    12937,
       12632,    12298,    11934,    11538,
       11111,    10650,    10156,     9628,
       -9065,    -8466,    -7830,    -7158,
       -6447,    -5698,    -4910,    -4083,
       -3215,    -2306,    -1357,     -366,
         668,     1743,     2861,     4022,
        5226,     6474,     7764,     9098,
       10476,    11897,    13361,    14868,
       16418,    18011,    19645,    21322,
       23039,    24798,    26596,    28433,
       30309,    32222,    34172,    36158,
       38177,    40231,    42315,    44431,
       46575,    48747,    50945,    53167,
       55411,    57676,    59959,    62258,
       64571,    66897,    69231,    71573,
       73919,    76268,    78615,    80959,
       83296,    85624,    87939,    90239,
       92519,    94778,    97011,    99215,
      101386,   103521,   105616,   107668,
      109673,   111626,   113524,   115362,
      117138,   118847,   120484,   122045,
      123527,   124925,   126234,   127451,
      128571,   129591,   130504,   131308,
      131997,   132568,   133016,   133338,
      133528,   133582,   133495,   133265,
      132886,   132355,   131668,   130820,
      129807,   128626,   127274,   125746,
      124038,   122148,   120071,   117806,
      115348,   112694,   109843,   106790,
      103534,   100071,    96401,    92520,
       88426,    84119,    79597,    74857,
       69900,    64723,    59327,    53711,
       47875,    41818,    35542,    29045,
       22330,    15397,     8247,      881,
        6697,    14487,    22487,    30692,
       39101,    47711,    56517,    65516,
       74704,    84076,    93628,   103355,
      113251,   123311,   133528,   143897,
      154410,   165061,   175843,   186747,
      197766,   208892,   220116,   231429,
      242822,   254285,   265810,   277384,
      288999,   300644,   312306,   323976,
      335641,   347289,   358909,   370488,
      382013,   393471,   404848,   416133,
      427310,   438366,   449286,   460057,
      470663,   481090,   491323,   501347,
      511147,   520707,   530011,   539044,
      547790,   556233,   564357,   572146,
      579584,   586654,   593341,   599627,
      605498,   610936,   615925,   620449,
      624491,   628037,   631069,   633571,
      635529,   636925,   637745,   637972,
      637593,   636592,   634953,   632662,
      629705,   626068,   621737,   616698,
      610938,   604443,   597202,   589202,
      580431,   570877,   560530,   549378,
      537411,   524620,   510994,   496525,
      481205,   465026,   447979,   430058,
      411256,   391569,   370989,   349514,
      327137,   303857,   279670,   254573,
      228564,   201644,   173811,   145065,
      115408,    84840,    53365,    20984,
      -12298,   -46478,   -81550,  -117508,
     -154347,  -192060,  -230638,  -270073,
     -310356,  -351478,  -393427,  -436192,
     -479762,  -524124,  -569264,  -615168,
      661821,   709209,   757314,   806121,
      855611,   905766,   956569,  1007998,
     1060035,  1112658,  1165846,  1219578,
     1273830,  1328580,  1383805,  1439479,
     1495578,  1552077,  1608950,  1666171,
     1723714,  1781550,  1839653,  1897995,
     1956546,  2015279,  2074163,  2133170,
     2192270,  2251432,  2310626,  2369822,
     2428988,  2488093,  2547106,  2605996,
     2664731,  2723279,  2781607,  2839685,
     2897481,  2954962,  3012096,  3068851,
     3125195,  3181097,  3236524,  3291445,
     3345829,  3399643,  3452858,  3505441,
     3557362,  3608591,  3659098,  3708853,
     3757825,  3805987,  3853309,  3899763,
     3945322,  3989957,  4033642,  4076350,
     4118055,  4158733,  4198357,  4236904,
     4274351,  4310673,  4345850,  4379859,
     4412678,  4444289,  4474670,  4503803,
     4531671,  4558255,  4583539,  4607508,
     4630146,  4651438,  4671373,  4689936,
     4707117,  4722905,  4737290,  4750262,
     4761813,  4771936,  4780625,  4787874,
     4793679,  4798036,  4800943,  4802396,
     4802396,  4800943,  4798036,  4793679,
     4787874,  4780625,  4771936,  4761813,
     4750262,  4737290,  4722905,  4707117,
     4689936,  4671373,  4651438,  4630146,
     4607508,  4583539,  4558255,  4531671,
     4503803,  4474670,  4444289,  4412678,
     4379859,  4345850,  4310673,  4274351,
     4236904,  4198357,  4158733,  4118055,
    -4076350, -4033642, -3989957, -3945322,
    -3899763, -3853309, -3805987, -3757825,
    -3708853, -3659098, -3608591, -3557362,
    -3505441, -3452858, -3399643, -3345829,
    -3291445, -3236524, -3181097, -3125195,
    -3068851, -3012096, -2954962, -2897481,
    -2839685, -2781607, -2723279, -2664731,
    -2605996, -2547106, -2488093, -2428988,
    -2369822, -2310626, -2251432, -2192270,
    -2133170, -2074163, -2015279, -1956546,
    -1897995, -1839653, -1781550, -1723714,
    -1666171, -1608950, -1552077, -1495578,
    -1439479, -1383805, -1328580, -1273830,
    -1219578, -1165846, -1112658, -1060035,
    -1007998,  -956569,  -905766,  -855611,
     -806121,  -757314,  -709209,  -661821,
     -615168,  -569264,  -524124,  -479762,
     -436192,  -393427,  -351478,  -310356,
     -270073,  -230638,  -192060,  -154347,
     -117508,   -81550,   -46478,   -12298,
       20984,    53365,    84840,   115408,
      145065,   173811,   201644,   228564,
      254573,   279670,   303857,   327137,
      349514,   370989,   391569,   411256,
      430058,   447979,   465026,   481205,
      496525,   510994,   524620,   537411,
      549378,   560530,   570877,   580431,
      589202,   597202,   604443,   610938,
      616698,   621737,   626068,   629705,
      632662,   634953,   636592,   637593,
      637972,   637745,   636925,   635529,
      633571,   631069,   628037,   624491,
     -620449,  -615925,  -610936,  -605498,
     -599627,  -593341,  -586654,  -579584,
     -572146,  -564357,  -556233,  -547790,
     -539044,  -530011,  -520707,  -511147,
     -501347,  -491323,  -481090,  -470663,
     -460057,  -449286,  -438366,  -427310,
     -416133,  -404848,  -393471,  -382013,
     -370488,  -358909,  -347289,  -335641,
     -323976,  -312306,  -300644,  -288999,
     -277384,  -265810,  -254285,  -242822,
     -231429,  -220116,  -208892,  -197766,
     -186747,  -175843,  -165061,  -154410,
     -143897,  -133528,  -123311,  -113251,
     -103355,   -93628,   -84076,   -74704,
      -65516,   -56517,   -47711,   -39101,
      -30692,   -22487,   -14487,    -6697,
         881,     8247,    15397,    22330,
       29045,    35542,    41818,    47875,
       53711,    59327,    64723,    69900,
       74857,    79597,    84119,    88426,
       92520,    96401,   100071,   103534,
      106790,   109843,   112694,   115348,
      117806,   120071,   122148,   124038,
      125746,   127274,   128626,   129807,
      130820,   131668,   132355,   132886,
      133265,   133495,   133582,   133528,
      133338,   133016,   132568,   131997,
      131308,   130504,   129591,   128571,
      127451,   126234,   124925,   123527,
      122045,   120484,   118847,   117138,
      115362,   113524,   111626,   109673,
      107668,   105616,   103521,   101386,
      -99215,   -97011,   -94778,   -92519,
      -90239,   -87939,   -85624,   -83296,
      -80959,   -78615,   -76268,   -73919,
      -71573,   -69231,   -66897,   -64571,
      -62258,   -59959,   -57676,   -55411,
      -53167,   -50945,   -48747,   -46575,
      -44431,   -42315,   -40231,   -38177,
      -36158,   -34172,   -32222,   -30309,
      -28433,   -26596,   -24798,   -23039,
      -21322,   -19645,   -18011,   -16418,
      -14868,   -13361,   -11897,   -10476,
       -9098,    -7764,    -6474,    -5226,
       -4022,    -2861,    -1743,     -668,
         366,     1357,     2306,     3215,
        4083,     4910,     5698,     6447,
        7158,     7830,     8466,     9065,
        9628,    10156,    10650,    11111,
       11538,    11934,    12298,    12632,
       12937,    13213,    13461,    13682,
       13877,    14046,    14191,    14313,
       14412,    14488,    14544,    14580,
       14596,    14594,    14573,    14536,
       14483,    14415,    14331,    14235,
       14125,    14002,    13869,    13724,
       13569,    13405,    13231,    13050,
       12861,    12665,    12463,    12255,
       12042,    11825,    11603,    11377,
       11149,    10918,    10685,    10450,
       10214,     9977,     9739,     9501,
        9264,     9027,     8791,     8556,
        8322,     8090,     7861,     7633,
        7408,     7186,     6966,     6749,
       -6536,    -6326,    -6119,    -5916,
       -5717,    -5522,    -5330,    -5143,
       -4959,    -4780,    -4604,    -4433,
       -4267,    -4104,    -3946,    -3791,
       -3642,    -3496,    -3354,    -3217,
       -3084,    -2955,    -2830,    -2709,
       -2592,    -2479,    -2370,    -2265,
       -2163,    -2065,    -1971,    -1880,
       -1793,    -1709,    -1628,    -1550,
       -1476,    -1405,    -1336,    -1271,
       -1208,    -1148,    -1090,    -1035,
        -983,     -932,     -884,     -839,
        -795,     -753,     -714,     -676,
        -640,     -606,     -574,     -543,
        -513,     -485,     -459,     -433,
        -410,     -387,     -365,     -345,
        -325,     -307,     -289,     -273,
        -257,     -242,     -228,     -215,
        -202,     -190,     -178,     -167,
        -157,     -147,     -138,     -129,
        -121,     -113,     -105,      -98,
         -91,      -85,      -79,      -73,
         -67,      -62,      -57,      -52,
         -48,      -43,      -38,      -38
};
