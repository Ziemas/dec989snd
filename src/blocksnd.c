#include "common.h"
#include "functions.h"

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

UInt32 snd_PlaySFX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend) {
    return snd_PlaySFX_EX(block, sound, vol, pan, pitch_mod, bend, -1, -1);
}

UInt32 snd_PlaySFXWithStruct(SFXBlock2Ptr block, SInt32 sound, SInt16 sfx_vol, SInt16 sfx_pan, SndPlayParamsPtr params) {
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

    if (stop_sound || 
        (!hand->CountDown && !hand->SH.Voices.core[0] 
            && !hand->SH.Voices.core[1] && !hand->SH.first_child)) {
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

UInt32 snd_PlaySFX_EX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend, SInt16 sfx_vol, SInt16 sfx_pan) {
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

    hand->SH.Sound = (MIDISound *)sfx;
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
    if (!hand->CountDown && !hand->SH.Voices.core[0] && !hand->SH.Voices.core[1] && !hand->SH.first_child) {
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
        if (!handler->SH.first_child && !handler->SH.Voices.core[0] && !handler->SH.Voices.core[1]) {
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

BOOL snd_DoBlockSoundStop(BlockSoundHandlerPtr handler, SInt32 silence, BOOL vlimit_stop) {
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

    if (!handler->SH.Voices.core[0] && !handler->SH.Voices.core[1] && !handler->SH.first_child) {
        snd_RemoveLFOsForHandler(handler);
        return 0;
    }

    return 1;
}

SInt32 snd_GetOnStopGrainIndex(BlockSoundHandlerPtr handler) {
	SInt32 i;
	SFX2Ptr sfx;

    sfx = (SFX2Ptr)handler->SH.Sound;
    for (i = 0; i < sfx->NumGrains ; i++) {
        if (sfx->FirstGrain[i].OpcodeData.MicroOp.Type == 0x2b) {
            if (i == sfx->NumGrains - 1) {
                return -1;
            }

            return i;
        }
    }

    return -1;
}

SInt32 snd_SFX_GRAIN_TYPE_NULL(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain) {
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_TONE);
//SInt32 snd_SFX_GRAIN_TYPE_TONE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain) {
//	SInt32 voice;
//	SInt32 g_pan;
//	SInt32 priority;
//	TonePtr tone;
//	SInt32 core;
//	SInt32 c_v;
//}

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_XREF_ID);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_XREF_NUM);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_LFO_SETTINGS);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_CONTROL_NULL);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_LOOP_START);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_LOOP_END);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_LOOP_CONTINUE);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_STOP);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_GetSoundIndexInBlock);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_RAND_PLAY);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_RAND_DELAY);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_RAND_PB);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_PB);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_ADD_PB);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_SET_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_SET_REGISTER_RAND);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_INC_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_DEC_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_TEST_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_MARKER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_GOTO_MARKER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_GOTO_RANDOM_MARKER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_WAIT_FOR_ALL_VOICES);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_PLAY_CYCLE);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_ADD_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_KEY_OFF_VOICES);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_KILL_VOICES);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_ON_STOP_MARKER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_COPY_REGISTER);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_PLUGIN_MESSAGE);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_BRANCH);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_STARTCHILDSOUND);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_STOPCHILDSOUND);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_UNKNOWN_GRAIN_TYPE);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_DoGrain);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXVolPan);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXPitch);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXPitchbend);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXPitchModifier);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_UpdateSFXPitch);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_DoesSFXLoop);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFXOwnerProc);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_CollapsePan);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_ScalePriorityForVolume);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_ResetAllHandlersForSound);
