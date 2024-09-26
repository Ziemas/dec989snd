#include "common.h"
#include "functions.h"
#include "globals.h"

// INCLUDE_ASM("asm/nonmatchings/midi", snd_PlayMIDISound);
UInt32 snd_PlayMIDISound(MIDISoundPtr sound, SInt32 vol, SInt32 pan,
                         SInt16 pitch_mod, SInt16 bend) {
    MIDIBlockHeaderPtr data_stream;
    MIDIHandlerPtr stream_handler;

    data_stream = snd_GetRealMIDIDataStream(sound);
    if (!data_stream || !data_stream->BankPtr) {
        return 0;
    }

    stream_handler = snd_GetFreeMIDIHandler();
    if (!stream_handler) {
        return -1;
    }

    if (vol == 0x7FFFFFFF) {
        vol = 0x400;
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
    stream_handler->Repeats = sound->Repeats;
    stream_handler->SH.Sound = (SoundPtr)sound;
    stream_handler->SH.Original_Vol = sound->Vol;
    stream_handler->SH.Original_Pan = sound->Pan;
    stream_handler->SH.Effects = 0;
    stream_handler->SH.flags = 0;
    stream_handler->SH.VolGroup = sound->VolGroup;

    snd_SetupMIDIStreamHandler(stream_handler, data_stream);
    snd_ActivateHandler(&stream_handler->SH);

    return stream_handler->SH.OwnerID;
}

void snd_SetupMIDIStreamHandler(MIDIHandlerPtr hand,
                                MIDIBlockHeaderPtr stream) {
    hand->BankPtr = stream->BankPtr;
    hand->PlayPos = hand->DataStart = stream->DataStart;
    hand->Tempo = stream->Tempo;
    hand->TickError = 0;
    hand->PPQ = stream->PPQ;
    snd_ResetControllers(hand);
    hand->Flags |= 1u;
}

MIDIBlockHeaderPtr snd_GetRealMIDIDataStream(MIDISoundPtr sound) {
    MIDIBlockHeaderPtr stream;

    stream = sound->MIDIBlock;
    if (!stream) {
        snd_ShowError(28, 0, 0, 0, 0);
        return NULL;
    }

    if (stream->DataID == 0x2044494D) {
        return stream;
    }

    if (sound->Index > ((MultiMIDIBlockHeaderPtr)stream)->NumMIDIBlocks - 1) {
        snd_ShowError(29, 0, 0, 0, 0);
        return NULL;
    }

    return ((MultiMIDIBlockHeaderPtr)stream)->BlockPtr[sound->Index];
}

void snd_MuteMIDIChannel(MIDIHandlerPtr stream, SInt32 channel) {
    stream->MuteState |= 1 << channel;
}

void snd_UnmuteMIDIChannel(MIDIHandlerPtr stream, SInt32 channel) {
    stream->MuteState &= ~(1 << channel);
}

// INCLUDE_ASM("asm/nonmatchings/midi", snd_ProcessMIDITick);
SInt32 snd_ProcessMIDITick(MIDIHandlerPtr stream) {
    static SInt32 in_function = 0;
    SInt32 used_bytes;
    SInt32 stream_done;
    SoundBankPtr bank;

    if (in_function) {
        return 0;
    }

    in_function = 1;

    if (stream->Flags & 1) {
        stream->PPT =
            1000 * gMicsPerTick / (stream->Tempo / (stream->PPQ / 10u));
        stream->RunningStatus = 0;
        stream->TickDelta = 100 * ReadVarLen(stream->PlayPos, &used_bytes);
        stream->TickCountdown =
            (((stream->Tempo * (stream->TickDelta / 100)) / stream->PPQ) +
             (gMicsPerTick - 1)) /
            gMicsPerTick;
        stream->PlayPos += used_bytes;
        stream->Flags &= ~1;
        stream->TickError = 0;
    } else {
        stream->TickCountdown--;
    }

    stream_done = 0;
    while (!stream_done && !stream->TickCountdown) {
        if (*stream->PlayPos < 0) {
            stream->RunningStatus = stream->PlayPos[0];
            stream->PlayPos++;
        }

        if (stream->RunningStatus == 0xff) {
            switch (stream->PlayPos[0]) {
            case 0x2f:
                stream->Repeats--;
                if (!stream->Repeats) {
                    stream_done = 1;
                } else {
                    stream->PlayPos = stream->DataStart;
                    if (stream->Repeats < 0) {
                        stream->Repeats = 0;
                    }
                }
                break;
            case 0x51:
                stream->PlayPos += 2;
                stream->Tempo = (((stream->PlayPos[0] & 0xff) << 16) |
                                 ((stream->PlayPos[1] & 0xff) << 8) |
                                 stream->PlayPos[2] & 0xff);
                stream->PPT =
                    1000 * gMicsPerTick / (stream->Tempo / (stream->PPQ / 10u));
                stream->PlayPos += 3;
                break;
            default:
                snd_ShowError(96, *stream->PlayPos, 0, 0, 0);
                stream_done = 1;
                break;
            }
        } else if (stream->RunningStatus == 0xf0) {
            if (stream->PlayPos[0] == 0x75) {
                stream->PlayPos++;
                stream->PlayPos = snd_AMEFunction(stream, stream->PlayPos);
                if (!stream->PlayPos) {
                    stream_done = 1;
                } else {
                    stream->PlayPos++;
                }
            } else {
                while ((UInt8)stream->PlayPos[0] != 0xF7) {
                    ++stream->PlayPos;
                }
                ++stream->PlayPos;
                snd_ShowError(97, 0, 0, 0, 0);
            }
        } else {
            switch (stream->RunningStatus & 0xf0) {
            case 0x80:
                snd_MIDINoteOff(stream);
                stream->PlayPos += 2;
                break;
            case 0x90:
                if (!stream->PlayPos[1]) {
                    snd_MIDINoteOff(stream);
                } else {
                    if (!((stream->MuteState >> (stream->RunningStatus & 0xf)) &
                          1)) {
                        snd_MIDINoteOn(stream);
                    }
                }
                stream->PlayPos += 2;
                break;
            case 0xd0:
                snd_MIDINoteOff(stream);
                stream->PlayPos = stream->PlayPos + 1;
                break;
            case 0xe0:
                snd_PitchBend(stream);
                stream->PlayPos += 2;
                break;
            case 0xb0:
                switch (stream->PlayPos[0]) {
                case 0:
                    bank = snd_FindBankByNum(stream->PlayPos[1]);
                    if (!bank) {
                        stream_done = 1;
                        snd_ShowError(104, stream->PlayPos[1], 0, 0, 0);
                    } else {
                        stream->BankPtr = bank;
                    }
                    break;
                case 7:
                    stream->Vol[stream->RunningStatus & 0xf] =
                        stream->PlayPos[1];
                    break;
                case 0xa:
                    stream->Pan[stream->RunningStatus & 0xf] =
                        snd_MIDITo360Pan(stream->PlayPos[1]);
                    break;
                case 0x40:
                    if (stream->PlayPos[1] >= 64) {
                        stream->DamperState |= 1
                                               << (stream->RunningStatus & 0xF);
                    } else {
                        snd_ReleaseDamper(stream);
                    }
                    break;
                }
                stream->PlayPos += 2;
                break;
            case 0xc0:
                stream->Prog[stream->RunningStatus & 0xF] = stream->PlayPos[0];
                stream->PlayPos++;
                break;
            default:
                snd_ShowError(98, stream->RunningStatus & 0xF0, 0, 0, 0);
                stream_done = 1;
                break;
            }
        }

        if (!stream_done) {
            stream->TickDelta = 100 * ReadVarLen(stream->PlayPos, &used_bytes) +
                                stream->TickError;

            if (stream->TickDelta < 0 || stream->TickDelta < stream->PPT / 2) {
                stream->TickError = stream->TickDelta;
                stream->TickDelta = 0;
            }

            if (stream->TickDelta) {
                stream->TickCountdown =
                    (((stream->Tempo * (stream->TickDelta / 100)) /
                      stream->PPQ) +
                     (gMicsPerTick - 1)) /
                    gMicsPerTick;
                stream->TickError =
                    stream->TickDelta - stream->PPT * stream->TickCountdown;
            }

            stream->PlayPos = &stream->PlayPos[used_bytes];
        }
    }

    in_function = 0;
    return stream_done;
}

UInt32 ReadVarLen(UInt8 *buffer, SInt32 *used_bytes) {
    UInt32 ret_val;
    SInt32 used;

    used = 1;
    if (((ret_val = *buffer) & 0x80) != 0) {
        ret_val &= 0x7Fu;
        do {
            ++buffer;
            ++used;
            ret_val = (ret_val << 7) + (*buffer & 0x7F);
        } while ((*buffer & 0x80) != 0);
    }
    *used_bytes = used;
    return ret_val;
}

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
