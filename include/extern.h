#ifndef EXTERN_H_
#define EXTERN_H_

#include "common.h"

SInt32 InitResidentModule();
UInt32 snd_PlaySoundByNameVolPanPMPB_E(SoundBankPtr block, char *block_name, char *sound, SInt32 vol, SInt32 pan,
                                       SInt16 pitch_mod, SInt16 bend);
SInt32 snd_GetSoundUserData_E(SoundBankPtr bank, char *bank_name, int sound_index, char *sound_name, int *destination);
void snd_SetMIDIRegister_E(UInt32 handle, SInt32 reg, SInt16 value);
SInt32 snd_GetMIDIRegister_E(UInt32 handle, SInt32 reg);
void snd_SetGlobalExcite_E(SInt32 value);
SInt32 snd_GetGlobalExcite_E();
UInt32 snd_SRAMMalloc_E(UInt32 size);
UInt32 snd_SRAMMarkUsed_E(UInt32 loc, UInt32 size);
void snd_SRAMFree_E(UInt32 loc, UInt32 size);
SInt32 snd_GetDopplerPitchMod_E(SInt32 approaching_mph);
void snd_MIDINoteOn_E(MIDIHandlerPtr stream);
void snd_MIDINoteOff_E(MIDIHandlerPtr stream);
void snd_PitchBend_E(MIDIHandlerPtr stream);
SInt16 snd_MIDITo360Pan_E(SInt8 pan);
void snd_ReleaseDamper_E(MIDIHandlerPtr stream);
void snd_ResetControllers_E(MIDIHandlerPtr stream);
SInt32 snd_SRAMGetFreeMem_E();
SInt32 snd_SRAMMaxFreeBlockSize_E();

#endif // EXTERN_H_
