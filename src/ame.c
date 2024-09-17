#include "common.h"
#include "functions.h"
#include "globals.h"

UInt32 snd_PlayAMESound(MIDISoundPtr sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend) {
    AMEHandlerPtr stream_handler;
    MultiMIDIBlockHeaderPtr ame_master;

    ame_master = sound->MIDIBlock;
    if (!ame_master) {
        snd_ShowError(89, 0, 0, 0, 0);
        return 0;
    }

    if (ame_master->Flags & 2) {
        return 0;
    }

    stream_handler = snd_GetFreeAMEHandler();
    if (!stream_handler) {
        return 0;
    }

    ame_master->Flags |= 2;
    if (vol == 0x7fffffff) {
        vol = 1024;
    }
    stream_handler->SH.Current_Vol = (sound->Vol * vol) >> 10;
    if (stream_handler->SH.Current_Vol >= 128) {
        stream_handler->SH.Current_Vol = 127;
    }

    if (pan == -1 || pan == -2) {
        stream_handler->SH.Current_Pan = sound->Pan;
    } else {
        stream_handler->SH.Current_Pan = pan;
    }
    stream_handler->SH.Current_PM = pitch_mod;
    stream_handler->SH.Sound = sound;
    stream_handler->SH.Effects = NULL;
    stream_handler->SH.VolGroup = sound->VolGroup;

    snd_StartAMESegment(stream_handler, 0);
    snd_ActivateHandler(stream_handler);

    return stream_handler->SH.OwnerID;
}

INCLUDE_ASM("asm/nonmatchings/ame", snd_StartAMESegment);
//SInt32 snd_StartAMESegment(AMEHandlerPtr parent, SInt32 segment) {
//	GSoundHandlerPtr walk;
//	AMEStreamHeaderPtr stream_ptr;
//	MultiMIDIBlockHeaderPtr ame_master;
//	MIDISoundPtr m_sound;
//}

INCLUDE_ASM("asm/nonmatchings/ame", snd_StopAMESegment);
//void snd_StopAMESegment(MIDIHandlerPtr handler) {}

INCLUDE_ASM("asm/nonmatchings/ame", snd_FindAMEStream);
//AMEStreamHeaderPtr snd_FindAMEStream(MIDIHandlerPtr stream1, SInt32 index) {
//	MultiMIDIBlockHeaderPtr ame_master;
//}

INCLUDE_ASM("asm/nonmatchings/ame", snd_SetAMESoundVolumePan);
//void snd_SetAMESoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan) {
//	AMEHandlerPtr stream;
//	MIDIHandlerPtr walk;
//}

INCLUDE_ASM("asm/nonmatchings/ame", snd_SetAMESoundPitchModifier);
//void snd_SetAMESoundPitchModifier(UInt32 handle, SInt16 mod) {
//	AMEHandlerPtr stream;
//	MIDIHandlerPtr walk;
//}

INCLUDE_ASM("asm/nonmatchings/ame", snd_AMEFunction);
//SInt8* snd_AMEFunction(MIDIHandlerPtr stream, UInt8 *ame_header) {
//	SInt32 compare_value;
//	UInt8 *command_ptr;
//	SInt16 next_command;
//	SInt32 ignore_next_command;
//	AMEStreamHeaderPtr stream_to_stop;
//	SInt32 y;
//	SInt32 group;
//	SInt32 counter;
//	SInt32 done;
//	SInt32 stop;
//	SInt8 *stream_end;
//	AMEHandlerPtr ame_handler;
//}

void snd_SetMIDIRegister(UInt32 handle, SInt32 reg, SInt16 value) {
	AMEHandlerPtr ame;
    if (reg < 0 || reg >= 16) {
        snd_ShowError(92, reg, value, 0, 0);
        return;
    }

    snd_LockMasterTick(10);

    if (((handle >> 24) & 0x1f) != 2 || !(ame = snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    if (value >= 128) {
        value = 127;
    }

    if (value < 0) {
        value = 0;
    }

    ame->MIDIRegister[reg] = value;

    snd_UnlockMasterTick();
}

void snd_SetAllMIDIRegister(UInt32 handle, SInt8 *vals) {
	AMEHandlerPtr ame;
	SInt32 i;

    snd_LockMasterTick(11);

    if (((handle >> 24) & 0x1f) != 2 || !(ame = snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    for (i = 0; i < 16; i++) {
        ame->MIDIRegister[i] = vals[i]; 
    }
    
    snd_UnlockMasterTick();
}

SInt32 snd_GetMIDIRegister(UInt32 handle, SInt32 reg) {
	AMEHandlerPtr ame;

    if (((handle >> 24) & 0x1f) != 2) {
        return 0;
    }

    if (!(ame = snd_CheckHandlerStillActive(handle))) {
        return 0;
    }

    return ame->MIDIRegister[reg];
}