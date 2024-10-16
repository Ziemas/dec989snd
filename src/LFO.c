#include "common.h"
#include "functions.h"

/* data 1e8 */ SInt16 gLFO_sine[2048] = {
    32767,  32766,  32766,  32765,  32764,  32763,  32761,  32759,  32757,  32754,  32751,  32748,  32744,  32740,
    32736,  32732,  32727,  32722,  32717,  32711,  32705,  32699,  32692,  32685,  32678,  32670,  32662,  32654,
    32646,  32637,  32628,  32618,  32609,  32599,  32588,  32578,  32567,  32556,  32544,  32532,  32520,  32508,
    32495,  32482,  32468,  32455,  32441,  32426,  32412,  32397,  32382,  32366,  32350,  32334,  32318,  32301,
    32284,  32267,  32249,  32231,  32213,  32194,  32176,  32156,  32137,  32117,  32097,  32077,  32056,  32035,
    32014,  31992,  31970,  31948,  31926,  31903,  31880,  31856,  31833,  31809,  31785,  31760,  31735,  31710,
    31684,  31659,  31633,  31606,  31580,  31553,  31525,  31498,  31470,  31442,  31413,  31385,  31356,  31326,
    31297,  31267,  31236,  31206,  31175,  31144,  31113,  31081,  31049,  31017,  30984,  30951,  30918,  30885,
    30851,  30817,  30783,  30748,  30713,  30678,  30643,  30607,  30571,  30535,  30498,  30461,  30424,  30386,
    30349,  30311,  30272,  30234,  30195,  30156,  30116,  30076,  30036,  29996,  29955,  29915,  29873,  29832,
    29790,  29748,  29706,  29663,  29621,  29577,  29534,  29490,  29446,  29402,  29358,  29313,  29268,  29222,
    29177,  29131,  29085,  29038,  28992,  28945,  28897,  28850,  28802,  28754,  28706,  28657,  28608,  28559,
    28510,  28460,  28410,  28360,  28309,  28259,  28208,  28156,  28105,  28053,  28001,  27948,  27896,  27843,
    27790,  27736,  27683,  27629,  27575,  27520,  27466,  27411,  27355,  27300,  27244,  27188,  27132,  27076,
    27019,  26962,  26905,  26847,  26789,  26731,  26673,  26615,  26556,  26497,  26437,  26378,  26318,  26258,
    26198,  26137,  26077,  26016,  25954,  25893,  25831,  25769,  25707,  25645,  25582,  25519,  25456,  25392,
    25329,  25265,  25201,  25136,  25072,  25007,  24942,  24877,  24811,  24745,  24679,  24613,  24546,  24480,
    24413,  24346,  24278,  24211,  24143,  24075,  24006,  23938,  23869,  23800,  23731,  23661,  23592,  23522,
    23452,  23382,  23311,  23240,  23169,  23098,  23027,  22955,  22883,  22811,  22739,  22666,  22594,  22521,
    22448,  22374,  22301,  22227,  22153,  22079,  22004,  21930,  21855,  21780,  21705,  21629,  21554,  21478,
    21402,  21326,  21249,  21173,  21096,  21019,  20942,  20864,  20787,  20709,  20631,  20553,  20474,  20396,
    20317,  20238,  20159,  20079,  20000,  19920,  19840,  19760,  19680,  19599,  19519,  19438,  19357,  19276,
    19194,  19113,  19031,  18949,  18867,  18785,  18702,  18620,  18537,  18454,  18371,  18287,  18204,  18120,
    18036,  17952,  17868,  17784,  17699,  17615,  17530,  17445,  17360,  17274,  17189,  17103,  17017,  16931,
    16845,  16759,  16672,  16586,  16499,  16412,  16325,  16238,  16150,  16063,  15975,  15887,  15799,  15711,
    15623,  15534,  15446,  15357,  15268,  15179,  15090,  15001,  14911,  14822,  14732,  14642,  14552,  14462,
    14372,  14281,  14191,  14100,  14009,  13918,  13827,  13736,  13645,  13553,  13462,  13370,  13278,  13186,
    13094,  13002,  12909,  12817,  12724,  12632,  12539,  12446,  12353,  12260,  12166,  12073,  11980,  11886,
    11792,  11698,  11604,  11510,  11416,  11322,  11227,  11133,  11038,  10944,  10849,  10754,  10659,  10564,
    10469,  10373,  10278,  10182,  10087,  9991,   9895,   9799,   9703,   9607,   9511,   9415,   9319,   9222,
    9126,   9029,   8932,   8836,   8739,   8642,   8545,   8448,   8351,   8253,   8156,   8059,   7961,   7864,
    7766,   7668,   7571,   7473,   7375,   7277,   7179,   7081,   6982,   6884,   6786,   6688,   6589,   6491,
    6392,   6293,   6195,   6096,   5997,   5898,   5799,   5700,   5601,   5502,   5403,   5304,   5205,   5106,
    5006,   4907,   4807,   4708,   4608,   4509,   4409,   4310,   4210,   4110,   4011,   3911,   3811,   3711,
    3611,   3511,   3411,   3311,   3211,   3111,   3011,   2911,   2811,   2711,   2610,   2510,   2410,   2310,
    2209,   2109,   2009,   1908,   1808,   1708,   1607,   1507,   1406,   1306,   1206,   1105,   1005,   904,
    804,    703,    603,    502,    402,    301,    201,    100,    0,      -100,   -201,   -301,   -402,   -502,
    -603,   -703,   -804,   -904,   -1005,  -1105,  -1206,  -1306,  -1406,  -1507,  -1607,  -1708,  -1808,  -1908,
    -2009,  -2109,  -2209,  -2310,  -2410,  -2510,  -2610,  -2711,  -2811,  -2911,  -3011,  -3111,  -3211,  -3311,
    -3411,  -3511,  -3611,  -3711,  -3811,  -3911,  -4011,  -4110,  -4210,  -4310,  -4409,  -4509,  -4608,  -4708,
    -4807,  -4907,  -5006,  -5106,  -5205,  -5304,  -5403,  -5502,  -5601,  -5700,  -5799,  -5898,  -5997,  -6096,
    -6195,  -6293,  -6392,  -6491,  -6589,  -6688,  -6786,  -6884,  -6982,  -7081,  -7179,  -7277,  -7375,  -7473,
    -7571,  -7668,  -7766,  -7864,  -7961,  -8059,  -8156,  -8253,  -8351,  -8448,  -8545,  -8642,  -8739,  -8836,
    -8932,  -9029,  -9126,  -9222,  -9319,  -9415,  -9511,  -9607,  -9703,  -9799,  -9895,  -9991,  -10087, -10182,
    -10278, -10373, -10469, -10564, -10659, -10754, -10849, -10944, -11038, -11133, -11227, -11322, -11416, -11510,
    -11604, -11698, -11792, -11886, -11980, -12073, -12166, -12260, -12353, -12446, -12539, -12632, -12724, -12817,
    -12909, -13002, -13094, -13186, -13278, -13370, -13462, -13553, -13645, -13736, -13827, -13918, -14009, -14100,
    -14191, -14281, -14372, -14462, -14552, -14642, -14732, -14822, -14911, -15001, -15090, -15179, -15268, -15357,
    -15446, -15534, -15623, -15711, -15799, -15887, -15975, -16063, -16150, -16238, -16325, -16412, -16499, -16586,
    -16672, -16759, -16845, -16931, -17017, -17103, -17189, -17274, -17360, -17445, -17530, -17615, -17699, -17784,
    -17868, -17952, -18036, -18120, -18204, -18287, -18371, -18454, -18537, -18620, -18702, -18785, -18867, -18949,
    -19031, -19113, -19194, -19276, -19357, -19438, -19519, -19599, -19680, -19760, -19840, -19920, -20000, -20079,
    -20159, -20238, -20317, -20396, -20474, -20553, -20631, -20709, -20787, -20864, -20942, -21019, -21096, -21173,
    -21249, -21326, -21402, -21478, -21554, -21629, -21705, -21780, -21855, -21930, -22004, -22079, -22153, -22227,
    -22301, -22374, -22448, -22521, -22594, -22666, -22739, -22811, -22883, -22955, -23027, -23098, -23169, -23240,
    -23311, -23382, -23452, -23522, -23592, -23661, -23731, -23800, -23869, -23938, -24006, -24075, -24143, -24211,
    -24278, -24346, -24413, -24480, -24546, -24613, -24679, -24745, -24811, -24877, -24942, -25007, -25072, -25136,
    -25201, -25265, -25329, -25392, -25456, -25519, -25582, -25645, -25707, -25769, -25831, -25893, -25954, -26016,
    -26077, -26137, -26198, -26258, -26318, -26378, -26437, -26497, -26556, -26615, -26673, -26731, -26789, -26847,
    -26905, -26962, -27019, -27076, -27132, -27188, -27244, -27300, -27355, -27411, -27466, -27520, -27575, -27629,
    -27683, -27736, -27790, -27843, -27896, -27948, -28001, -28053, -28105, -28156, -28208, -28259, -28309, -28360,
    -28410, -28460, -28510, -28559, -28608, -28657, -28706, -28754, -28802, -28850, -28897, -28945, -28992, -29038,
    -29085, -29131, -29177, -29222, -29268, -29313, -29358, -29402, -29446, -29490, -29534, -29577, -29621, -29663,
    -29706, -29748, -29790, -29832, -29873, -29915, -29955, -29996, -30036, -30076, -30116, -30156, -30195, -30234,
    -30272, -30311, -30349, -30386, -30424, -30461, -30498, -30535, -30571, -30607, -30643, -30678, -30713, -30748,
    -30783, -30817, -30851, -30885, -30918, -30951, -30984, -31017, -31049, -31081, -31113, -31144, -31175, -31206,
    -31236, -31267, -31297, -31326, -31356, -31385, -31413, -31442, -31470, -31498, -31525, -31553, -31580, -31606,
    -31633, -31659, -31684, -31710, -31735, -31760, -31785, -31809, -31833, -31856, -31880, -31903, -31926, -31948,
    -31970, -31992, -32014, -32035, -32056, -32077, -32097, -32117, -32137, -32156, -32176, -32194, -32213, -32231,
    -32249, -32267, -32284, -32301, -32318, -32334, -32350, -32366, -32382, -32397, -32412, -32426, -32441, -32455,
    -32468, -32482, -32495, -32508, -32520, -32532, -32544, -32556, -32567, -32578, -32588, -32599, -32609, -32618,
    -32628, -32637, -32646, -32654, -32662, -32670, -32678, -32685, -32692, -32699, -32705, -32711, -32717, -32722,
    -32727, -32732, -32736, -32740, -32744, -32748, -32751, -32754, -32757, -32759, -32761, -32763, -32764, -32765,
    -32766, -32766, -32767, -32766, -32766, -32765, -32764, -32763, -32761, -32759, -32757, -32754, -32751, -32748,
    -32744, -32740, -32736, -32732, -32727, -32722, -32717, -32711, -32705, -32699, -32692, -32685, -32678, -32670,
    -32662, -32654, -32646, -32637, -32628, -32618, -32609, -32599, -32588, -32578, -32567, -32556, -32544, -32532,
    -32520, -32508, -32495, -32482, -32468, -32455, -32441, -32426, -32412, -32397, -32382, -32366, -32350, -32334,
    -32318, -32301, -32284, -32267, -32249, -32231, -32213, -32194, -32176, -32156, -32137, -32117, -32097, -32077,
    -32056, -32035, -32014, -31992, -31970, -31948, -31926, -31903, -31880, -31856, -31833, -31809, -31785, -31760,
    -31735, -31710, -31684, -31659, -31633, -31606, -31580, -31553, -31525, -31498, -31470, -31442, -31413, -31385,
    -31356, -31326, -31297, -31267, -31236, -31206, -31175, -31144, -31113, -31081, -31049, -31017, -30984, -30951,
    -30918, -30885, -30851, -30817, -30783, -30748, -30713, -30678, -30643, -30607, -30571, -30535, -30498, -30461,
    -30424, -30386, -30349, -30311, -30272, -30234, -30195, -30156, -30116, -30076, -30036, -29996, -29955, -29915,
    -29873, -29832, -29790, -29748, -29706, -29663, -29621, -29577, -29534, -29490, -29446, -29402, -29358, -29313,
    -29268, -29222, -29177, -29131, -29085, -29038, -28992, -28945, -28897, -28850, -28802, -28754, -28706, -28657,
    -28608, -28559, -28510, -28460, -28410, -28360, -28309, -28259, -28208, -28156, -28105, -28053, -28001, -27948,
    -27896, -27843, -27790, -27736, -27683, -27629, -27575, -27520, -27466, -27411, -27355, -27300, -27244, -27188,
    -27132, -27076, -27019, -26962, -26905, -26847, -26789, -26731, -26673, -26615, -26556, -26497, -26437, -26378,
    -26318, -26258, -26198, -26137, -26077, -26016, -25954, -25893, -25831, -25769, -25707, -25645, -25582, -25519,
    -25456, -25392, -25329, -25265, -25201, -25136, -25072, -25007, -24942, -24877, -24811, -24745, -24679, -24613,
    -24546, -24480, -24413, -24346, -24278, -24211, -24143, -24075, -24006, -23938, -23869, -23800, -23731, -23661,
    -23592, -23522, -23452, -23382, -23311, -23240, -23169, -23098, -23027, -22955, -22883, -22811, -22739, -22666,
    -22594, -22521, -22448, -22374, -22301, -22227, -22153, -22079, -22004, -21930, -21855, -21780, -21705, -21629,
    -21554, -21478, -21402, -21326, -21249, -21173, -21096, -21019, -20942, -20864, -20787, -20709, -20631, -20553,
    -20474, -20396, -20317, -20238, -20159, -20079, -20000, -19920, -19840, -19760, -19680, -19599, -19519, -19438,
    -19357, -19276, -19194, -19113, -19031, -18949, -18867, -18785, -18702, -18620, -18537, -18454, -18371, -18287,
    -18204, -18120, -18036, -17952, -17868, -17784, -17699, -17615, -17530, -17445, -17360, -17274, -17189, -17103,
    -17017, -16931, -16845, -16759, -16672, -16586, -16499, -16412, -16325, -16238, -16150, -16063, -15975, -15887,
    -15799, -15711, -15623, -15534, -15446, -15357, -15268, -15179, -15090, -15001, -14911, -14822, -14732, -14642,
    -14552, -14462, -14372, -14281, -14191, -14100, -14009, -13918, -13827, -13736, -13645, -13553, -13462, -13370,
    -13278, -13186, -13094, -13002, -12909, -12817, -12724, -12632, -12539, -12446, -12353, -12260, -12166, -12073,
    -11980, -11886, -11792, -11698, -11604, -11510, -11416, -11322, -11227, -11133, -11038, -10944, -10849, -10754,
    -10659, -10564, -10469, -10373, -10278, -10182, -10087, -9991,  -9895,  -9799,  -9703,  -9607,  -9511,  -9415,
    -9319,  -9222,  -9126,  -9029,  -8932,  -8836,  -8739,  -8642,  -8545,  -8448,  -8351,  -8253,  -8156,  -8059,
    -7961,  -7864,  -7766,  -7668,  -7571,  -7473,  -7375,  -7277,  -7179,  -7081,  -6982,  -6884,  -6786,  -6688,
    -6589,  -6491,  -6392,  -6293,  -6195,  -6096,  -5997,  -5898,  -5799,  -5700,  -5601,  -5502,  -5403,  -5304,
    -5205,  -5106,  -5006,  -4907,  -4807,  -4708,  -4608,  -4509,  -4409,  -4310,  -4210,  -4110,  -4011,  -3911,
    -3811,  -3711,  -3611,  -3511,  -3411,  -3311,  -3211,  -3111,  -3011,  -2911,  -2811,  -2711,  -2610,  -2510,
    -2410,  -2310,  -2209,  -2109,  -2009,  -1908,  -1808,  -1708,  -1607,  -1507,  -1406,  -1306,  -1206,  -1105,
    -1005,  -904,   -804,   -703,   -603,   -502,   -402,   -301,   -201,   -100,   0,      100,    201,    301,
    402,    502,    603,    703,    804,    904,    1005,   1105,   1206,   1306,   1406,   1507,   1607,   1708,
    1808,   1908,   2009,   2109,   2209,   2310,   2410,   2510,   2610,   2711,   2811,   2911,   3011,   3111,
    3211,   3311,   3411,   3511,   3611,   3711,   3811,   3911,   4011,   4110,   4210,   4310,   4409,   4509,
    4608,   4708,   4807,   4907,   5006,   5106,   5205,   5304,   5403,   5502,   5601,   5700,   5799,   5898,
    5997,   6096,   6195,   6293,   6392,   6491,   6589,   6688,   6786,   6884,   6982,   7081,   7179,   7277,
    7375,   7473,   7571,   7668,   7766,   7864,   7961,   8059,   8156,   8253,   8351,   8448,   8545,   8642,
    8739,   8836,   8932,   9029,   9126,   9222,   9319,   9415,   9511,   9607,   9703,   9799,   9895,   9991,
    10087,  10182,  10278,  10373,  10469,  10564,  10659,  10754,  10849,  10944,  11038,  11133,  11227,  11322,
    11416,  11510,  11604,  11698,  11792,  11886,  11980,  12073,  12166,  12260,  12353,  12446,  12539,  12632,
    12724,  12817,  12909,  13002,  13094,  13186,  13278,  13370,  13462,  13553,  13645,  13736,  13827,  13918,
    14009,  14100,  14191,  14281,  14372,  14462,  14552,  14642,  14732,  14822,  14911,  15001,  15090,  15179,
    15268,  15357,  15446,  15534,  15623,  15711,  15799,  15887,  15975,  16063,  16150,  16238,  16325,  16412,
    16499,  16586,  16672,  16759,  16845,  16931,  17017,  17103,  17189,  17274,  17360,  17445,  17530,  17615,
    17699,  17784,  17868,  17952,  18036,  18120,  18204,  18287,  18371,  18454,  18537,  18620,  18702,  18785,
    18867,  18949,  19031,  19113,  19194,  19276,  19357,  19438,  19519,  19599,  19680,  19760,  19840,  19920,
    20000,  20079,  20159,  20238,  20317,  20396,  20474,  20553,  20631,  20709,  20787,  20864,  20942,  21019,
    21096,  21173,  21249,  21326,  21402,  21478,  21554,  21629,  21705,  21780,  21855,  21930,  22004,  22079,
    22153,  22227,  22301,  22374,  22448,  22521,  22594,  22666,  22739,  22811,  22883,  22955,  23027,  23098,
    23169,  23240,  23311,  23382,  23452,  23522,  23592,  23661,  23731,  23800,  23869,  23938,  24006,  24075,
    24143,  24211,  24278,  24346,  24413,  24480,  24546,  24613,  24679,  24745,  24811,  24877,  24942,  25007,
    25072,  25136,  25201,  25265,  25329,  25392,  25456,  25519,  25582,  25645,  25707,  25769,  25831,  25893,
    25954,  26016,  26077,  26137,  26198,  26258,  26318,  26378,  26437,  26497,  26556,  26615,  26673,  26731,
    26789,  26847,  26905,  26962,  27019,  27076,  27132,  27188,  27244,  27300,  27355,  27411,  27466,  27520,
    27575,  27629,  27683,  27736,  27790,  27843,  27896,  27948,  28001,  28053,  28105,  28156,  28208,  28259,
    28309,  28360,  28410,  28460,  28510,  28559,  28608,  28657,  28706,  28754,  28802,  28850,  28897,  28945,
    28992,  29038,  29085,  29131,  29177,  29222,  29268,  29313,  29358,  29402,  29446,  29490,  29534,  29577,
    29621,  29663,  29706,  29748,  29790,  29832,  29873,  29915,  29955,  29996,  30036,  30076,  30116,  30156,
    30195,  30234,  30272,  30311,  30349,  30386,  30424,  30461,  30498,  30535,  30571,  30607,  30643,  30678,
    30713,  30748,  30783,  30817,  30851,  30885,  30918,  30951,  30984,  31017,  31049,  31081,  31113,  31144,
    31175,  31206,  31236,  31267,  31297,  31326,  31356,  31385,  31413,  31442,  31470,  31498,  31525,  31553,
    31580,  31606,  31633,  31659,  31684,  31710,  31735,  31760,  31785,  31809,  31833,  31856,  31880,  31903,
    31926,  31948,  31970,  31992,  32014,  32035,  32056,  32077,  32097,  32117,  32137,  32156,  32176,  32194,
    32213,  32231,  32249,  32267,  32284,  32301,  32318,  32334,  32350,  32366,  32382,  32397,  32412,  32426,
    32441,  32455,  32468,  32482,  32495,  32508,  32520,  32532,  32544,  32556,  32567,  32578,  32588,  32599,
    32609,  32618,  32628,  32637,  32646,  32654,  32662,  32670,  32678,  32685,  32692,  32699,  32705,  32711,
    32717,  32722,  32727,  32732,  32736,  32740,  32744,  32748,  32751,  32754,  32757,  32759,  32761,  32763,
    32764,  32765,  32766,  32766};
