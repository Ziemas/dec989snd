#ifndef VOL_H_
#define VOL_H_

#include "common.h"

void snd_InitDuckers();
void snd_SetMasterVolume(SInt32 which, SInt32 vol);
SInt32 snd_GetMasterVolume(SInt32 which);
void snd_SetPlaybackMode(SInt32 mode);
SInt32 snd_GetPlaybackMode();
void snd_MakeVolumes(SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan, SInt32 tone_vol, SInt32 tone_pan,
                     SpuVolume *spu_vol);
void snd_MakeVolumesB(SInt32 bank_vol, SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan,
                      SInt32 tone_vol, SInt32 tone_pan, SpuVolume *spu_vol);
void snd_CalcStereoBalance3d(SInt32 snd_vol, SInt32 snd_pan, SpuVolume *spu_vol);
SInt16 snd_AdjustVolToGroup(SInt16 vol, SInt32 group);
void snd_AdjustAllChannelsMasterVolume(UInt32 flags);
SInt16 snd_MIDITo360Pan(SInt8 pan);
void snd_CheckDuckers();
void snd_SetMasterVolumeDucker(SInt32 which, DuckerDefPtr state);

#endif // VOL_H_
