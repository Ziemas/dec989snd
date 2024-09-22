#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libsd.h"
#include "sdmacro.h"
#include "stdio.h"

SInt8 gBlockGlobalReg[32] = {0};
GrainHandler gGrainHandler[] = {
    snd_SFX_GRAIN_TYPE_NULL,
    snd_SFX_GRAIN_TYPE_TONE,
    snd_SFX_GRAIN_TYPE_XREF_ID,
    snd_SFX_GRAIN_TYPE_XREF_NUM,
    snd_SFX_GRAIN_TYPE_LFO_SETTINGS,
    snd_SFX_GRAIN_TYPE_STARTCHILDSOUND,
    snd_SFX_GRAIN_TYPE_STOPCHILDSOUND,
    snd_SFX_GRAIN_TYPE_PLUGIN_MESSAGE,
    snd_SFX_GRAIN_TYPE_BRANCH,
    snd_SFX_GRAIN_TYPE_TONE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_UNKNOWN_GRAIN_TYPE,
    snd_SFX_GRAIN_TYPE_CONTROL_NULL,
    snd_SFX_GRAIN_TYPE_LOOP_START,
    snd_SFX_GRAIN_TYPE_LOOP_END,
    snd_SFX_GRAIN_TYPE_LOOP_CONTINUE,
    snd_SFX_GRAIN_TYPE_STOP,
    snd_SFX_GRAIN_TYPE_RAND_PLAY,
    snd_SFX_GRAIN_TYPE_RAND_DELAY,
    snd_SFX_GRAIN_TYPE_RAND_PB,
    snd_SFX_GRAIN_TYPE_PB,
    snd_SFX_GRAIN_TYPE_ADD_PB,
    snd_SFX_GRAIN_TYPE_SET_REGISTER,
    snd_SFX_GRAIN_TYPE_SET_REGISTER_RAND,
    snd_SFX_GRAIN_TYPE_INC_REGISTER,
    snd_SFX_GRAIN_TYPE_DEC_REGISTER,
    snd_SFX_GRAIN_TYPE_TEST_REGISTER,
    snd_SFX_GRAIN_TYPE_MARKER,
    snd_SFX_GRAIN_TYPE_GOTO_MARKER,
    snd_SFX_GRAIN_TYPE_GOTO_RANDOM_MARKER,
    snd_SFX_GRAIN_TYPE_WAIT_FOR_ALL_VOICES,
    snd_SFX_GRAIN_TYPE_PLAY_CYCLE,
    snd_SFX_GRAIN_TYPE_ADD_REGISTER,
    snd_SFX_GRAIN_TYPE_KEY_OFF_VOICES,
    snd_SFX_GRAIN_TYPE_KILL_VOICES,
    snd_SFX_GRAIN_TYPE_ON_STOP_MARKER,
    snd_SFX_GRAIN_TYPE_COPY_REGISTER,
};

UInt32 snd_PlaySFX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan,
                   SInt16 pitch_mod, SInt16 bend) {
    return snd_PlaySFX_EX(block, sound, vol, pan, pitch_mod, bend, -1, -1);
}

UInt32 snd_PlaySFXWithStruct(SFXBlock2Ptr block, SInt32 sound, SInt16 sfx_vol,
                             SInt16 sfx_pan, SndPlayParamsPtr params) {
    SInt32 play_vol;
    SFX2 *sfx;
    BlockSoundHandlerPtr hand;
    SInt32 stop_sound;
    SInt32 i;
    SInt32 vol;
    SInt32 pan;
    SInt16 pitch_mod;
    SInt16 bend;

    hand = NULL;

    if (sound < 0 || sound >= block->NumSounds) {
        return 0;
    }

    sfx = &block->FirstSound[sound];
    sfx->Flags |= 0x4000;

    if (!sfx->NumGrains) {
        return 0;
    }

    if (sfx_vol == -1) {
        sfx_vol = sfx->Vol;
    }

    if (sfx_pan == -1) {
        sfx_pan = sfx->Pan;
    }

    if (!(params->mask & 4)) {
        vol = 0x400;
    } else {
        vol = params->vol;
    }

    if (!(params->mask & 8)) {
        pan = -1;
    } else {
        pan = params->pan;
    }

    if (!(params->mask & 0x10)) {
        pitch_mod = 0;
    } else {
        pitch_mod = params->pitch_mod;
    }

    if (!(params->mask & 0x20)) {
        bend = 0;
    } else {
        bend = params->pitch_bend;
    }

    if (vol == 0x7fffffff) {
        vol = 0x400;
    }

    play_vol = (sfx_vol * vol) >> 10;
    if (play_vol >= 128) {
        play_vol = 127;
    }

    if (pan == -1 || pan == -2) {
        pan = sfx_pan;
    }

    if (sfx->Flags & 2) {
        snd_StopAllHandlersForSound((SoundPtr)sfx, 0, 1);
    }

    snd_LockMasterTick(26);
    hand = snd_GetFreeBlockSoundHandler(sfx, vol, !(params->mask & 0x80000000));
    if (!hand) {
        snd_UnlockMasterTick();
        return 0;
    }

    hand->SH.Sound = (SoundPtr)sfx;
    hand->orig_sound = sfx;
    snd_RemoveLFOsForHandler(hand);
    snd_UnlockMasterTick();
    hand->SH.Effects = NULL;
    hand->SH.Original_Vol = sfx_vol;
    hand->SH.Original_Pan = sfx_pan;
    hand->SH.Current_Vol = play_vol;
    hand->SH.Current_Pan = pan;
    hand->SH.Current_PM = pitch_mod;
    hand->SH.flags = 0;
    hand->SH.VolGroup = sfx->VolGroup;
    hand->SH.Voices.core[0] = 0;
    hand->SH.Voices.core[1] = 0;
    hand->Current_Note = 60;
    hand->Current_Fine = 0;
    hand->Current_PB = bend;
    hand->LFO_Vol = 0;
    hand->App_Vol = vol;
    hand->LFO_Pan = 0;
    hand->App_Pan = pan;
    hand->LFO_PB = 0;
    hand->App_PB = bend;
    hand->LFO_PM = 0;
    hand->App_PM = pitch_mod;
    hand->CountDown = sfx->FirstGrain->Delay;
    hand->NextGrain = 0;
    if (params->mask & 0x40) {
        hand->Registers[0] = params->reg[0];
        hand->Registers[1] = params->reg[1];
        hand->Registers[2] = params->reg[2];
        hand->Registers[3] = params->reg[3];
    } else {
        hand->Registers[0] = 0;
        hand->Registers[1] = 0;
        hand->Registers[2] = 0;
        hand->Registers[3] = 0;
    }
    hand->sk_doing_skipping_play = 0;
    hand->sk_grains_to_play = 0;
    hand->sk_grains_to_skip = 0;
    hand->block = block;
    for (i = 0; i < 4; i++) {
        hand->lfo[i].type = 0;
        hand->lfo[i].next_step = 0;
        hand->lfo[i].target = 0;
        hand->lfo[i].setup_flags = 0;
        hand->lfo[i].running_flags = 0;
        hand->lfo[i].handler = hand;
    }

    stop_sound = 0;
    while (!stop_sound && hand->NextGrain != -1 && !hand->CountDown) {
        stop_sound = snd_DoGrain(hand);
    }

    if (stop_sound || (!hand->CountDown && !hand->SH.Voices.core[0] &&
                       !hand->SH.Voices.core[1] && !hand->SH.first_child)) {
        // in this case the sound finished all its grains
        //  and has no voices or children alive
        snd_StopHandlerPtr((GSoundHandlerPtr)hand, 1, 0, 0);
        hand->SH.Sound = NULL;
        hand = NULL;
    }

    if (hand) {
        snd_ActivateHandler((GSoundHandlerPtr)hand);
        return hand->SH.OwnerID;
    }

    return 0;
}

