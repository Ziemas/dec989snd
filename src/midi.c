#include "common.h"

INCLUDE_ASM("asm/nonmatchings/midi", snd_PlayMIDISound);

INCLUDE_ASM("asm/nonmatchings/midi", snd_SetupMIDIStreamHandler);

INCLUDE_ASM("asm/nonmatchings/midi", snd_GetRealMIDIDataStream);

INCLUDE_ASM("asm/nonmatchings/midi", snd_MuteMIDIChannel);

INCLUDE_ASM("asm/nonmatchings/midi", snd_UnmuteMIDIChannel);

INCLUDE_ASM("asm/nonmatchings/midi", snd_ProcessMIDITick);

INCLUDE_ASM("asm/nonmatchings/midi", ReadVarLen);

INCLUDE_ASM("asm/nonmatchings/midi", snd_MIDINoteOn);

INCLUDE_ASM("asm/nonmatchings/midi", snd_MIDINoteOff);

INCLUDE_ASM("asm/nonmatchings/midi", snd_ReleaseDamper);

INCLUDE_ASM("asm/nonmatchings/midi", snd_ResetControllers);

INCLUDE_ASM("asm/nonmatchings/midi", snd_PitchBend);

INCLUDE_ASM("asm/nonmatchings/midi", snd_SetMIDISoundVolumePan);

INCLUDE_ASM("asm/nonmatchings/midi", snd_SetMIDIHandlerVolumePan);

INCLUDE_ASM("asm/nonmatchings/midi", snd_SetMIDISoundPitchModifier);

INCLUDE_ASM("asm/nonmatchings/midi", snd_SetMIDIHandlerPitchModifier);

INCLUDE_ASM("asm/nonmatchings/midi", snd_MIDISoundOwnerProc);
