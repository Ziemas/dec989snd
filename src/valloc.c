#include "common.h"
#include "globals.h"
#include "functions.h"

/* data 1dfc */ SInt32 gLockBlockerSema = -1;
/* data 1e00 */ SInt32 gTickAttentionNeeded = 0;
/* data 1e04 */ SInt32 gVoiceAllocationBlocked = 0;
/* data 1e08 */ SInt32 gVoiceAllocatorInUse = 0;
/* data 1e0c */ UInt32 gVAllocOwnerID = 0;
/* data 1e10 */ SInt32 gChannelMode = 0;
/* data 1e14 */ VoiceRange gVoiceRanges[16] = {
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
    {0, 47},
};
/* data 1e54 */ VoiceAttributes *gPlayingListHead = NULL;
/* data 1e58 */ SInt32 gNoiseOwnerPriority[2] = {-1, -1};
/* data 1e60 */ SInt32 gNoiseOwnerVoice[2] = {-1, -1};
/* data 1e68 */ SInt32 gThreadWaitingForVoiceAllocator = -1;
/* bss 92d0 */ VoiceAttributes gChannelStatus[48];
/* bss 20 */ UInt32 gAwaitingKeyOn[2];
/* bss 28 */ UInt32 gAwaitingKeyOff[2];
/* bss 30 */ UInt32 gKeyedOnVoices[2];
/* bss 38 */ UInt32 gKeyedOffVoices[2];
/* bss 18 */ UInt32 gReverbVoices[2];
/* bss 10 */ UInt32 gExternVoices[2];

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
