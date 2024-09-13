#include "common.h"

INCLUDE_ASM("asm/nonmatchings/init", snd_SystemRunning);

INCLUDE_ASM("asm/nonmatchings/init", snd_StartSoundSystemEx);

INCLUDE_ASM("asm/nonmatchings/init", snd_InstallSoundTick);

INCLUDE_ASM("asm/nonmatchings/init", snd_RemoveSoundTick);

INCLUDE_ASM("asm/nonmatchings/init", snd_StopSoundSystem);

INCLUDE_ASM("asm/nonmatchings/init", snd_GetTickRate);

INCLUDE_ASM("asm/nonmatchings/init", snd_TimerLLIRQLoop);
