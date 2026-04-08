#ifndef PLAYSND_H_
#define PLAYSND_H_

#include "common.h"

SInt32 snd_GetSoundUserData(SoundBankPtr bank, char *bank_name, int sound_index, char *sound_name, int *destination);
UInt32 snd_PlaySoundByNameVolPanPMPB(SoundBankPtr block, char *block_name, char *sound, SInt32 vol, SInt32 pan,
                                     SInt16 pitch_mod, SInt16 bend);
UInt32 snd_PlaySoundEx(SndPlayParamsPtr params);
UInt32 snd_PlaySoundVolPanPMPB(SoundBankPtr bank, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
void snd_StopSound(UInt32 handle);
UInt32 snd_SoundIsStillPlaying(UInt32 handle);
SInt32 snd_IsSoundALooper(SoundBankPtr bank, SInt32 sound);
void snd_SetSoundVolPan(UInt32 handle, SInt32 vol, SInt32 pan);
UInt32 snd_SetSoundParams(UInt32 handle, UInt32 mask, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
SInt32 snd_GetSoundOriginalPitch(SoundBankPtr bank, SInt32 sound);
SInt32 snd_GetSoundCurrentPitch(UInt32 handle);
SInt32 snd_GetSoundPitchBend(UInt32 handle);
void snd_SetSoundPitch(UInt32 handle, SInt32 pitch);
void snd_SetSoundPitchBend(UInt32 handle, SInt16 bend);
void snd_SetSoundPitchModifier(UInt32 handle, SInt16 mod);
SInt8 snd_GetSoundReg(UInt32 handle, SInt32 which);
void snd_SetSoundReg(UInt32 handle, SInt32 which, SInt8 val);
void snd_SetAllSoundReg(UInt32 handle, SInt8 *vals);
SInt32 snd_GetSoundPitchModifier(UInt32 handle);
void snd_PauseSound(UInt32 handle);
void snd_ContinueSound(UInt32 handle);
void snd_StopAllSoundsInBank(SoundBankPtr bank, SInt32 silence);
void snd_SetGlobalExcite(SInt32 value);
SInt32 snd_GetGlobalExcite();
SoundBankPtr snd_FindBankByName(char *name);
SInt32 snd_FindSoundByName(SFXBlock2Ptr block, char *name, SFXBlock2Ptr *found_block);
SInt32 snd_CalcSoundNameHash(char *name);
SInt32 snd_CollectTones(SoundBankPtr bank, SInt32 prog, SInt32 key, TonePtr *tones);
void snd_DEBUG_SoloSound(SoundBankPtr bank, SInt32 sound);
SInt32 snd_DEBUG_CheckSolo(SoundBankPtr bank, SInt32 sound);
void snd_DEBUG_ClearSolo();

#endif // PLAYSND_H_
