#include "common.h"

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_PlaySFX);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_PlaySFXWithStruct);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_PlaySFX_EX);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_PlaySFXGrain);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_GetSFXGlobalReg);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXGlobalReg);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_GetSFXSoundReg);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetSFXSoundReg);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SetAllSFXSoundReg);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_ProcessBlockSoundTick);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_DoBlockSoundStop);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_GetOnStopGrainIndex);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_NULL);

INCLUDE_ASM("asm/nonmatchings/blocksnd", snd_SFX_GRAIN_TYPE_TONE);

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