/* data 11e8 */ LFOTracker *gActiveLFOs = NULL;
/* data 11ec */ SInt32 gNumLFOsInUse = 0;

SInt32 snd_LFO_TYPE_OFF(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SINE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SQUARE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_TRIANGLE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SAW(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_RAND(LFOTracker *tracker, int step);

/* data 11f0 */ LFOFunction gLFOFuncs[] = {
    snd_LFO_TYPE_OFF,      snd_LFO_TYPE_SINE, snd_LFO_TYPE_SQUARE,
    snd_LFO_TYPE_TRIANGLE, snd_LFO_TYPE_SAW,  snd_LFO_TYPE_RAND,
};

SInt32 snd_LFO_TYPE_OFF(LFOTracker *tracker, int step) { return 0; }

SInt32 snd_LFO_TYPE_SINE(LFOTracker *tracker, int step) { return gLFO_sine[step]; }

SInt32 snd_LFO_TYPE_SQUARE(LFOTracker *tracker, int step) { return (step < tracker->state_hold1) ? 32767 : -32767; }

SInt32 snd_LFO_TYPE_TRIANGLE(LFOTracker *tracker, int step) {
    SInt32 ret_val;

    ret_val = 0;

    if (step < 512) {
        ret_val = ((step * 0x7fff) / 512);
    } else if (step < 1536) {
        // feels fake
        ret_val = 0x7fff - ((step - 512) * 65534) / 1024;
    } else {
        ret_val = (((step - 1536) * 0x7fff) / 512) - 0x7fff;
    }

    return ret_val;
}

SInt32 snd_LFO_TYPE_SAW(LFOTracker *tracker, int step) {
    SInt32 ret_val;

    ret_val = 0;

    if (step < 1024) {
        ret_val = 0x7fff * step / 1023;
    } else {
        ret_val = ((0x7fff * (step - 1024)) / 1024) - 0x7fff;
    }

    return ret_val;
}

SInt32 snd_LFO_TYPE_RAND(LFOTracker *tracker, int step) {
    if (step >= 0x400 && tracker->state_hold2 == 1) {
        tracker->state_hold2 = 0;
        tracker->state_hold1 = 2 * ((snd_RandomUInt16() % 0x8000) - 0x3FFF);
    } else if (step < 0x400 && tracker->state_hold2 == 0) {
        tracker->state_hold2 = 1;
        tracker->state_hold1 = ((snd_RandomUInt16() % 0x8000) * ((snd_RandomUInt16() % 2) * -1));
    }

    return tracker->state_hold1;
}

SInt32 snd_GetLFO(register LFOTracker *tracker, register SInt32 step_mult) {
    SInt32 ret_val, step;

    ret_val = 0;
    step = tracker->next_step >> 16;

    tracker->next_step += step_mult * tracker->step_size;

    if (tracker->next_step > 0x07FFFFFF) {
        tracker->next_step -= 0x08000000;
    }

    ret_val = gLFOFuncs[tracker->type](tracker, step);
    if (tracker->setup_flags & 1) {
        ret_val *= -1;
    }

    return ret_val;
}

void snd_InitLFO(LFOTracker *tracker) {
    switch (tracker->type) {
    case 5:
        tracker->state_hold1 = (snd_RandomUInt16() % 0x8000) * ((snd_RandomUInt16() % 2) * -1);
        tracker->state_hold2 = 1;
        break;
    }

    snd_CalcLFODepth(tracker);
    snd_LockMasterTick(256);

    if (snd_InsertLFO(tracker)) {
        snd_DoOneLFO(tracker);
    }

    snd_UnlockMasterTick();
}

void snd_RemoveLFOsForHandler(BlockSoundHandlerPtr handler) {
    SInt32 i;

    for (i = 0; i < 4; i++) {
        snd_RemoveLFO(&handler->lfo[i]);
    }
}

void snd_RemoveLFO(LFOTracker *lfo) {
    LFOTracker *walk;

    lfo->running_flags &= ~1;

    if (gActiveLFOs == lfo) {
        gActiveLFOs = lfo->next;
        gNumLFOsInUse--;

        return;
    } else {
        walk = gActiveLFOs;
        while (walk && walk->next != lfo) {
            walk = walk->next;
        }

        if (walk) {
            walk->next = walk->next->next;
            gNumLFOsInUse--;
        }
    }
}

BOOL snd_InsertLFO(LFOTracker *lfo) {
    LFOTracker *walk;

    if ((lfo->running_flags & 1) != 0) {
        return 1;
    }

    if (gNumLFOsInUse == 10) {
        return 0;
    }

    gNumLFOsInUse++;

    lfo->next = NULL;
    lfo->running_flags |= 1;

    if (!gActiveLFOs) {
        gActiveLFOs = lfo;

        return 1;
    } else {
        walk = gActiveLFOs;
        while (walk->next) {
            walk = walk->next;
        }

        walk->next = lfo;
    }

    return 1;
}

UInt32 snd_DoOneLFO(LFOTracker *tracker) {
    BlockSoundHandlerPtr handler;
    LFOTracker *target_tracker;
    SInt32 range, lfo, new_val;
    UInt32 ret;

    ret = 0;
    handler = tracker->handler;

    switch (tracker->target) {
    case 1: // volume
        lfo = snd_GetLFO(tracker, 2) - 0x7fff;
        new_val = (tracker->range * lfo) >> 16;
        if (handler->LFO_Vol != new_val) {
            handler->LFO_Vol = new_val;
            ret = 1;
        }
        break;
    case 2: // pan
        lfo = snd_GetLFO(tracker, 2);
        new_val = (tracker->range * lfo) >> 15;
        if (handler->LFO_Pan != new_val) {
            handler->LFO_Pan = new_val;
            ret = 1;
        }
        break;
    case 3: // pitch mod
        lfo = snd_GetLFO(tracker, 2);
        new_val = (lfo * tracker->range) >> 15;
        if (new_val != handler->LFO_PM) {
            handler->LFO_PM = new_val;
            ret = 2;
        }
        break;
    case 4: // pitch bend
        lfo = snd_GetLFO(tracker, 2);
        new_val = (lfo * tracker->range) >> 15;
        if (new_val != handler->LFO_PB) {
            handler->LFO_PB = new_val;
            ret = 2;
        }
        break;
    case 5: // modulate other lfo frequency
        target_tracker = &handler->lfo[tracker->target_extra];
        lfo = snd_GetLFO(tracker, 2);
        range = ((target_tracker->orig_step_size >> 8) * tracker->depth) >> 10;
        target_tracker->step_size = target_tracker->orig_step_size + (((range * lfo) >> 15) << 8);
        break;
    case 6: // modulate lfo depth
        target_tracker = &handler->lfo[tracker->target_extra];
        lfo = snd_GetLFO(tracker, 2);
        range = (target_tracker->orig_depth * tracker->depth) >> 10;
        target_tracker->depth = target_tracker->orig_depth + ((range * lfo) >> 15);
        snd_CalcLFODepth(target_tracker);
        break;
    }

    return ret;
}

void snd_HandleLFOs() {
    LFOTracker *tracker;
    UInt32 flags;

    if (snd_GetTick() & 1) {
        return;
    }

    for (tracker = gActiveLFOs; tracker; tracker = tracker->next) {
        flags = snd_DoOneLFO(tracker);
        if (flags & 1)
            snd_SetSFXVolPan(tracker->handler->SH.OwnerID, 0x7FFFFFFF, -2, 1);
        if (flags & 2)
            snd_UpdateSFXPitch(tracker->handler);
    }
}

void snd_CalcLFODepth(LFOTracker *lfo) {
    SFX2 *sfx;

    switch (lfo->target) {
    case 1:
        sfx = (SFX2 *)lfo->handler->SH.Sound;
        lfo->range = (sfx->Vol * lfo->depth) >> 10;
        break;
    case 2:
        lfo->range = (180 * lfo->depth) >> 10;
        break;
    case 3:
        lfo->range = (6096 * lfo->depth) >> 10;
        break;
    case 4:
        lfo->range = (0x7fff * lfo->depth) >> 10;
        break;
    }

    lfo->last_lfo = 0;
}