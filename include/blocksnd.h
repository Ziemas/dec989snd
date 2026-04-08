#ifndef BLOCKSND_H_
#define BLOCKSND_H_

#include "common.h"

UInt32 snd_PlaySFX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
UInt32 snd_PlaySFXWithStruct(SFXBlock2Ptr block, SInt32 sound, SInt16 sfx_vol, SInt16 sfx_pan, SndPlayParamsPtr params);
UInt32 snd_PlaySFX_EX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend,
                      SInt16 sfx_vol, SInt16 sfx_pan);
UInt32 snd_PlaySFXGrain(SFXBlock2Ptr block, SInt32 sindex, SInt32 gindex);
SInt8 snd_GetSFXGlobalReg(SInt32 which);
void snd_SetSFXGlobalReg(SInt32 which, SInt8 val);
SInt8 snd_GetSFXSoundReg(UInt32 handle, SInt32 which);
void snd_SetSFXSoundReg(UInt32 handle, SInt32 which, SInt8 val);
void snd_SetAllSFXSoundReg(UInt32 handle, SInt8 *vals);
SInt32 snd_ProcessBlockSoundTick(BlockSoundHandlerPtr handler);
BOOL snd_DoBlockSoundStop(BlockSoundHandlerPtr handler, SInt32 silence, BOOL vlimit_stop);
SInt32 snd_GetOnStopGrainIndex(BlockSoundHandlerPtr handler);
SInt32 snd_SFX_GRAIN_TYPE_NULL(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_TONE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_XREF_ID(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_XREF_NUM(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_LFO_SETTINGS(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_CONTROL_NULL(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_LOOP_START(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_LOOP_END(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_LOOP_CONTINUE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_STOP(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_GetSoundIndexInBlock(SFXBlock2Ptr block, SFX2 *sfx);
SInt32 snd_SFX_GRAIN_TYPE_RAND_PLAY(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_RAND_DELAY(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_RAND_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_ADD_PB(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_SET_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_SET_REGISTER_RAND(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_INC_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_DEC_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_TEST_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_GOTO_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_GOTO_RANDOM_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_WAIT_FOR_ALL_VOICES(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_PLAY_CYCLE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_ADD_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_KEY_OFF_VOICES(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_KILL_VOICES(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_ON_STOP_MARKER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_COPY_REGISTER(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_PLUGIN_MESSAGE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_BRANCH(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_STARTCHILDSOUND(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_GRAIN_TYPE_STOPCHILDSOUND(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_SFX_UNKNOWN_GRAIN_TYPE(BlockSoundHandlerPtr handler, SFX2Ptr sfx, SFXGrain2Ptr grain);
SInt32 snd_DoGrain(BlockSoundHandlerPtr handler);
void snd_SetSFXVolPan(UInt32 handle, SInt32 vol, SInt32 pan, SInt32 cause);
void snd_SetSFXPitch(UInt32 handle, SInt32 pitch);
void snd_SetSFXPitchbend(UInt32 handle, SInt16 bend);
void snd_SetSFXPitchModifier(UInt32 handle, SInt16 mod);
void snd_UpdateSFXPitch(BlockSoundHandlerPtr hand);
SInt32 snd_DoesSFXLoop(SFXBlock2Ptr block, SInt32 sound);
void snd_SFXOwnerProc(SInt32 voice, UInt32 owner, SInt32 flag);
SInt32 snd_CollapsePan(SInt32 g_pan, SInt32 app_vol, SFX2 *sfx);
SInt8 snd_ScalePriorityForVolume(SInt32 vol, TonePtr tone);
void snd_ResetAllHandlersForSound(SFX2 *sfx);

#endif // BLOCKSND_H_
