#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libsd.h"
#include "sdmacro.h"

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

void snd_MIDINoteOn(MIDIHandlerPtr stream) {
    TonePtr tones[32];
    SInt32 num_tones;
    SInt32 x;
    SInt32 voice;
    SInt32 midi_channel;
    SInt32 program;
    SInt8 note;
    SInt8 vol;
    SInt32 pan_calc;
    SInt32 core;
    SInt32 c_v;

    midi_channel = stream->RunningStatus & 0xF;
    program = stream->Prog[midi_channel];
    note = *stream->PlayPos;
    vol = stream->PlayPos[1];
    num_tones = snd_CollectTones(stream->BankPtr, program, note, tones);

    if (!snd_LockVoiceAllocatorEx(1, 42)) {
        return;
    }

    for (x = 0; x < num_tones; x++) {
        voice =
            snd_AllocateVoice(stream->SH.Sound->VolGroup, tones[x]->Priority);
        if (voice != -1) {
            core = voice / 24;
            c_v = voice % 24;
            stream->SH.Voices.core[core] |= 1 << c_v;
            gChannelStatus[voice].Owner = &stream->SH;
            gChannelStatus[voice].OwnerProc = snd_MIDISoundOwnerProc;
            gChannelStatus[voice].Tone = tones[x];
            gChannelStatus[voice].StartNote = note;
            gChannelStatus[voice].StartFine = 0;
            gChannelStatus[voice].Priority = tones[x]->Priority;
            gChannelStatus[voice].VolGroup = stream->SH.Sound->VolGroup;
            gChannelStatus[voice].Volume.left =
                gChannelStatus[voice].Volume.right = 0;
            pan_calc = stream->Pan[midi_channel] + stream->SH.Current_Pan;
            if (pan_calc >= 360)
                pan_calc -= 360;
            snd_MakeVolumesB(
                stream->SH.Current_Vol, vol * stream->Vol[midi_channel] / 127,
                pan_calc, stream->BankPtr->FirstProg[program].Vol,
                stream->BankPtr->FirstProg[program].Pan, tones[x]->Vol,
                tones[x]->Pan, &gChannelStatus[voice].Volume);
            gChannelStatus[voice].OwnerData.MIDIData.MidiChannel = midi_channel;
            gChannelStatus[voice].Current_PB = stream->PitchBend[midi_channel];
            gChannelStatus[voice].Current_PM = stream->SH.Current_PM;
            gChannelStatus[voice].OwnerData.MIDIData.KeyOnVelocity = vol;
            gChannelStatus[voice].OwnerData.MIDIData.KeyOnProg =
                &stream->BankPtr->FirstProg[program];
            gChannelStatus[voice].OwnerData.MIDIData.ShouldBeOff = 0;
            snd_StartVAGVoice(voice, 0);
        }
    }

    snd_UnlockVoiceAllocator();
}

void snd_MIDINoteOff(MIDIHandlerPtr stream) {
    VoiceAttributes *walk;
    SInt32 midi_channel;
    SInt8 note;

    midi_channel = stream->RunningStatus & 0xf;
    note = stream->PlayPos[0];

    if (!snd_LockVoiceAllocatorEx(1, 43)) {
        return;
    }

    walk = gPlayingListHead;
    while (walk) {
        if (walk->Owner == stream &&
            walk->OwnerData.MIDIData.MidiChannel == midi_channel &&
            walk->StartNote == note) {
            if (((stream->DamperState >> midi_channel) & 1) == 0) {
                snd_KeyOffVoice(walk->voice);
            } else {
                walk->OwnerData.MIDIData.ShouldBeOff = 1;
            }
        }

        walk = walk->playlist;
    }

    snd_UnlockVoiceAllocator();
}

void snd_ReleaseDamper(MIDIHandlerPtr stream) {
    VoiceAttributes *walk;
    SInt32 midi_channel;

    midi_channel = stream->RunningStatus & 0xf;
    stream->DamperState &= ~(1 << midi_channel);

    snd_LockVoiceAllocatorEx(1, 44);

    walk = gPlayingListHead;
    while (walk) {
        if (walk->Owner == stream &&
            walk->OwnerData.MIDIData.MidiChannel == midi_channel &&
            walk->OwnerData.MIDIData.ShouldBeOff == 1) {

            snd_KeyOffVoice(walk->voice);
        }

        walk = walk->playlist;
    }

    snd_UnlockVoiceAllocator();
}

void snd_ResetControllers(MIDIHandlerPtr stream) {
    SInt32 x;

    stream->MuteState = 0;
    for (x = 0; x < 16; x++) {
        stream->Prog[x] = 0;
        stream->Vol[x] = 127;
        stream->Pan[x] = 0;
        stream->PitchBend[x] = 0;
    }

    stream->DamperState = 0;
}