UInt32 snd_PlaySFX_EX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan,
                      SInt16 pitch_mod, SInt16 bend, SInt16 sfx_vol,
                      SInt16 sfx_pan) {
    SndPlayParams params;

    params.mask = 0x3C;
    params.vol = vol;
    params.pan = pan;
    params.pitch_mod = pitch_mod;
    params.pitch_bend = bend;

    return snd_PlaySFXWithStruct(block, sound, sfx_vol, sfx_pan, &params);
}

UInt32 snd_PlaySFXGrain(SFXBlock2Ptr block, SInt32 sindex, SInt32 gindex) {
    SFX2 *sfx;
    BlockSoundHandlerPtr hand;

    hand = NULL;
    sfx = &block->FirstSound[sindex];
    if (!sfx->NumGrains) {
        return 0;
    }

    snd_LockMasterTick(27);
    hand = snd_GetFreeBlockSoundHandler(0, 0, 0);
    if (!hand) {
        snd_UnlockMasterTick();
        return 0;
    }

    hand->SH.Sound = (SoundPtr)sfx;
    snd_UnlockMasterTick();
    hand->SH.Original_Vol = sfx->Vol;
    hand->SH.Original_Pan = sfx->Pan;
    hand->SH.Current_Vol = sfx->Vol;
    hand->App_Vol = 0x400;
    hand->LFO_Vol = 0;
    hand->SH.Current_Pan = sfx->Pan;
    hand->App_Pan = 0;
    hand->LFO_Pan = 0;
    hand->SH.Current_PM = 0;
    hand->Current_PB = 0;
    hand->App_PB = 0;
    hand->LFO_PB = 0;
    hand->SH.Effects = 0;
    hand->SH.flags = 0;
    hand->SH.VolGroup = sfx->VolGroup;
    hand->CountDown = 0;
    hand->NextGrain = gindex;
    hand->Current_Note = 60;
    hand->Current_Fine = 0;

    snd_DoGrain(hand);
    hand->NextGrain = -1;
    if (!hand->CountDown && !hand->SH.Voices.core[0] &&
        !hand->SH.Voices.core[1] && !hand->SH.first_child) {
        snd_DeactivateHandler(&hand->SH, 0);
        hand = 0;
    }

    if (hand) {
        snd_ActivateHandler(&hand->SH);
        return hand->SH.OwnerID;
    }

    return 0;
}

SInt8 snd_GetSFXGlobalReg(SInt32 which) {
    if (which <= 0 || which > 32) {
        return 0;
    }

    return gBlockGlobalReg[which - 1];
}

void snd_SetSFXGlobalReg(SInt32 which, SInt8 val) {
    if (which <= 0 || which > 32) {
        return;
    }

    gBlockGlobalReg[which - 1] = val;
}

SInt8 snd_GetSFXSoundReg(UInt32 handle, SInt32 which) {
    BlockSoundHandlerPtr hand;
    SInt8 val;

    if (which < 0 || which >= 4) {
        return 0;
    }

    snd_LockMasterTick(28);
    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return 0;
    }

    val = hand->Registers[which];
    snd_UnlockMasterTick();

    return val;
}

void snd_SetSFXSoundReg(UInt32 handle, SInt32 which, SInt8 val) {
    BlockSoundHandlerPtr hand;

    if (which < 0 || which >= 4) {
        return;
    }

    snd_LockMasterTick(29);
    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    hand->Registers[which] = val;

    snd_UnlockMasterTick();
}

void snd_SetAllSFXSoundReg(UInt32 handle, SInt8 *vals) {
    BlockSoundHandlerPtr hand;
    SInt32 i;

    snd_LockMasterTick(30);
    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    for (i = 0; i < 4; i++) {
        hand->Registers[i] = vals[i];
    }

    snd_UnlockMasterTick();
}

SInt32 snd_ProcessBlockSoundTick(BlockSoundHandlerPtr handler) {
    SInt32 stop_sound;

    stop_sound = 0;
    if (handler->NextGrain == -1) {
        if (!handler->SH.first_child && !handler->SH.Voices.core[0] &&
            !handler->SH.Voices.core[1]) {
            return 1;
        }
        return 0;
    }

    handler->CountDown--;
    while (!stop_sound && handler->NextGrain != -1 && handler->CountDown <= 0) {
        stop_sound = snd_DoGrain(handler);
    }

    return stop_sound;
}

