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
    stream_handler->SH.Sound = (SoundPtr)sound;
    stream_handler->SH.Effects = NULL;
    stream_handler->SH.VolGroup = sound->VolGroup;

    snd_StartAMESegment(stream_handler, 0);
    snd_ActivateHandler(&stream_handler->SH);

    return stream_handler->SH.OwnerID;
}

SInt32 snd_StartAMESegment(AMEHandlerPtr parent, SInt32 segment) {
    GSoundHandlerPtr walk;
    AMEStreamHeaderPtr stream_ptr;
    MultiMIDIBlockHeaderPtr ame_master;
    MIDISoundPtr m_sound;

    m_sound = (MIDISoundPtr)parent->SH.Sound;
    ame_master = (MultiMIDIBlockHeaderPtr)m_sound->MIDIBlock;
    stream_ptr = (AMEStreamHeaderPtr)ame_master->BlockPtr[segment];

    if ((stream_ptr->MH.SH.OwnerID >> 31) & 1) {
        return 0;
    }

    if (!stream_ptr->MB.BankPtr) {
        return 0;
    }

    stream_ptr->MH.SH.first_child = NULL;
    stream_ptr->MH.SH.siblings = NULL;

    walk = &parent->SH;
    if (!walk->first_child) {
        walk->first_child = &stream_ptr->MH.SH;
    } else {
        walk = walk->first_child;
        while (walk->siblings) {
            walk = walk->siblings;
        }

        walk->siblings = &stream_ptr->MH.SH;
    }

    stream_ptr->MH.SH.parent = &parent->SH;
    stream_ptr->MH.SH.Current_Vol = parent->SH.Current_Vol;
    stream_ptr->MH.SH.Current_Pan = parent->SH.Current_Pan;
    stream_ptr->MH.SH.Current_PM = parent->SH.Current_PM;
    stream_ptr->MH.SH.flags = parent->SH.flags;
    stream_ptr->MH.SH.VolGroup = parent->SH.VolGroup;
    stream_ptr->MH.SH.Sound = parent->SH.Sound;
    stream_ptr->MH.SH.Voices.core[0] = stream_ptr->MH.SH.Voices.core[1] = 0;
    stream_ptr->MH.Repeats = 0;

    snd_SetupMIDIStreamHandler(&stream_ptr->MH, &stream_ptr->MB);
    snd_ActivateHandler(&stream_ptr->MH.SH);

    return 1;
}

void snd_StopAMESegment(MIDIHandlerPtr handler) {
    if (!((handler->SH.OwnerID >> 31) & 1)) {
        return;
    }

    snd_StopHandlerPtr(&handler->SH, 0, 0, 0);
    handler->Flags &= ~1;
}

AMEStreamHeaderPtr snd_FindAMEStream(MIDIHandlerPtr stream1, SInt32 index) {
    MultiMIDIBlockHeaderPtr ame_master;

    ame_master = ((MIDISoundPtr)(stream1->SH.Sound))->MIDIBlock;

    return (AMEStreamHeaderPtr)ame_master->BlockPtr[index];
}

