#ifndef AME_H_
#define AME_H_

#include "common.h"

UInt32 snd_PlayAMESound(MIDISoundPtr sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
SInt32 snd_StartAMESegment(AMEHandlerPtr parent, SInt32 segment);
void snd_StopAMESegment(MIDIHandlerPtr handler);
AMEStreamHeaderPtr snd_FindAMEStream(MIDIHandlerPtr stream1, SInt32 index);
void snd_SetAMESoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan);
void snd_SetAMESoundPitchModifier(UInt32 handle, SInt16 mod);
SInt8 *snd_AMEFunction(MIDIHandlerPtr stream, UInt8 *ame_header);
void snd_SetMIDIRegister(UInt32 handle, SInt32 reg, SInt16 value);
void snd_SetAllMIDIRegister(UInt32 handle, SInt8 *vals);
SInt32 snd_GetMIDIRegister(UInt32 handle, SInt32 reg);

#endif // AME_H_