BOOL snd_DoBlockSoundStop(BlockSoundHandlerPtr handler, SInt32 silence,
                          BOOL vlimit_stop) {
    SInt32 stop_sound;
    SInt32 stop_index;

    stop_sound = 0;
    stop_index = snd_GetOnStopGrainIndex(handler);
    if (stop_index == -1 || silence) {
        snd_RemoveLFOsForHandler(handler);
        return 0;
    }

    snd_LockMasterTick(19);
    snd_KeyOffVoicesEx(&handler->SH.Voices, 0);
    handler->SH.Voices.core[0] = handler->SH.Voices.core[1] = 0;
    handler->NextGrain = stop_index + 1;
    if (vlimit_stop) {
        handler->SH.flags |= 0x20;
    }

    while (!stop_sound && handler->NextGrain != -1) {
        stop_sound = snd_DoGrain(handler);
    }

    snd_UnlockMasterTick();

    if (!handler->SH.Voices.core[0] && !handler->SH.Voices.core[1] &&
        !handler->SH.first_child) {
        snd_RemoveLFOsForHandler(handler);
        return 0;
    }

    return 1;
}

SInt32 snd_GetOnStopGrainIndex(BlockSoundHandlerPtr handler) {
    SInt32 i;
    SFX2Ptr sfx;

    sfx = (SFX2Ptr)handler->SH.Sound;
    for (i = 0; i < sfx->NumGrains; i++) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x2b) {
            if (i == sfx->NumGrains - 1) {
                return -1;
            }

            return i;
        }
    }

    return -1;
}