void snd_SetAMESoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan) {
    AMEHandlerPtr stream;
    MIDIHandlerPtr walk;

    snd_LockVoiceAllocatorEx(1, 10);
    if (!(stream = (AMEHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockVoiceAllocator();
        return;
    }

    if (vol < 0) {
        stream->SH.Current_Vol = -1 * vol;
    } else if (vol != 0x7fffffff) {
        stream->SH.Current_Vol = (stream->SH.Sound->Vol * vol) >> 10;
    }

    if (stream->SH.Current_Vol >= 128) {
        stream->SH.Current_Vol = 127;
    }

    if (pan == -1) {
        stream->SH.Current_Pan = stream->SH.Sound->Pan;
    } else if (pan != -2) {
        stream->SH.Current_Pan = pan;
    }

    snd_UnlockVoiceAllocator();

    walk = (MIDIHandlerPtr)stream->SH.first_child;
    while (walk) {
        snd_SetMIDIHandlerVolumePan(walk, vol, pan);
        walk = (MIDIHandlerPtr)walk->SH.siblings;
    }
}

void snd_SetAMESoundPitchModifier(UInt32 handle, SInt16 mod) {
    AMEHandlerPtr stream;
    MIDIHandlerPtr walk;

    snd_LockVoiceAllocatorEx(1, 11);

    if (!(stream = (AMEHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockVoiceAllocator();
        return;
    }

    stream->SH.Current_PM = mod;
    snd_UnlockVoiceAllocator();

    walk = (MIDIHandlerPtr)stream->SH.first_child;
    while (walk) {
        snd_SetMIDIHandlerPitchModifier(walk, mod);
        walk = (MIDIHandlerPtr)walk->SH.siblings;
    }
}

SInt8 *snd_AMEFunction(MIDIHandlerPtr stream, UInt8 *ame_header) {
    SInt32 compare_value;
    UInt8 *command_ptr;
    SInt16 next_command;
    SInt32 ignore_next_command;
    AMEStreamHeaderPtr stream_to_stop;
    SInt32 y;
    SInt32 group;
    SInt32 counter;
    SInt32 done;
    SInt32 stop;
    SInt8 *stream_end;
    AMEHandlerPtr ame_handler;

    if (!stream) {
        return (SInt8 *)1;
    }

    if (!stream->SH.parent) {
        ame_handler = (AMEHandlerPtr)stream;
    } else {
        ame_handler = (AMEHandlerPtr)stream->SH.parent;
    }

    command_ptr = ame_header;
    ignore_next_command = 0;
    done = 0;
    stop = 0;

    while (!done) {
        next_command = *command_ptr;
        command_ptr++;
        switch (next_command) {
        case 0:
            if (!ignore_next_command) {
                if (gGlobalExcite < *command_ptr + 1) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 1:
            if (!ignore_next_command) {
                if (gGlobalExcite != *command_ptr + 1) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 2:
            if (!ignore_next_command) {
                if (gGlobalExcite > *command_ptr - 1) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 3:
            if (!ignore_next_command) {
                stream_to_stop = snd_FindAMEStream(stream, *command_ptr);
                if (&stream_to_stop->MH == stream) {
                    stop = 1;
                    done = 1;
                } else {
                    snd_StopAMESegment(&stream_to_stop->MH);
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 4:
            if (ignore_next_command == 1) {
                ignore_next_command = 2;
            }
            break;
        case 5:
            if (ignore_next_command == 2) {
                ignore_next_command = 0;
            }
            break;
        case 6:
            if (!ignore_next_command) {
                if (ame_handler->MIDIRegister[command_ptr[0]] > command_ptr[1] - 1) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr += 2;
            break;
        case 7:
            if (!ignore_next_command) {
                if (ame_handler->MIDIRegister[command_ptr[0]] < command_ptr[1] + 1) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr += 2;
            break;
        case 11:
            ame_handler->MIDIMacro[command_ptr[0]] = &command_ptr[1];
            command_ptr++;
            while (*command_ptr != 0xf7) {
                command_ptr++;
            }
            command_ptr++;
            break;
        case 12:
            if (!ignore_next_command) {
                stream_end = snd_AMEFunction(stream, ame_handler->MIDIMacro[*command_ptr]);
                if (!stream_end) { // ?
                    stop = 1;
                    done = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 13:
            if (!ignore_next_command) {
                stop = 1;
                done = 1;
                snd_StartAMESegment(ame_handler, ame_handler->MIDIRegister[*command_ptr] - 1);
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 14:
            if (!ignore_next_command) {
                snd_StartAMESegment(ame_handler, ame_handler->MIDIRegister[*command_ptr] - 1);
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 15:
            if (!ignore_next_command) {
                group = *command_ptr;
                command_ptr++;
                ame_handler->group[group].basis = *command_ptr;
                command_ptr++;
                for (y = 0; *command_ptr != 0xf7; y++) {
                    ame_handler->group[group].channel[y] = command_ptr[0];
                    command_ptr++;
                    ame_handler->group[group].excite_min[y] = command_ptr[0];
                    command_ptr++;
                    ame_handler->group[group].excite_max[y] = command_ptr[0];
                    command_ptr++;
                }
                ame_handler->group[group].num_channels = y;
                command_ptr++;
            } else {
                while (*command_ptr != 0xf7) {
                    command_ptr++;
                }
                command_ptr++;
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            break;
        case 16:
            if (!ignore_next_command) {
                if (!ame_handler->group[command_ptr[0]].basis) {
                    compare_value = gGlobalExcite;
                } else {
                    compare_value = ame_handler->MIDIRegister[ame_handler->group[command_ptr[0]].basis - 1];
                }
                for (counter = 0; counter < ame_handler->group[command_ptr[0]].num_channels; counter++) {
                    if (compare_value > ame_handler->group[command_ptr[0]].excite_min[counter] - 1 &&
                        compare_value < ame_handler->group[command_ptr[0]].excite_max[counter] + 1) {
                        snd_UnmuteMIDIChannel(stream, ame_handler->group[command_ptr[0]].channel[counter]);
                    } else {
                        snd_MuteMIDIChannel(stream, ame_handler->group[command_ptr[0]].channel[counter]);
                    }
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 17:
            if (!ignore_next_command) {
                stop = 1;
                done = 1;
                snd_StartAMESegment(ame_handler, command_ptr[0]);
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 18:
            if (!ignore_next_command) {
                snd_StartAMESegment(ame_handler, command_ptr[0]);
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 19:
            if (!ignore_next_command) {
                ame_handler->MIDIRegister[command_ptr[0]] = command_ptr[1];
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr += 2;
            break;
        case 20:
            if (!ignore_next_command) {
                if (ame_handler->MIDIRegister[command_ptr[0]] < 127) {
                    ame_handler->MIDIRegister[command_ptr[0]]++;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 21:
            if (!ignore_next_command) {
                if (ame_handler->MIDIRegister[command_ptr[0]] > 0) {
                    ame_handler->MIDIRegister[command_ptr[0]]--;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr++;
            break;
        case 22:
            if (!ignore_next_command) {
                if (ame_handler->MIDIRegister[command_ptr[0]] != command_ptr[1]) {
                    ignore_next_command = 1;
                }
            } else {
                if (ignore_next_command == 1) {
                    ignore_next_command = 0;
                }
            }
            command_ptr += 2;
            break;
        }

        if (command_ptr[0] == 0xf7) {
            done = 1;
        }
    }

    if (stop) {
        return 0;
    } else {
        return command_ptr;
    }
}

void snd_SetMIDIRegister(UInt32 handle, SInt32 reg, SInt16 value) {
    AMEHandlerPtr ame;
    if (reg < 0 || reg >= 16) {
        snd_ShowError(92, reg, value, 0, 0);
        return;
    }

    snd_LockMasterTick(10);

    if (((handle >> 24) & 0x1f) != 2 || !(ame = (AMEHandlerPtr)snd_CheckHandlerStillActive(handle))) {
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

    if (((handle >> 24) & 0x1f) != 2 || !(ame = (AMEHandlerPtr)snd_CheckHandlerStillActive(handle))) {
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

    if (!(ame = (AMEHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return 0;
    }

    return ame->MIDIRegister[reg];
}