void snd_PitchBend(MIDIHandlerPtr stream) {
    SInt32 core;
    SInt32 c_v;
    SInt32 new_note;
    SInt32 new_fine;
    VoiceAttributes *walk;
    UInt32 hold;
    SInt32 intr_state;
    SInt32 dis;
    SInt32 midi_channel;

    midi_channel = stream->RunningStatus & 0xF;
    hold = (stream->PlayPos[0] & 0x7F | ((stream->PlayPos[1] & 0x7Fu) << 7));
    hold = 0xFFFF * hold / 0x3FFF;
    stream->PitchBend[midi_channel] = (SInt32)hold - 0x8000;
    snd_LockVoiceAllocatorEx(1, 45);
    walk = gPlayingListHead;
    dis = CpuSuspendIntr(&intr_state);

    while (walk) {
        if (walk->Owner == stream &&
            walk->OwnerData.MIDIData.MidiChannel == midi_channel) {
            walk->Current_PB = stream->PitchBend[midi_channel];
            core = walk->voice / 24;
            c_v = walk->voice % 24;

            snd_PitchBendTone(walk->Tone, walk->Current_PB, walk->Current_PM,
                              walk->StartNote, walk->StartFine, &new_note,
                              &new_fine);

            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH,
                          PS1Note2Pitch(walk->Tone->CenterNote,
                                        walk->Tone->CenterFine, new_note,
                                        new_fine));
        }

        walk = walk->playlist;
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    snd_UnlockVoiceAllocator();
}

void snd_SetMIDISoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan) {
    MIDIHandlerPtr stream;

    if (!(stream = (MIDIHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }

    snd_SetMIDIHandlerVolumePan(stream, vol, pan);
}

void snd_SetMIDIHandlerVolumePan(MIDIHandlerPtr stream, SInt32 vol,
                                 SInt32 pan) {
    SInt32 pan_calc;
    SpuVolume spu_vol;
    SInt32 core;
    SInt32 c_v;
    VoiceAttributes *walk;
    SInt32 intr_state;
    SInt32 dis;

    snd_LockVoiceAllocatorEx(1, 46);

    if (vol < 0) {
        stream->SH.Current_Vol = -1 * vol;
    } else if (vol != 0x7FFFFFFF) {
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

    walk = gPlayingListHead;
    while (walk) {
        if (walk->Owner == stream) {
            pan_calc = stream->Pan[walk->OwnerData.MIDIData.MidiChannel] +
                       stream->SH.Current_Pan;

            if (pan_calc >= 360) {
                pan_calc -= 360;
            }

            snd_MakeVolumesB(
                stream->SH.Current_Vol,
                walk->OwnerData.MIDIData.KeyOnVelocity *
                    stream->Vol[walk->OwnerData.MIDIData.MidiChannel] / 127,
                pan_calc, walk->OwnerData.MIDIData.KeyOnProg->Vol,
                walk->OwnerData.MIDIData.KeyOnProg->Pan, walk->Tone->Vol,
                walk->Tone->Pan, &walk->Volume);

            if (!(stream->SH.flags & 2)) {
                spu_vol.left = (UInt16)snd_AdjustVolToGroup(walk->Volume.left,
                                                            walk->VolGroup) >>
                               1;
                spu_vol.right = (UInt16)snd_AdjustVolToGroup(walk->Volume.right,
                                                             walk->VolGroup) >>
                                1;

                core = walk->voice / 24;
                c_v = walk->voice % 24;
                dis = CpuSuspendIntr(&intr_state);
                sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, spu_vol.left);
                sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, spu_vol.right);
                if (!dis) {
                    CpuResumeIntr(intr_state);
                }
            }
        }

        walk = walk->playlist;
    }

    snd_UnlockVoiceAllocator();
}

void snd_SetMIDISoundPitchModifier(UInt32 handle, SInt16 mod) {
    MIDIHandlerPtr stream;

    if (!(stream = (MIDIHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }

    snd_SetMIDIHandlerPitchModifier(stream, mod);
}

void snd_SetMIDIHandlerPitchModifier(MIDIHandlerPtr stream, SInt16 mod) {
    SInt32 core;
    SInt32 c_v;
    SInt32 new_note;
    SInt32 new_fine;
    VoiceAttributes *walk;
    SInt32 intr_state;
    SInt32 dis;

    snd_LockVoiceAllocatorEx(1, 47);
    stream->SH.Current_PM = mod;
    walk = gPlayingListHead;
    while (walk) {
        if (walk->Owner == stream) {
            walk->Current_PM = mod;
            if (!(stream->SH.flags & 2)) {
                snd_PitchBendTone(walk->Tone, walk->Current_PB,
                                  walk->Current_PM, walk->StartNote,
                                  walk->StartFine, &new_note, &new_fine);

                core = walk->voice / 24;
                c_v = walk->voice % 24;
                dis = CpuSuspendIntr(&intr_state);
                sceSdSetParam((core | SD_VOICE(c_v)) | SD_VP_PITCH,
                              PS1Note2Pitch(walk->Tone->CenterNote,
                                            walk->Tone->CenterFine, new_note,
                                            new_fine));
                if (!dis) {
                    CpuResumeIntr(intr_state);
                }
            }
        }

        walk = walk->playlist;
    }

    snd_UnlockVoiceAllocator();
}

void snd_MIDISoundOwnerProc(SInt32 voice, UInt32 owner, SInt32 flag) {
    MIDIHandlerPtr snd;
    SInt32 core;
    SInt32 c_v;

    snd = (MIDIHandlerPtr)owner;
    core = voice / 24;
    c_v = voice % 24;
    snd->SH.Voices.core[core] &= ~(1 << c_v);
}
