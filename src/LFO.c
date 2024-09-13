#include "common.h"

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_OFF);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_SINE);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_SQUARE);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_TRIANGLE);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_SAW);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_LFO_TYPE_RAND);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_GetLFO);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_InitLFO);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_RemoveLFOsForHandler);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_RemoveLFO);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_InsertLFO);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_DoOneLFO);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_HandleLFOs);

INCLUDE_ASM("asm/nonmatchings/LFO", snd_CalcLFODepth);
