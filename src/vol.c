#include "common.h"

INCLUDE_ASM("asm/nonmatchings/vol", snd_InitDuckers);

INCLUDE_ASM("asm/nonmatchings/vol", snd_SetMasterVolume);

INCLUDE_ASM("asm/nonmatchings/vol", snd_GetMasterVolume);

INCLUDE_ASM("asm/nonmatchings/vol", snd_SetPlaybackMode);

INCLUDE_ASM("asm/nonmatchings/vol", snd_GetPlaybackMode);

INCLUDE_ASM("asm/nonmatchings/vol", snd_MakeVolumes);

INCLUDE_ASM("asm/nonmatchings/vol", snd_MakeVolumesB);

INCLUDE_ASM("asm/nonmatchings/vol", snd_CalcStereoBalance3d);

INCLUDE_ASM("asm/nonmatchings/vol", snd_AdjustVolToGroup);

INCLUDE_ASM("asm/nonmatchings/vol", snd_AdjustAllChannelsMasterVolume);

INCLUDE_ASM("asm/nonmatchings/vol", snd_MIDITo360Pan);

INCLUDE_ASM("asm/nonmatchings/vol", snd_CheckDuckers);

INCLUDE_ASM("asm/nonmatchings/vol", snd_SetMasterVolumeDucker);