SInt32 snd_SFX_GRAIN_TYPE_NULL(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                               SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_TONE(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                               SFXGrain2Ptr grain) {
    SInt32 voice;
    SInt32 g_pan;
    SInt32 priority;
    TonePtr tone;
    SInt32 core;
    SInt32 c_v;

    g_pan = 0;
    if (handler->SH.flags & 0x20) {
        return 0;
    }

    tone = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);
    handler->SH.Current_Vol =
        ((handler->App_Vol * handler->SH.Original_Vol >> 10) +
         handler->LFO_Vol);

    if (handler->SH.Current_Vol >= 128) {
        handler->SH.Current_Vol = 127;
    }

    if (handler->SH.Current_Vol < 0) {
        handler->SH.Current_Vol = 0;
    }

    handler->SH.Current_Pan = handler->App_Pan + handler->LFO_Pan;
    while (handler->SH.Current_Pan >= 360) {
        handler->SH.Current_Pan -= 360;
    }
    while (handler->SH.Current_Pan < 0) {
        handler->SH.Current_Pan += 360;
    }

    priority = snd_ScalePriorityForVolume(handler->App_Vol, tone);

    snd_LockVoiceAllocatorEx(1, 26);
    if (tone->Flags & 8) {
        voice = snd_AllocateNoiseVoice(sfx->VolGroup, priority);
    } else {
        voice = snd_AllocateVoice(sfx->VolGroup, priority);
    }

    if (voice != -1) {
        core = voice / 24;
        c_v = voice % 24;
        handler->SH.Voices.core[core] |= 1 << c_v;
        gChannelStatus[voice].Owner = &handler->SH;
        gChannelStatus[voice].OwnerProc = snd_SFXOwnerProc;
        gChannelStatus[voice].Tone = tone;

        if (tone->Vol < 0) {
            if (tone->Vol >= -4) {
                gChannelStatus[voice].OwnerData.BlockData.g_vol =
                    handler->Registers[-1 * tone->Vol - 1];
            } else if (tone->Vol == -5) {
                gChannelStatus[voice].OwnerData.BlockData.g_vol =
                    snd_RandomUInt16() % 0x80;
            } else {
                gChannelStatus[voice].OwnerData.BlockData.g_vol =
                    gBlockGlobalReg[-1 * tone->Vol - 6];
            }
        } else {
            gChannelStatus[voice].OwnerData.BlockData.g_vol = tone->Vol;
        }

        if (gChannelStatus[voice].OwnerData.BlockData.g_vol < 0) {
            gChannelStatus[voice].OwnerData.BlockData.g_vol = 0;
        }

        if (tone->Pan < 0) {
            if (tone->Pan >= -4) {
                gChannelStatus[voice].OwnerData.BlockData.g_pan =
                    360 * handler->Registers[-1 * tone->Pan - 1] / 127;
            } else if (tone->Pan == -5) {
                gChannelStatus[voice].OwnerData.BlockData.g_pan =
                    snd_RandomUInt16() % 360;
            } else {
                gChannelStatus[voice].OwnerData.BlockData.g_pan =
                    360 * gBlockGlobalReg[-1 * tone->Pan - 6] / 127;
            }
        } else {
            gChannelStatus[voice].OwnerData.BlockData.g_pan = tone->Pan;
        }

        while (gChannelStatus[voice].OwnerData.BlockData.g_pan < 0) {
            gChannelStatus[voice].OwnerData.BlockData.g_pan += 360;
        }
        while (gChannelStatus[voice].OwnerData.BlockData.g_pan >= 360) {
            gChannelStatus[voice].OwnerData.BlockData.g_pan -= 360;
        }

        gChannelStatus[voice].StartNote = handler->Current_Note;
        gChannelStatus[voice].StartFine = handler->Current_Fine;
        gChannelStatus[voice].Current_PB = handler->Current_PB;
        gChannelStatus[voice].Current_PM = handler->SH.Current_PM;
        gChannelStatus[voice].Priority = priority;
        gChannelStatus[voice].VolGroup = sfx->VolGroup;

        if (((SFX2Ptr)handler->SH.Sound)->Flags & 4) {
            g_pan =
                snd_CollapsePan(gChannelStatus[voice].OwnerData.BlockData.g_pan,
                                handler->App_Vol, sfx);
        } else {
            g_pan = gChannelStatus[voice].OwnerData.BlockData.g_pan;
        }

        gChannelStatus[voice].Volume.left = gChannelStatus[voice].Volume.right =
            0;

        snd_MakeVolumes(127, 0, handler->SH.Current_Vol,
                        handler->SH.Current_Pan,
                        gChannelStatus[voice].OwnerData.BlockData.g_vol, g_pan,
                        &gChannelStatus[voice].Volume);

        snd_StartVAGVoice(voice, (tone->Flags & 8) != 0);
    }

    snd_UnlockVoiceAllocator();
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_XREF_ID(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                  SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_XREF_NUM(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                   SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_LFO_SETTINGS(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    LFOTracker *lfo;
    SInt32 index;
    LFOParams *lfop;

    lfop = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);
    index = lfop->which_lfo;
    lfo = &handler->lfo[index];
    lfo->target = lfop->target;
    if (!lfo->target) {
        snd_RemoveLFO(lfo);
        lfo->type = 0;
        return 0;
    }

    lfo->type = lfop->shape;
    lfo->target = lfop->target;
    lfo->target_extra = lfop->target_extra;
    lfo->setup_flags = lfop->flags;
    lfo->depth = lfop->depth;
    lfo->orig_depth = lfop->depth;
    lfo->step_size = lfop->step_size;
    lfo->orig_step_size = lfop->step_size;
    lfo->state_hold1 = 0;
    lfo->last_lfo = 0;

    if (lfo->type == 2) {
        lfo->state_hold1 = lfop->duty_cycle;
    }

    lfo->state_hold2 = 0;

    if (lfo->setup_flags & 2) {
        lfo->next_step = (snd_RandomUInt16() & 0x7FF) << 16;
    } else {
        lfo->next_step = lfop->start_offset << 16;
    }

    snd_InitLFO(lfo);

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_CONTROL_NULL(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_LOOP_START(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                     SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_LOOP_END(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                   SFXGrain2Ptr grain) {
    SInt32 found;
    SInt32 i;

    found = 0;
    for (i = handler->NextGrain - 1; i > -1 && !found; i--) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x15) {
            handler->NextGrain = i - 1;
            found = 1;
        }
    }

    if (!found) {
        snd_ShowError(8, 0, 0, 0, 0);
        return -1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_LOOP_CONTINUE(BlockSoundHandlerPtr handler,
                                        SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 found;
    SInt32 i;

    found = 0;
    for (i = handler->NextGrain + 1; i < sfx->NumGrains && !found; i++) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x16) {
            handler->NextGrain = i;
            found = 1;
        }
    }

    if (!found) {
        snd_ShowError(9, 0, 0, 0, 0);
        return -1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_STOP(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                               SFXGrain2Ptr grain) {
    return -1;
}

SInt32 snd_GetSoundIndexInBlock(SFXBlock2Ptr block, SFX2 *sfx) {
    int i;

    for (i = 0; i < block->NumSounds; i++) {
        if (&block->FirstSound[i] == sfx) {
            return i;
        }
    }

    return -1;
}

SInt32 snd_SFX_GRAIN_TYPE_RAND_PLAY(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                    SFXGrain2Ptr grain) {
    SInt32 work32;

    if (handler->sk_doing_skipping_play) {
        snd_ShowError(10, handler->block->BlockID,
                      snd_GetSoundIndexInBlock(handler->block, sfx),
                      handler->NextGrain + 1, 0);
        return -1;
    } else {
        work32 = snd_RandomUInt16() % grain->OpcodeData.MicroOp.Arg[0];

        if (work32 == grain->OpcodeData.MicroOp.Arg[2]) {
            work32++;
            if (work32 == grain->OpcodeData.MicroOp.Arg[0]) {
                work32 = 0;
            }
        }

        grain->OpcodeData.MicroOp.Arg[2] = work32;
        handler->NextGrain += grain->OpcodeData.MicroOp.Arg[1] * work32;
        handler->sk_grains_to_play = grain->OpcodeData.MicroOp.Arg[1] + 1;
        handler->sk_grains_to_skip =
            (grain->OpcodeData.MicroOp.Arg[0] - (work32 + 1)) *
            grain->OpcodeData.MicroOp.Arg[1];
        handler->sk_doing_skipping_play = 1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_RAND_DELAY(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                     SFXGrain2Ptr grain) {
    return snd_RandomUInt16() % ((grain->OpcodeData.Opcode & 0xffffff) + 1);
}

SInt32 snd_SFX_GRAIN_TYPE_RAND_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                  SFXGrain2Ptr grain) {
    SInt32 work32;

    work32 = (0xffff * (snd_RandomUInt16() % 0x7fff)) / 0x7fff;
    work32 = work32 - 0x8000;
    work32 = grain->OpcodeData.MicroOp.Arg[0] * work32 / 100;
    snd_SetSFXPitchbend(handler->SH.OwnerID, work32);

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                             SFXGrain2Ptr grain) {
    SInt32 work32;

    if (grain->OpcodeData.MicroOp.Arg[0] < 0) {
        work32 = -0x8000 * grain->OpcodeData.MicroOp.Arg[0] / -128;
    } else {
        work32 = 0x7FFF * grain->OpcodeData.MicroOp.Arg[0] / 127;
    }

    snd_SetSFXPitchbend(handler->SH.OwnerID, work32);

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_ADD_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                 SFXGrain2Ptr grain) {
    SInt32 work32;

    work32 =
        handler->Current_PB + 0x7FFF * grain->OpcodeData.MicroOp.Arg[0] / 127;
    if (work32 > 0x7fff) {
        work32 = 0x7fff;
    }
    if (work32 < -0x8000) {
        work32 = -0x8000;
    }

    snd_SetSFXPitchbend(handler->SH.OwnerID, work32);

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_SET_REGISTER(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] =
            grain->OpcodeData.MicroOp.Arg[1];
    } else {
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] =
            grain->OpcodeData.MicroOp.Arg[1];
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_SET_REGISTER_RAND(BlockSoundHandlerPtr handler,
                                            SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 work32;
    work32 =
        (grain->OpcodeData.MicroOp.Arg[2] - grain->OpcodeData.MicroOp.Arg[1]) +
        1;
    work32 = (snd_RandomUInt16() % work32);
    work32 += grain->OpcodeData.MicroOp.Arg[1];

    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] = work32;
    } else {
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] = work32;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_INC_REGISTER(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 work32;

    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        work32 = handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] + 1;
        if (work32 > 127) {
            work32 = 127;
        }
        handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] = work32;
    } else {
        work32 = gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] + 1;
        if (work32 > 127) {
            work32 = 127;
        }
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] = work32;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_DEC_REGISTER(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 work32;

    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        work32 = handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] - 1;
        if (work32 < -128) {
            work32 = -128;
        }
        handler->Registers[grain->OpcodeData.MicroOp.Arg[0]] = work32;
    } else {
        work32 = gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] - 1;
        if (work32 < -128) {
            work32 = -128;
        }
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1] = work32;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_TEST_REGISTER(BlockSoundHandlerPtr handler,
                                        SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt8 source;

    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        source = handler->Registers[grain->OpcodeData.MicroOp.Arg[0]];
    } else {
        source = gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1];
    }

    switch (grain->OpcodeData.MicroOp.Arg[1]) {
    case 0:
        if (source >= grain->OpcodeData.MicroOp.Arg[2]) {
            handler->NextGrain++;
        }
        break;
    case 1:
        if (source != grain->OpcodeData.MicroOp.Arg[2]) {
            handler->NextGrain++;
        }
        break;
    case 2:
        if (source <= grain->OpcodeData.MicroOp.Arg[2]) {
            handler->NextGrain++;
        }
        break;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                 SFXGrain2Ptr grain) {
    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_GOTO_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                      SFXGrain2Ptr grain) {
    SInt32 work32;
    SInt32 i;
    SInt32 found;

    work32 = grain->OpcodeData.MicroOp.Arg[0];
    found = 0;

    for (i = 0; i < sfx->NumGrains && !found; i++) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x23 &&
            sfx->FirstGrain[i].OpcodeData.MicroOp.Arg[0] == work32) {
            handler->NextGrain = i - 1;
            found = 1;
        }
    }

    if (!found) {
        snd_ShowError(11, 0, 0, 0, 0);
        return -1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_GOTO_RANDOM_MARKER(BlockSoundHandlerPtr handler,
                                             SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 work32;
    SInt32 i;
    SInt32 found;

    work32 =
        grain->OpcodeData.MicroOp.Arg[1] - grain->OpcodeData.MicroOp.Arg[0] + 1;
    work32 = snd_RandomUInt16() % work32;
    work32 += grain->OpcodeData.MicroOp.Arg[0];
    found = 0;

    for (i = 0; i < sfx->NumGrains && !found; i++) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x23 &&
            sfx->FirstGrain[i].OpcodeData.MicroOp.Arg[0] == work32) {
            handler->NextGrain = i - 1;
            found = 1;
        }
    }

    if (!found) {
        snd_ShowError(12, work32, 0, 0, 0);
        return -1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_WAIT_FOR_ALL_VOICES(BlockSoundHandlerPtr handler,
                                              SFX2Ptr sfx, SFXGrain2Ptr grain) {
    if (handler->SH.Voices.core[0] || handler->SH.Voices.core[1]) {
        handler->NextGrain--;
        return 1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_PLAY_CYCLE(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                     SFXGrain2Ptr grain) {
    SInt32 work32;

    if (handler->sk_doing_skipping_play) {
        snd_ShowError(13, 0, 0, 0, 0);
        return -1;
    } else {
        work32 = grain->OpcodeData.MicroOp.Arg[2];
        grain->OpcodeData.MicroOp.Arg[2]++;

        if (grain->OpcodeData.MicroOp.Arg[2] ==
            grain->OpcodeData.MicroOp.Arg[0]) {
            grain->OpcodeData.MicroOp.Arg[2] = 0;
        }

        handler->NextGrain += grain->OpcodeData.MicroOp.Arg[1] * work32;
        handler->sk_grains_to_play = grain->OpcodeData.MicroOp.Arg[1] + 1;
        handler->sk_grains_to_skip =
            (grain->OpcodeData.MicroOp.Arg[0] - (work32 + 1)) *
            grain->OpcodeData.MicroOp.Arg[1];
        handler->sk_doing_skipping_play = 1;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_ADD_REGISTER(BlockSoundHandlerPtr handler,
                                       SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt32 work32;

    if (grain->OpcodeData.MicroOp.Arg[1] > -1) {
        work32 = handler->Registers[grain->OpcodeData.MicroOp.Arg[1]] +
                 grain->OpcodeData.MicroOp.Arg[0];
        if (work32 > 127) {
            work32 = 127;
        }
        if (work32 < -128) {
            work32 = -128;
        }
        handler->Registers[grain->OpcodeData.MicroOp.Arg[1]] = work32;
    } else {
        work32 = gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[1] - 1] +
                 grain->OpcodeData.MicroOp.Arg[0];
        if (work32 > 127) {
            work32 = 127;
        }
        if (work32 < -128) {
            work32 = -128;
        }
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[1] - 1] = work32;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_KEY_OFF_VOICES(BlockSoundHandlerPtr handler,
                                         SFX2Ptr sfx, SFXGrain2Ptr grain) {
    snd_KeyOffVoicesEx(&handler->SH.Voices, 0);
    handler->SH.Voices.core[0] = handler->SH.Voices.core[1] = 0;

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_KILL_VOICES(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                      SFXGrain2Ptr grain) {
    snd_SilenceVoicesEx(&handler->SH.Voices, 0);
    handler->SH.Voices.core[0] = handler->SH.Voices.core[1] = 0;

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_ON_STOP_MARKER(BlockSoundHandlerPtr handler,
                                         SFX2Ptr sfx, SFXGrain2Ptr grain) {
    handler->NextGrain = sfx->NumGrains - 1;

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_COPY_REGISTER(BlockSoundHandlerPtr handler,
                                        SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SInt8 work;

    if (grain->OpcodeData.MicroOp.Arg[0] > -1) {
        work = handler->Registers[grain->OpcodeData.MicroOp.Arg[0]];
    } else {
        work = gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[0] - 1];
    }

    if (grain->OpcodeData.MicroOp.Arg[1] > -1) {
        handler->Registers[grain->OpcodeData.MicroOp.Arg[1]] = work;
    } else {
        gBlockGlobalReg[-1 * grain->OpcodeData.MicroOp.Arg[1] - 1] = work;
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_PLUGIN_MESSAGE(BlockSoundHandlerPtr handler,
                                         SFX2Ptr sfx, SFXGrain2Ptr grain) {
    PluginParams *pp;

    pp = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);
    return snd_DoExternCall(pp->id, pp->index, handler->SH.OwnerID,
                            (SInt32)pp->data, (SInt32)handler, 0, 0);
}

SInt32 snd_SFX_GRAIN_TYPE_BRANCH(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                 SFXGrain2Ptr grain) {
    SInt32 index;
    SFXBlock2Ptr block;
    SFX2 *new_sfx;
    BlockSoundHandlerPtr weakest;
    PlaySoundParams *psp;

    weakest = NULL;
    psp = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);
    if (psp->sound_id > -1) {
        block = handler->block;
        index = psp->sound_id;
    } else {
        block = NULL;
        index = snd_FindSoundByName(block, psp->snd_name, &block);
        if (index < 0 || !block) {
            index = snd_FindSoundByName(handler->block, psp->snd_name, &block);
            if (index < 0) {
                if (!gPrefs_Silent) {
                    printf("989snd: Didn't find child sound "
                           "named -> %s\n",
                           psp->snd_name);
                }
                return -1;
            }
        }
    }

    snd_SilenceVoicesEx(&handler->SH.Voices, 0);
    handler->SH.Voices.core[0] = handler->SH.Voices.core[1] = 0;
    new_sfx = &block->FirstSound[index];
    if (new_sfx->Flags & 8) {
        if (!snd_CheckInstanceLimit(new_sfx, handler->App_Vol, 0, &weakest)) {
            return -1;
        }
        if (weakest && weakest != handler) {
            snd_StopHandlerPtr(&weakest->SH, 1, 0, 1);
        }
    }

    new_sfx->Flags |= 0x4000u;
    if (!new_sfx->NumGrains) {
        return -1;
    }

    handler->SH.Sound = (SoundPtr)new_sfx;
    handler->SH.Original_Vol = new_sfx->Vol;
    handler->SH.Original_Pan = new_sfx->Pan;
    handler->SH.Current_Vol = (handler->App_Vol * new_sfx->Vol) >> 10;
    handler->SH.VolGroup = new_sfx->VolGroup;
    handler->CountDown = new_sfx->FirstGrain->Delay;
    handler->NextGrain = -1;
    handler->sk_doing_skipping_play = 0;
    handler->sk_grains_to_play = 0;
    handler->sk_grains_to_skip = 0;
    handler->block = block;

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_STARTCHILDSOUND(BlockSoundHandlerPtr handler,
                                          SFX2Ptr sfx, SFXGrain2Ptr grain) {
    UInt32 sid;
    SInt32 reg;
    SInt32 vol;
    SInt32 pan;
    SInt32 index;
    SFXBlock2Ptr block;
    SndPlayParams params;
    PlaySoundParams *psp;

    reg = 0;
    if (handler->SH.flags & 0x20) {
        return 0;
    }

    psp = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);

    if (!handler->SH.Original_Vol) {
        return 0;
    }

    if (psp->vol > -1) {
        reg = psp->vol;
    } else {
        if (psp->vol >= -4) {
            reg = handler->Registers[-1 * psp->vol - 1];
        } else if (psp->vol == -5) {
            reg = snd_RandomUInt16() % 0x80;
        } else {
            reg = gBlockGlobalReg[-1 * psp->vol - 6];
        }
    }

    if (reg < 0) {
        reg = reg * -1;
    }
    if (reg > 127) {
        reg = 127;
    }

    vol = reg;

    if (psp->pan > -1) {
        pan = psp->pan;
    } else {
        if (psp->pan >= -4) {
            reg = handler->Registers[-1 * psp->pan - 1];
            if (reg < 0) {
                reg = reg * -1;
            }
            if (reg >= 128) {
                reg = 127;
            }
            pan = 360 * reg / 127;
        } else if (psp->pan == -5) {
            pan = snd_RandomUInt16() % 360;
        } else {
            reg = gBlockGlobalReg[-1 * psp->pan - 6];
            if (reg < 0) {
                reg = reg * -1;
            }
            if (reg >= 128) {
                reg = 127;
            }
            pan = 360 * reg / 127;
        }
    }

    if (psp->sound_id > -1) {
        block = handler->block;
        index = psp->sound_id;
    } else {
        block = NULL;
        index = snd_FindSoundByName(block, psp->snd_name, &block);
        if (index < 0 || !block) {
            index = snd_FindSoundByName(handler->block, psp->snd_name, &block);
            if (index < 0) {
                if (!gPrefs_Silent) {
                    printf("989snd: Didn't find child sound "
                           "named -> %s\n",
                           psp->snd_name);
                }

                return 0;
            }
        }
    }

    params.mask = 0x8000007C;
    params.vol = handler->App_Vol * handler->SH.Original_Vol / 127;
    params.pan = handler->App_Pan;
    params.pitch_mod = handler->App_PM;
    params.pitch_bend = handler->App_PB;
    params.reg[0] = handler->Registers[0];
    params.reg[1] = handler->Registers[1];
    params.reg[2] = handler->Registers[2];
    params.reg[3] = handler->Registers[3];

    sid = snd_PlaySFXWithStruct(block, index, vol, pan, &params);
    if (sid) {
        snd_AttachSoundToHandlersChildList(&handler->SH, sid);
    }

    return 0;
}

SInt32 snd_SFX_GRAIN_TYPE_STOPCHILDSOUND(BlockSoundHandlerPtr handler,
                                         SFX2Ptr sfx, SFXGrain2Ptr grain) {
    SFX2Ptr sts;
    SInt32 index;
    SFXBlock2Ptr block;
    PlaySoundParams *psp;

    psp = handler->block->GrainData + (grain->OpcodeData.Opcode & 0xffffff);
    snd_LockMasterTick(31);
    if (psp->sound_id < 0) {
        block = NULL;
        index = snd_FindSoundByName(block, psp->snd_name, &block);
        if (index < 0 || !block) {
            index = snd_FindSoundByName(handler->block, psp->snd_name, &block);
            if (index < 0) {
                if (!gPrefs_Silent) {
                    printf("989snd: Didn't find child sound "
                           "named -> %s\n",
                           psp->snd_name);
                }

                snd_UnlockMasterTick();
                return 0;
            }
        }
    } else {
        index = psp->sound_id;
        block = handler->block;
    }

    sts = &block->FirstSound[index];
    while (snd_KillChildrenWithSound(handler, sts))
        ;

    snd_UnlockMasterTick();

    return 0;
}

SInt32 snd_SFX_UNKNOWN_GRAIN_TYPE(BlockSoundHandlerPtr handler, SFX2Ptr sfx,
                                  SFXGrain2Ptr grain) {
    snd_ShowError(95, 0, 0, 0, 0);

    return -1;
}

SInt32 snd_DoGrain(BlockSoundHandlerPtr handler) {
    SFX2Ptr sfx;
    SFXGrain2Ptr grain;
    SInt32 stop_sound;
    SInt32 ret;

    stop_sound = 0;
    sfx = (SFX2Ptr)handler->SH.Sound;
    grain = &sfx->FirstGrain[handler->NextGrain];
    ret = gGrainHandler[grain->OpcodeData.MicroOp.Type](handler, sfx, grain);
    if (ret > -1) {
        if (handler->sk_doing_skipping_play) {
            handler->sk_grains_to_play--;
            if (!handler->sk_grains_to_play) {
                handler->NextGrain += handler->sk_grains_to_skip;
                handler->sk_doing_skipping_play = 0;
            }
        }

        handler->NextGrain++;

        if (handler->NextGrain > sfx->NumGrains - 1) {
            if (!handler->SH.Voices.core[0] && !handler->SH.Voices.core[1] &&
                (!handler->SH.first_child || handler->SH.parent)) {
                stop_sound = 1;
            } else {
                handler->NextGrain = -1;
            }
        } else {
            handler->CountDown =
                sfx->FirstGrain[handler->NextGrain].Delay + ret;
        }

        return stop_sound;
    } else {
        stop_sound = 1;
    }

    return stop_sound;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXVolPan);
#else
void snd_SetSFXVolPan(UInt32 handle, SInt32 vol, SInt32 pan, SInt32 cause) {
    BlockSoundHandlerPtr hand;
    SInt32 uses_voice;
    SInt32 own_the_allocator;
    SFX2Ptr sfx;
    SInt32 new_vol;
    SInt32 new_pan;
    GSoundHandlerPtr child_walk;
    SInt32 child_vol;
    SInt32 intr_state;
    SInt32 dis;
    SpuVolume spu_vol;
    SInt32 g_pan;

    uses_voice = -1;
    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }
    sfx = (SFX2Ptr)hand->SH.Sound;

    if (vol < 0) {
        hand->App_Vol = -1 * 1024 * vol / 127;
    } else if (vol != 0x7fffffff) {
        hand->App_Vol = vol;
    }

    if (pan == -1) {
        hand->App_Pan = sfx->Pan;
    } else if (pan != -2) {
        hand->App_Pan = pan;
    }

    new_vol = ((hand->App_Vol * hand->SH.Original_Vol) >> 10) + hand->LFO_Vol;
    if (new_vol > 127) {
        new_vol = 127;
    }
    if (new_vol < 0) {
        new_vol = 0;
    }

    new_pan = hand->App_Pan + hand->LFO_Pan;
    while (new_pan >= 360) {
        new_pan -= 360;
    }
    while (new_pan < 0) {
        new_pan += 360;
    }

    if (new_vol == hand->SH.Current_Vol && new_pan == hand->SH.Current_Pan) {
        return;
    }

    if (cause == 0 && hand->SH.first_child) {
        child_walk = hand->SH.first_child;
        while (child_walk) {
            child_vol = (hand->App_Vol * hand->SH.Original_Vol) / 127;
            snd_SetSFXVolPan(child_walk->OwnerID, child_vol, pan, cause);
            child_walk = child_walk->siblings;
        }
    }

    hand->SH.Current_Vol = new_vol;
    hand->SH.Current_Pan = new_pan;

    own_the_allocator = snd_LockVoiceAllocatorEx(1, 27);
    dis = CpuSuspendIntr(&intr_state);

    while ((uses_voice = snd_GetNextHandlerVoice(hand, uses_voice + 1)) != -1) {
        g_pan = 0;
        if (((SFX2Ptr)hand->SH.Sound)->Flags & 4) {
            g_pan = snd_CollapsePan(
                gChannelStatus[uses_voice].OwnerData.BlockData.g_pan,
                hand->App_Vol, sfx);
        } else {
            g_pan = gChannelStatus[uses_voice].OwnerData.BlockData.g_pan;
        }

        snd_MakeVolumes(127, 0, hand->SH.Current_Vol, hand->SH.Current_Pan,
                        gChannelStatus[uses_voice].OwnerData.BlockData.g_vol,
                        g_pan, &gChannelStatus[uses_voice].Volume);

        if ((hand->SH.flags & 2) == 0) {
            spu_vol.left = (UInt16)snd_AdjustVolToGroup(
                               gChannelStatus[uses_voice].Volume.left,
                               hand->SH.VolGroup) >>
                           1;
            spu_vol.right = (UInt16)snd_AdjustVolToGroup(
                                gChannelStatus[uses_voice].Volume.right,
                                hand->SH.VolGroup) >>
                            1;

            sceSdSetParam(uses_voice / 24 | SD_VOICE(uses_voice % 24) |
                              SD_VP_VOLL,
                          spu_vol.left);
            sceSdSetParam(uses_voice / 24 | SD_VOICE(uses_voice % 24) |
                              SD_VP_VOLR,
                          spu_vol.right);
        }

        gChannelStatus[uses_voice].Priority = snd_ScalePriorityForVolume(
            hand->App_Vol, gChannelStatus[uses_voice].Tone);
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    if (own_the_allocator) {
        snd_UnlockVoiceAllocator();
    }
}
#endif

void snd_SetSFXPitch(UInt32 handle, SInt32 pitch) {
    BlockSoundHandlerPtr hand;

    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }

    hand->Current_Note = pitch / 128;
    hand->Current_Fine = pitch % 128;

    snd_UpdateSFXPitch(hand);
}

void snd_SetSFXPitchbend(UInt32 handle, SInt16 bend) {
    BlockSoundHandlerPtr hand;
    GSoundHandlerPtr child_walk;

    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }

    if (hand->SH.first_child) {
        child_walk = hand->SH.first_child;
        while (child_walk) {
            snd_SetSFXPitchbend(child_walk->OwnerID, bend);
            child_walk = child_walk->siblings;
        }
    }

    hand->App_PB = bend;
    snd_UpdateSFXPitch(hand);
}

void snd_SetSFXPitchModifier(UInt32 handle, SInt16 mod) {
    BlockSoundHandlerPtr hand;
    GSoundHandlerPtr child_walk;

    if (!(hand = (BlockSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return;
    }

    if (hand->SH.first_child) {
        child_walk = hand->SH.first_child;
        while (child_walk) {
            snd_SetSFXPitchModifier(child_walk->OwnerID, mod);
            child_walk = child_walk->siblings;
        }
    }

    hand->App_PM = mod;
    snd_UpdateSFXPitch(hand);
}

void snd_UpdateSFXPitch(BlockSoundHandlerPtr hand) {
    SInt32 own_the_allocator;
    SInt32 uses_voice;
    SInt32 note;
    SInt32 fine;
    SInt32 new_pb;
    SInt32 intr_state;
    SInt32 dis;
    SInt32 clock;

    uses_voice = -1;
    hand->SH.Current_PM = hand->App_PM + hand->LFO_PM;
    new_pb = hand->App_PB + hand->LFO_PB;

    if (new_pb > 32767) {
        hand->Current_PB = 32767;
    } else if (new_pb < -32768) {
        hand->Current_PB = -32768;
    } else {
        hand->Current_PB = new_pb;
    }

    own_the_allocator = snd_LockVoiceAllocatorEx(1, 28);
    while ((uses_voice = snd_GetNextHandlerVoice(hand, uses_voice + 1)) != -1) {
        gChannelStatus[uses_voice].Current_PB = hand->Current_PB;
        gChannelStatus[uses_voice].Current_PM = hand->SH.Current_PM;

        if ((hand->SH.flags & 2) == 0) {
            dis = CpuSuspendIntr(&intr_state);

            if ((gChannelStatus[uses_voice].Tone->Flags & 8) == 0) {
                snd_PitchBendTone(gChannelStatus[uses_voice].Tone,
                                  hand->Current_PB, hand->SH.Current_PM,
                                  hand->Current_Note, hand->Current_Fine, &note,
                                  &fine);
                sceSdSetParam(
                    ((uses_voice / 24) | SD_VOICE(uses_voice % 24)) |
                        SD_VP_PITCH,
                    PS1Note2Pitch(gChannelStatus[uses_voice].Tone->CenterNote,
                                  gChannelStatus[uses_voice].Tone->CenterFine,
                                  note, fine));
            } else {
                clock = gChannelStatus[uses_voice].Tone->CenterNote +
                        32 * gChannelStatus[uses_voice].Current_PB / 0x7FFF;
                if (clock < 0)
                    clock = 0;
                if (clock >= 64)
                    clock = 63;
                sceSdSetCoreAttr((uses_voice / 24) | 8, clock);
            }

            if (!dis) {
                CpuResumeIntr(intr_state);
            }
        }
    }

    if (own_the_allocator) {
        snd_UnlockVoiceAllocator();
    }
}

SInt32 snd_DoesSFXLoop(SFXBlock2Ptr block, SInt32 sound) {
    return block->FirstSound[sound].Flags & 1;
}

void snd_SFXOwnerProc(SInt32 voice, UInt32 owner, SInt32 flag) {
    BlockSoundHandlerPtr snd;
    SInt32 core;
    SInt32 c_v;
    BlockSoundHandlerPtr parent;

    snd = (BlockSoundHandlerPtr)owner;
    core = voice / 24;
    c_v = voice % 24;

    snd->SH.Voices.core[core] &= ~(1 << c_v);
    if (!snd->SH.Voices.core[0] && !snd->SH.Voices.core[1] &&
        snd->NextGrain == -1 && (snd->SH.parent || !snd->SH.first_child)) {
        snd_LockMasterTick(16);
        parent = (BlockSoundHandler *)snd->SH.parent;
        snd_RemoveLFOsForHandler(snd);
        snd_DeactivateHandler(&snd->SH, 0);

        if (parent && !parent->SH.first_child && !parent->SH.Voices.core[0] &&
            !parent->SH.Voices.core[1] && parent->NextGrain == -1) {
            snd_RemoveLFOsForHandler(parent);
            snd_DeactivateHandler(&parent->SH, 0);
        }
        snd_UnlockMasterTick();
    }
}

SInt32 snd_CollapsePan(SInt32 g_pan, SInt32 app_vol, SFX2 *sfx) {
    SInt32 w_vol;
    SInt32 m_vol;
    SInt32 f_vol;

    w_vol = 0x400;
    m_vol = w_vol / 6;
    f_vol = w_vol - m_vol;

    if (app_vol < f_vol) {
        if (app_vol > m_vol) {
            if (g_pan > 180) {
                g_pan = 360 -
                        (((360 - g_pan) * (app_vol - m_vol)) / (f_vol - m_vol));
            } else {
                g_pan = ((app_vol - m_vol) * g_pan) / (f_vol - m_vol);
            }
        } else {
            g_pan = 0;
        }
    }

    return g_pan;
}

SInt8 snd_ScalePriorityForVolume(SInt32 vol, TonePtr tone) {
    SInt32 p;
    SInt32 depth;
    SInt32 var;

    p = tone->Priority;
    depth = (tone->Flags >> 1) & 3;
    if (!(tone->Flags & 6)) {
        return p;
    }

    switch (depth) {
    case 1:
        var = p / 5;
        break;
    case 2:
        var = p / 2;
        break;
    default:
        var = p;
        break;
    }

    if (!var) {
        return p;
    }

    if (vol > 950) {
        return p;
    }

    p -= var - (var * vol / 950);
    return p;
}

void snd_ResetAllHandlersForSound(SFX2 *sfx) {
    GSoundHandlerPtr walk;
    BlockSoundHandlerPtr hand;
    int x;

    snd_LockMasterTick(17);
    walk = gActiveSoundListHead;
    while (walk) {
        if (walk->Sound == (SoundPtr)sfx && !walk->parent) {
            hand = (BlockSoundHandlerPtr)walk;
            hand->SH.Original_Vol = sfx->Vol;
            hand->SH.Original_Pan = sfx->Pan;
            hand->SH.Current_Vol = -1;

            for (x = 0; x < 4; x++) {
                if (hand->lfo[x].type && hand->lfo[x].target == 1) {
                    snd_CalcLFODepth(hand->lfo);
                }
            }

            snd_SetSFXVolPan(hand->SH.OwnerID, 0x7fffffff, -2, 0);
            snd_UpdateSFXPitch(hand);
        }
        walk = walk->next;
    }
    snd_UnlockMasterTick();
}
