#include "common.h"
#include "functions.h"

/* data 13d8 */ static SInt16 r250_index = 0;
/* data 13dc */ static UInt16 r250_buffer[250] = {
    15301,  -7772,  10921,  -9191,  19316,  -22382, -10809, -16284, 32360,
    -15954, 26124,  25833,  -31132, 11030,  26232,  13965,  16051,  -1901,
    -9676,  5184,   15931,  -25754, 16845,  11371,  -26912, 10328,  9139,
    1684,   -16868, -6148,  13297,  1364,   -9508,  15687,  -2257,  27771,
    5277,   -20908, 31973,  -18559, 16327,  23408,  -29471, -13264, -31926,
    -3987,  -7172,  3472,   21367,  -9179,  -9191,  -11501, 7712,   -9652,
    -25762, 10241,  -15372, -17541, 1718,   -18649, -17644, 6010,   29575,
    -10564, 30458,  21966,  -11087, 10387,  4492,   644,    -8505,  -23929,
    -3716,  -10948, -24687, -11484, -5661,  -22408, -15166, -20845, 286,
    12071,  3574,   -4152,  15592,  -19859, 9711,   23022,  -30280, -20043,
    -16623, 146,    9053,   5881,   -28901, -22256, -12072, 8529,   -31192,
    -581,   -27270, 12730,  101,    16208,  12607,  -6615,  22036,  8221,
    31337,  11984,  20290,  26734,  19552,  48,     31940,  -22088, -30774,
    -12192, -4872,  12809,  -8218,  17436,  -20806, 19375,  30,     17425,
    14117,  5416,   23853,  -9753,  -7541,  32074,  26526,  2192,   11447,
    11,     -12090, -30384, -926,   -653,   26899,  25357,  7667,   3577,
    -26122, -14375, 4,      -7109,  -8194,  -6979,  -12303, 1066,   29237,
    -28728, 19370,  17493,  -27968, 3,      -4068,  -26660, 17586,  -599,
    21716,  -9064,  -7376,  -20581, -10315, -1656,  1,      32200,  -3470,
    22911,  24090,  10438,  -24753, -29172, 14999,  2489,   -22252, 9898,
    -25924, 9245,   593,    -30679, -24482, 30162,  -39,    -12196, 27209,
    -20119, -28039, 4612,   -7139,  -12626, -9223,  -2820,  22377,  -25226,
    15190,  -31065, -1531,  18090,  11326,  -14697, -2635,  -6252,  5580,
    15231,  9467,   13161,  -7036,  7259,   317,    -14568, 2962,   23006,
    32280,  6994,   18751,  5148,   -12797, -16166, -13644, 18552,  -13272,
    -11505, 2804,   17360,  1919,   19639,  2323,   9448,   -21715, 11022,
    -20036, 31509,  -16356, -29938, -26653, 19754,  987,    11521,  -10042,
    -27480, 20664,  2629,   -14550, 31009,  -11493, -5793};
/* data 15d0 */ static UInt32 seed = 1;

static UInt16 myrand();
static void mysrand(UInt16 newseed);

void snd_RandInit(SInt16 seed) {
    SInt16 j;
    SInt16 k;
    UInt16 mask;
    UInt16 msb;

    mysrand(seed);
    r250_index = 0;
    for (j = 0; j < 250; j++) {
        r250_buffer[j] = myrand();
    }

    for (j = 0; j < 250; ++j) {
        if (myrand() >= 0x4001u) {
            r250_buffer[j] |= 0x8000u;
        }
    }

    msb = 0x8000;
    mask = -1;
    for (j = 0; j < 16; ++j) {
        k = 11 * j + 3;
        r250_buffer[k] &= mask;
        r250_buffer[k] |= msb;
        mask >>= 1;
        msb >>= 1;
    }
}

UInt16 snd_RandomUInt16() {
    register SInt32 j;
    register UInt32 new_rand;

    if (r250_index >= 147) {
        j = r250_index - 147;
    } else {
        j = r250_index + 103;
    }

    new_rand = r250_buffer[r250_index] = r250_buffer[r250_index] ^ r250_buffer[j];

    if (r250_index >= 249) {
        r250_index = 0;
    } else {
        r250_index++;
    }

    return new_rand;
}

static UInt16 myrand() {
    seed = 0x15A4E35 * seed + 1;
    return (seed) >> 16 & 0x7FFF;
}

static void mysrand(UInt16 newseed) {
    seed = newseed;
}
