#ifndef VALLOC_H_
#define VALLOC_H_

#include "common.h"

extern SInt32 gTickAttentionNeeded;
extern UInt32 gVAllocOwnerID;
extern SInt32 gVoiceAllocatorInUse;
extern VoiceAttributes *gPlayingListHead;
extern VoiceAttributes gChannelStatus[48];
extern UInt32 gAwaitingKeyOn[2];
extern UInt32 gAwaitingKeyOff[2];
extern UInt32 gKeyedOnVoices[2];
extern UInt32 gKeyedOffVoices[2];
extern UInt32 gReverbVoices[2];

void snd_InitVoiceAllocator();
void snd_CleanupVoiceAllocator();
void snd_SetMixerMode(SInt32 channel_mode, SInt32 reverb_mode);
void snd_SetGroupVoiceRange(SInt32 group, SInt32 min, SInt32 max);
SInt32 snd_LockVoiceAllocatorEx(BOOL block, UInt32 ownerID);
SInt32 snd_IsVoiceAllocatorLocked();
SInt32 snd_CheckAllocatorCueTick();
void snd_UnlockVoiceAllocator();
SInt32 snd_ExternVoiceAlloc(SInt32 vol_group, SInt32 priority);
void snd_ExternVoiceFree(SInt32 voice);
void snd_SetVoiceNoiseOrWave(SInt32 voice, BOOL wave);
SInt32 snd_AllocateNoiseVoice(SInt32 vol_group, SInt32 priority);
SInt32 snd_AllocateVoice(SInt32 vol_group, SInt32 priority);
void snd_StartVAGVoice(SInt32 voice, BOOL noise);
void snd_StartVoices(VoiceFlags *voices);
void snd_StartPendingVoices();
void snd_StopPendingVoices();
void snd_GetKeyedOnVoices(VoiceFlags *voices);
void snd_GetKeyedOffVoices(VoiceFlags *voices);
void snd_VoiceIsDone(SInt32 voice);
UInt32 snd_GetVoiceAge(SInt32 voice);
SInt32 snd_GetVoiceStatus(SInt32 voice);
SInt32 snd_GetVoicePriority(SInt32 voice);
void snd_KeyOffVoice(SInt32 voice);
void snd_KeyOffVoicesEx(VoiceFlags *voices, BOOL do_owner_proc);
void snd_SilenceVoicesEx(VoiceFlags *voices, BOOL do_owner_proc);
void snd_PauseVoices(VoiceFlags *voices);
void snd_PauseVoicesOwnedWithOwner(GSoundHandlerPtr owner);
void snd_UnPauseVoices(VoiceFlags *voices);
void snd_UnPauseVoicesOwnedWithOwner(GSoundHandlerPtr owner);
VoiceAttributes *snd_GetVoiceStatusPtr(SInt32 voice);
UInt16 PS1Note2Pitch(SInt8 center_note, SInt8 center_fine, UInt16 note, SInt16 fine);
void snd_PitchBendTone(TonePtr tone, SInt32 pb, SInt32 pm, SInt32 note, SInt32 fine, SInt32 *new_note,
                       SInt32 *new_fine);
void snd_MarkVoicePlaying(SInt32 voice);
void snd_MarkVoiceFree(SInt32 voice);
void snd_HardFreeVoice(SInt32 voice);
void snd_KeyOnVoiceRaw(int core, int voice, BOOL reverb);
void snd_KeyOffVoiceRaw(int core, int voice);

#endif // VALLOC_H_
