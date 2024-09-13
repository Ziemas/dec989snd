#include "common.h"

INCLUDE_ASM("asm/nonmatchings/valloc", snd_InitVoiceAllocator);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_CleanupVoiceAllocator);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_SetMixerMode);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_SetGroupVoiceRange);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_LockVoiceAllocatorEx);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_IsVoiceAllocatorLocked);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_CheckAllocatorCueTick);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_UnlockVoiceAllocator);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_ExternVoiceAlloc);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_ExternVoiceFree);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_SetVoiceNoiseOrWave);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_AllocateNoiseVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_AllocateVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartVAGVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartPendingVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StopPendingVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetKeyedOnVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetKeyedOffVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_VoiceIsDone);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceAge);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceStatus);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoicePriority);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoicesEx);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_SilenceVoicesEx);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PauseVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PauseVoicesOwnedWithOwner);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_UnPauseVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_UnPauseVoicesOwnedWithOwner);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceStatusPtr);

INCLUDE_ASM("asm/nonmatchings/valloc", PS1Note2Pitch);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PitchBendTone);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_MarkVoicePlaying);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_MarkVoiceFree);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_HardFreeVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOnVoiceRaw);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoiceRaw);
