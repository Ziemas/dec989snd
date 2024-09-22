#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "common.h"
#include "libcdvd.h"

// *****************************************************************************
// FILE -- 989snd.c
// *****************************************************************************

BOOL snd_RegisterExternProcHandler(Extern989HandlerPtr hand);
BOOL snd_UnregisterExternProcHandler(Extern989HandlerPtr hand);
Extern989HandlerPtr snd_FindExternProcHandler(UInt32 id);
SInt32 snd_DoExternCall(UInt32 proc_id, SInt32 func_index, SInt32 arg1, SInt32 arg2, SInt32 arg3, SInt32 arg4, SInt32 arg5);
SInt32 snd_DoExternCallWithData(UInt32 proc_id, SInt32 func_index, SInt32 data_size, void *data_ptr);
void snd_CMD_SL_INIT(SInt8 *msg_data);
void snd_CMD_SL_CLOSE(SInt8 *msg_data);
void snd_CMD_SL_LOADBANK(SInt8 *msg_data);
void snd_CMD_SL_LOADBANKBYLOC(SInt8 *msg_data);
void snd_CMD_SL_BANKLOADFROMEE(SInt8 *msg_data);
void snd_CMD_SL_BANKLOADFROMIOP(SInt8 *msg_data);
void snd_CMD_SL_LOADMMD(SInt8 *msg_data);
void snd_CMD_SL_LOADMMDBYLOC(SInt8 *msg_data);
void snd_CMD_SL_UNLOADBANK_A(SInt8 *msg_data);
void snd_CMD_SL_UNLOADMIDI_A(SInt8 *msg_data);
void snd_CMD_SL_RESOLVEBANKS(SInt8 *msg_data);
void snd_CMD_SL_SETMASTERVOL_A(SInt8 *msg_data);
void snd_CMD_SL_GETMASTERVOL(SInt8 *msg_data);
void snd_CMD_SL_SETPLAYBACKMODE_A(SInt8 *msg_data);
void snd_CMD_SL_GETPLAYBACKMODE(SInt8 *msg_data);
void snd_CMD_SL_SETMIXERMODE_A(SInt8 *msg_data);
void snd_CMD_SL_SETREVERBMODE(SInt8 *msg_data);
void snd_CMD_SL_SETGROUPVOICERANGE(SInt8 *msg_data);
void snd_CMD_SL_SETREVERBTYPE(SInt8 *msg_data);
void snd_CMD_SL_SETREVERBDEPTH(SInt8 *msg_data);
void snd_CMD_SL_AUTOVERB(SInt8 *msg_data);
void snd_CMD_SL_PLAYSOUND(SInt8 *msg_data);
void snd_CMD_SL_PLAYSOUND_A(SInt8 *msg_data);
void snd_CMD_SL_PLAYSOUNDEX(SInt8 *msg_data);
void snd_CMD_SL_PAUSESOUND(SInt8 *msg_data);
void snd_CMD_SL_CONTINUESOUND(SInt8 *msg_data);
void snd_CMD_SL_STOPSOUND(SInt8 *msg_data);
void snd_CMD_SL_PAUSEALLSOUNDS(SInt8 *msg_data);
void snd_CMD_SL_CONTINUEALLSOUNDS(SInt8 *msg_data);
void snd_CMD_SL_STOPALLSOUNDS(SInt8 *msg_data);
void snd_CMD_SL_STOPALLSOUNDSINGROUP(SInt8 *msg_data);
void snd_CMD_SL_SOUNDISSTILLPLAYING(SInt8 *msg_data);
void snd_CMD_SL_ISSOUNDALOOPER(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDVOLPAN_A(SInt8 *msg_data);
void snd_CMD_SL_GETSOUNDORIGPITCH(SInt8 *msg_data);
void snd_CMD_SL_GETSOUNDCURRPITCH(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDPITCH_A(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDPITCHBEND_A(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDPITCHMOD_A(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDPARAMS(SInt8 *msg_data);
void snd_CMD_SL_AUTOVOL_A(SInt8 *msg_data);
void snd_CMD_SL_AUTOPAN_A(SInt8 *msg_data);
void snd_CMD_SL_AUTOPITCH_A(SInt8 *msg_data);
void snd_CMD_SL_AUTOPITCHBEND_A(SInt8 *msg_data);
void snd_CMD_SL_GETGLOBALEXCITE(SInt8 *msg_data);
void snd_CMD_SL_SETGLOBALEXCITE_A(SInt8 *msg_data);
void snd_CMD_SL_GETMIDIREGISTER(SInt8 *msg_data);
void snd_CMD_SL_SETMIDIREGISTER_A(SInt8 *msg_data);
void snd_CMD_SL_INITVAGSTREAMINGEX(SInt8 *msg_data);
void snd_CMD_SL_SETVAGSTREAMCHANNELRANGE(SInt8 *msg_data);
void snd_CMD_SL_SETVAGSTREAMSUBGROUPVOLPAN(SInt8 *msg_data);
void snd_CMD_SL_PLAYVAGSTREAMBYNAME(SInt8 *msg_data);
void snd_CMD_SL_PLAYVAGSTREAMBYLOC(SInt8 *msg_data);
void snd_CMD_SL_PAUSEVAGSTREAM(SInt8 *msg_data);
void snd_CMD_SL_CONTINUEVAGSTREAM(SInt8 *msg_data);
void snd_CMD_SL_STOPVAGSTREAM(SInt8 *msg_data);
void snd_CMD_SL_GETVAGSTREAMQUEUECOUNT(SInt8 *msg_data);
void snd_CMD_SL_GETVAGSTREAMLOC(SInt8 *msg_data);
void snd_CMD_SL_GETVAGSTREAMTIMEREMAINING(SInt8 *msg_data);
void snd_CMD_SL_GETVAGSTREAMTOTALTIME(SInt8 *msg_data);
void snd_CMD_SL_ISVAGSTREAMBUFFERED(SInt8 *msg_data);
void snd_CMD_SL_STOPALLVAGSTREAMS(SInt8 *msg_data);
void snd_CMD_SL_CLOSEVAGSTREAMING(SInt8 *msg_data);
void snd_CMD_SL_STREAMSAFECHECKCDIDLE(SInt8 *msg_data);
void snd_CMD_SL_STREAMSAFECDBREAK(SInt8 *msg_data);
void snd_CMD_SL_STREAMSAFECDREAD(SInt8 *msg_data);
void snd_CMD_SL_STREAMSAFECDSEARCHFILE(SInt8 *msg_data);
void snd_CMD_SL_ALLOCIOPRAM(SInt8 *msg_data);
void snd_CMD_SL_FREEIOPRAM(SInt8 *msg_data);
void snd_CMD_SL_INITMOVIESOUND(SInt8 *msg_data);
void snd_CMD_SL_CLOSEMOVIESOUND(SInt8 *msg_data);
void snd_CMD_SL_RESETMOVIESOUND(SInt8 *msg_data);
void snd_CMD_SL_STARTMOVIESOUND(SInt8 *msg_data);
void snd_CMD_SL_PAUSEMOVIESOUND(SInt8 *msg_data);
void snd_CMD_SL_SETMOVIESOUNDVOLPAN(SInt8 *msg_data);
void snd_CMD_SL_UPDATEMOVIEADPCM(SInt8 *msg_data);
void snd_CMD_SL_GETMOVIENAX(SInt8 *msg_data);
void snd_CMD_SL_GETTRANSSTATUS(SInt8 *msg_data);
void snd_CMD_SL_GETSTICK(SInt8 *msg_data);
void snd_CMD_SL_GETVOICEALLOC(SInt8 *msg_data);
void snd_CMD_SL_LOCKVALLOC(SInt8 *msg_data);
void snd_CMD_SL_EXTERNVALLOC(SInt8 *msg_data);
void snd_CMD_SL_EXTERNVFREE(SInt8 *msg_data);
void snd_CMD_SL_UNLOCKVALLOC(SInt8 *msg_data);
void snd_CMD_SL_SRAMMALLOC(SInt8 *msg_data);
void snd_CMD_SL_SRAMMARKUSED(SInt8 *msg_data);
void snd_CMD_SL_SRAMFREE(SInt8 *msg_data);
void snd_CMD_SL_SRAMGETFREE(SInt8 *msg_data);
void snd_CMD_SL_SRAMMAXFREE(SInt8 *msg_data);
void snd_CMD_SL_EXTERNCALL(SInt8 *msg_data);
void snd_CMD_SL_EXTERNCALLWITHDATA(SInt8 *msg_data);
void snd_CMD_SL_SETREVERBEX(SInt8 *msg_data);
void snd_CMD_SL_PREALLOCREVERBWORKAREA(SInt8 *msg_data);
void snd_CMD_SL_GETLASTLOADERROR(SInt8 *msg_data);
void snd_CMD_SL_SETPANMODE(SInt8 *msg_data);
void snd_CMD_SL_PLAYSOUNDBYNAME(SInt8 *msg_data);
void snd_CMD_SL_PLAYSOUNDBYNAME_A(SInt8 *msg_data);
void snd_CMD_SL_GETSOUNDUD(SInt8 *msg_data);
void snd_CMD_SL_GETSOUNDREG(SInt8 *msg_data);
void snd_CMD_SL_SETSOUNDREG(SInt8 *msg_data);
void snd_CMD_SL_GETSFXGLOBALREG(SInt8 *msg_data);
void snd_CMD_SL_SETSFXGLOBALREG(SInt8 *msg_data);
void snd_CMD_SL_SETALLSOUNDREG(SInt8 *msg_data);
void snd_CMD_SL_SETMASTERVOLUMEDUCKER(SInt8 *msg_data);
void snd_CMD_SL_CDSTATUS(SInt8 *msg_data);
void snd_CMD_SL_COMMAND_BATCH(SInt8 *msg_data);
SInt32 snd_StartEEMessaging();
SInt32 snd_StartEELoaderMessaging();
SInt32 start(SInt32 argc, SInt8 **argv);
void snd_ParseCommandLineArg(char *arg);
SInt32 snd_GetVal(char *st);
void snd_DumpVersionAndInfo();
void snd_RegisterErrorDisplayFunc(SndErrorDisplayFunc func);
void snd_ShowError(SInt32 num, UInt32 a1, UInt32 a2, UInt32 a3, UInt32 a4);
void snd_EEDMADone(SInt32 *sema_id_ptr);
void snd_Install989Monitor(Extern989MonitorInfo *mon);

// *****************************************************************************
// FILE -- LFO.c
// *****************************************************************************

SInt32 snd_LFO_TYPE_OFF(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SINE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SQUARE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_TRIANGLE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SAW(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_RAND(LFOTracker *tracker, int step);
SInt32 snd_GetLFO(LFOTracker *tracker, SInt32 step_mult);
void snd_InitLFO(LFOTracker *tracker);
void snd_RemoveLFOsForHandler(BlockSoundHandlerPtr handler);
void snd_RemoveLFO(LFOTracker *lfo);
BOOL snd_InsertLFO(LFOTracker *lfo);
UInt32 snd_DoOneLFO(LFOTracker *tracker);
void snd_HandleLFOs();
void snd_CalcLFODepth(LFOTracker *lfo);

// *****************************************************************************
// FILE -- ame.c
// *****************************************************************************

UInt32 snd_PlayAMESound(MIDISoundPtr sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
SInt32 snd_StartAMESegment(AMEHandlerPtr parent, SInt32 segment);
void snd_StopAMESegment(MIDIHandlerPtr handler);
AMEStreamHeaderPtr snd_FindAMEStream(MIDIHandlerPtr stream1, SInt32 index);
void snd_SetAMESoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan);
void snd_SetAMESoundPitchModifier(UInt32 handle, SInt16 mod);
SInt8* snd_AMEFunction(MIDIHandlerPtr stream, UInt8 *ame_header);
void snd_SetMIDIRegister(UInt32 handle, SInt32 reg, SInt16 value);
void snd_SetAllMIDIRegister(UInt32 handle, SInt8 *vals);
SInt32 snd_GetMIDIRegister(UInt32 handle, SInt32 reg);

// *****************************************************************************
// FILE -- autopan.c
// *****************************************************************************

void snd_AutoPan(UInt32 handle, SInt32 pan, SInt32 dir, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPanHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

// *****************************************************************************
// FILE -- autopb.c
// *****************************************************************************

void snd_AutoPitchBend(UInt32 handle, SInt32 pb, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPitchBendHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

// *****************************************************************************
// FILE -- autoptch.c
// *****************************************************************************

void snd_AutoPitch(UInt32 handle, SInt32 pitch, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPitchHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

// *****************************************************************************
// FILE -- autoverb.c
// *****************************************************************************

void snd_AutoReverb(SInt32 core, SInt32 depth, SInt32 delta_time, SInt32 channel_flags);
void snd_AutoReverb_L(SInt32 core, SInt32 depth, SInt32 delta_time, SInt32 channel_flags);
void snd_KillAutoVerbForCore(SInt32 core);
void snd_AutoVerbHandler();

// *****************************************************************************
// FILE -- autovol.c
// *****************************************************************************

void snd_AutoVol(UInt32 handle, SInt32 vol, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoVolHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

// *****************************************************************************
// FILE -- blocksnd.c
// *****************************************************************************

UInt32 snd_PlaySFX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
UInt32 snd_PlaySFXWithStruct(SFXBlock2Ptr block, SInt32 sound, SInt16 sfx_vol, SInt16 sfx_pan, SndPlayParamsPtr params);
UInt32 snd_PlaySFX_EX(SFXBlock2Ptr block, SInt32 sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend, SInt16 sfx_vol, SInt16 sfx_pan);
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

// *****************************************************************************
// FILE -- extern.c
// *****************************************************************************

SInt32 InitResidentModule();
UInt32 snd_PlaySoundByNameVolPanPMPB_E(SoundBankPtr block, char *block_name, char *sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, SInt16 bend);
SInt32 snd_GetSoundUserData_E(SoundBankPtr bank, char *bank_name, int sound_index, char *sound_name, int *destination);
void snd_SetMIDIRegister_E(UInt32 handle, SInt32 reg, SInt16 value);
SInt32 snd_GetMIDIRegister_E(UInt32 handle, SInt32 reg);
void snd_SetGlobalExcite_E(SInt32 value);
SInt32 snd_GetGlobalExcite_E();
UInt32 snd_SRAMMalloc_E(UInt32 size);
UInt32 snd_SRAMMarkUsed_E(UInt32 loc, UInt32 size);
void snd_SRAMFree_E(UInt32 loc, UInt32 size);
SInt32 snd_GetDopplerPitchMod_E(SInt32 approaching_mph);
void snd_MIDINoteOn_E(MIDIHandlerPtr stream);
void snd_MIDINoteOff_E(MIDIHandlerPtr stream);
void snd_PitchBend_E(MIDIHandlerPtr stream);
SInt16 snd_MIDITo360Pan_E(SInt8 pan);
void snd_ReleaseDamper_E(MIDIHandlerPtr stream);
void snd_ResetControllers_E(MIDIHandlerPtr stream);
SInt32 snd_SRAMGetFreeMem_E();
SInt32 snd_SRAMMaxFreeBlockSize_E();

// *****************************************************************************
// FILE -- init.c
// *****************************************************************************

SInt32 snd_SystemRunning();
SInt32 snd_StartSoundSystemEx(UInt32 flags);
SInt32 snd_InstallSoundTick();
void snd_RemoveSoundTick();
SInt32 snd_StopSoundSystem();
SInt32 snd_GetTickRate();
SInt32 snd_TimerLLIRQLoop();

// *****************************************************************************
// FILE -- intr_code.c
// *****************************************************************************

SInt32 snd_TimerLLTick(timercommon *common);
int snd_TransCallback(SInt32 ch, void *data);
void snd_DoneVAGReadCB(SInt32 code);
int snd_VAGDMADone0(int ch, void *data);

// *****************************************************************************
// FILE -- loader.c
// *****************************************************************************

void snd_InitLoader();
void snd_ShutdownLoader();
void snd_RegisterIOPMemAllocator(ExternSndIOPAlloc AllocProc, ExternSndIOPFree FreeProc);
BOOL snd_OpenDataSourceByName(char *name, SInt32 offset);
BOOL snd_OpenDataSourceByLoc(SInt32 sector, SInt32 offset);
BOOL snd_OpenDataSourceFromEE(UInt32 ee_loc);
void snd_CloseDataSource();
SInt32 snd_SeekDataSource(SInt32 where, SInt32 from_where);
void snd_SetDataSourceMark();
SInt32 snd_GetLastLoadError();
SInt32 snd_ReadBytesFromEE(UInt32 ee_loc, void *iop_loc, SInt32 bytes);
SInt32 snd_ReadBytes(void *buffer, SInt32 count);
SoundBankPtr snd_BankLoadEx(SInt8 *name, SInt32 offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
void snd_IterateBlockListToMonitor();
SoundBankPtr snd_ParseIOPBank(SInt8 *iop_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_ReadBank(UInt32 spu_mem_loc, UInt32 spu_mem_size);
SInt32 snd_FileRead(SInt32 sect_loc, SInt32 offset, SInt32 size, SInt8 *buffer);
SoundBankPtr snd_BankLoadByLocEx(SInt32 sect_loc, SInt32 file_offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_BankLoadFromEEEx(UInt32 ee_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
UInt32 snd_GetSRAMUsedByBank(SoundBankPtr bank);
SoundBankPtr snd_BankLoadFromIOPEx(void *iop_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
void snd_UnloadBank(SoundBankPtr bank);
BOOL snd_BankLoaded(SoundBankPtr bank);
void snd_UnloadBlock(SFXBlock2Ptr block);
BOOL snd_BlockLoaded(SFXBlock2Ptr block);
MultiMIDIBlockHeaderPtr snd_MMDLoad(SInt8 *name, SInt32 offset);
MultiMIDIBlockHeaderPtr snd_MMDLoadFromIOPLoc(SInt8 *iop_loc);
MultiMIDIBlockHeaderPtr snd_MMDLoadFromDataSource();
MultiMIDIBlockHeaderPtr snd_MMDLoadByLoc(SInt32 sect_loc, SInt32 file_offset);
void snd_UnloadMMD(MultiMIDIBlockHeaderPtr mmd);
void snd_UnloadAllMMD();
SInt32 snd_BankTransfer(SoundBankPtr bank, SInt8 *data, UInt32 data_size, SInt32 offset, SInt32 state, UInt32 spu_mem_loc, UInt32 spu_mem_size, SpuTransferCallbackProc callback);
void snd_ClearTransSema();
SInt32 snd_IsCurrentTransferComplete(BOOL wait);
SInt32 snd_EndBankTransfer(SoundBankPtr bank);
void snd_RemoveBank(SoundBankPtr bank);
void snd_RemoveBlock(SFXBlock2Ptr block);
void snd_UnloadAllBanks();
void snd_ResolveBankXREFS();
SoundBankPtr snd_FindBankByID(UInt32 id);
SFXBlock2Ptr snd_FindBlockByName(UInt32 *name);
SFXBlock2Ptr snd_FindBlockByID(UInt32 id);
SoundBankPtr snd_FindBankByNum(SInt8 num);
SFXBlock2Ptr snd_FindBlockByNum(SInt8 num);
SInt32 snd_RegisterMIDI(MIDIBlockHeaderPtr midi);
void snd_UnregisterMIDI(MIDIBlockHeaderPtr midi);
MIDIBlockHeaderPtr snd_FindMIDIBlock(UInt32 id);
void snd_InsertMIDIBlockInList(MIDIBlockHeaderPtr midi);
void snd_MIDIStreamGone(MIDIBlockHeaderPtr midi);
void snd_SetCDSifReg(SInt32 reg8, SInt32 reg9);
void* snd_IOPMemAlloc(SInt32 size, SInt32 use, SInt32 *act_size);
void snd_IOPMemFree(void *mem);
SInt32 snd_GetFreeSPUDMA();
void snd_FreeSPUDMA(SInt32 ch);
void snd_WaitDMADone();

// *****************************************************************************
// FILE -- midi.c
// *****************************************************************************

UInt32 snd_PlayMIDISound(MIDISoundPtr sound, SInt32 vol, SInt32 pan, SInt16 pitch_mod, int bend);
void snd_SetupMIDIStreamHandler(MIDIHandlerPtr hand, MIDIBlockHeaderPtr stream);
MIDIBlockHeaderPtr snd_GetRealMIDIDataStream(MIDISoundPtr sound);
void snd_MuteMIDIChannel(MIDIHandlerPtr stream, SInt32 channel);
void snd_UnmuteMIDIChannel(MIDIHandlerPtr stream, SInt32 channel);
SInt32 snd_ProcessMIDITick(MIDIHandlerPtr stream);
UInt32 ReadVarLen(UInt8 *buffer, SInt32 *used_bytes);
void snd_MIDINoteOn(MIDIHandlerPtr stream);
void snd_MIDINoteOff(MIDIHandlerPtr stream);
void snd_ReleaseDamper(MIDIHandlerPtr stream);
void snd_ResetControllers(MIDIHandlerPtr stream);
void snd_PitchBend(MIDIHandlerPtr stream);
void snd_SetMIDISoundVolumePan(UInt32 handle, SInt32 vol, SInt32 pan);
void snd_SetMIDIHandlerVolumePan(MIDIHandlerPtr stream, SInt32 vol, SInt32 pan);
void snd_SetMIDISoundPitchModifier(UInt32 handle, SInt16 mod);
void snd_SetMIDIHandlerPitchModifier(MIDIHandlerPtr stream, SInt16 mod);
void snd_MIDISoundOwnerProc(SInt32 voice, UInt32 owner, SInt32 flag);

// *****************************************************************************
// FILE -- moviesnd.c
// *****************************************************************************

void* snd_InitMovieSoundEx(SInt32 buff_size, SInt32 spu_buff_size, SInt32 vol, SInt32 pan, SInt32 vol_group, SInt32 type);
void snd_UpdateMovieADPCM(SInt32 data_size, UInt32 offset);
UInt32 snd_GetMovieNAX();
void snd_ClearMovieSoundBuffer();
void snd_CloseMovieSound();
void snd_ResetMovieSound();
void snd_StartMovieSoundEx(void *buff, SInt32 buff_size, SInt32 offset, UInt32 sr, SInt32 ch);
void snd_AdjustMovieVolDueToMasterChange(SInt32 which);
void snd_SetMovieSoundVolPan(SInt32 vol, SInt32 pan);
UInt32 snd_GetMovieBufferPos();
SInt32 snd_PauseMovieSound();

// *****************************************************************************
// FILE -- my_rand.c
// *****************************************************************************

void snd_RandInit(SInt16 seed);
UInt16 snd_RandomUInt16();
//static UInt16 myrand();
//static void mysrand(UInt16 newseed);

// *****************************************************************************
// FILE -- pantable.c
// *****************************************************************************

void snd_SetPanTable(VolPair *table);
void snd_SetPanMode(SInt32 mode);

// *****************************************************************************
// FILE -- playsnd.c
// *****************************************************************************

SInt32 snd_GetSoundUserData(SoundBankPtr bank, char *bank_name, int sound_index, char *sound_name, int *destination);
UInt32 snd_PlaySoundByNameVolPanPMPB(SoundBankPtr block, char *block_name, char *sound, SInt32 vol, SInt32 pan, int pitch_mod, int bend);
UInt32 snd_PlaySoundEx(SndPlayParamsPtr params);
UInt32 snd_PlaySoundVolPanPMPB(SoundBankPtr bank, SInt32 sound, SInt32 vol, SInt32 pan, int pitch_mod, int bend);
void snd_StopSound(UInt32 handle);
UInt32 snd_SoundIsStillPlaying(UInt32 handle);
SInt32 snd_IsSoundALooper(SoundBankPtr bank, SInt32 sound);
void snd_SetSoundVolPan(UInt32 handle, SInt32 vol, SInt32 pan);
UInt32 snd_SetSoundParams(UInt32 handle, UInt32 mask, SInt32 vol, SInt32 pan, int pitch_mod, int bend);
SInt32 snd_GetSoundOriginalPitch(SoundBankPtr bank, SInt32 sound);
SInt32 snd_GetSoundCurrentPitch(UInt32 handle);
SInt32 snd_GetSoundPitchBend(UInt32 handle);
void snd_SetSoundPitch(UInt32 handle, SInt32 pitch);
void snd_SetSoundPitchBend(UInt32 handle, SInt16 bend);
void snd_SetSoundPitchModifier(UInt32 handle, SInt16 mod);
SInt8 snd_GetSoundReg(UInt32 handle, SInt32 which);
void snd_SetSoundReg(UInt32 handle, SInt32 which, SInt8 val);
void snd_SetAllSoundReg(UInt32 handle, SInt8 *vals);
SInt32 snd_GetSoundPitchModifier(UInt32 handle);
void snd_PauseSound(UInt32 handle);
void snd_ContinueSound(UInt32 handle);
void snd_StopAllSoundsInBank(SoundBankPtr bank, SInt32 silence);
void snd_SetGlobalExcite(SInt32 value);
SInt32 snd_GetGlobalExcite();
SoundBankPtr snd_FindBankByName(char *name);
SInt32 snd_FindSoundByName(SFXBlock2Ptr block, char *name, SFXBlock2Ptr *found_block);
SInt32 snd_CalcSoundNameHash(char *name);
SInt32 snd_CollectTones(SoundBankPtr bank, SInt32 prog, SInt32 key, TonePtr *tones);
void snd_DEBUG_SoloSound(SoundBankPtr bank, SInt32 sound);
SInt32 snd_DEBUG_CheckSolo(SoundBankPtr bank, SInt32 sound);
void snd_DEBUG_ClearSolo();

// *****************************************************************************
// FILE -- reverb.c
// *****************************************************************************

void snd_SetReverbMode(SInt32 reverb_mode);
void snd_PreAllocReverbWorkArea(UInt32 core, SInt32 type);
void snd_PreAllocReverbWorkArea_L(SInt32 core, SInt32 type);
void snd_SetReverbEx(UInt32 core, SInt32 type, SInt32 depth, SInt32 delay, SInt32 feedback);
void snd_SetReverbType(SInt32 core, SInt32 type);
SInt32 snd_SetReverbType_L(SInt32 core, SInt32 type, SInt32 delay, SInt32 feedback);
void snd_SetReverbDepth(SInt32 core, SInt32 left, SInt32 right);
void snd_SetReverbDepth_L(SInt32 core, SInt32 left, SInt32 right);
void snd_CheckVerb();

// *****************************************************************************
// FILE -- sndhand.c
// *****************************************************************************

void snd_InitHandlers(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size);
void snd_InitSoundHandlers();
GSoundHandlerPtr snd_FindFreeHandler(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size);
BOOL snd_CheckInstanceLimit(SFX2 *sfx, SInt32 vol, BOOL parent, BlockSoundHandlerPtr *weakest_holder);
BlockSoundHandlerPtr snd_GetFreeBlockSoundHandler(SFX2 *sfx, SInt32 vol, BOOL parent);
MIDIHandlerPtr snd_GetFreeMIDIHandler();
AMEHandlerPtr snd_GetFreeAMEHandler();
void snd_ActivateHandler(GSoundHandlerPtr snd);
void snd_DeactivateHandler(GSoundHandlerPtr snd, SInt32 and_child);
void snd_AttachSoundToHandlersChildList(GSoundHandlerPtr handler, UInt32 id);
GSoundHandlerPtr snd_CheckHandlerStillActive(UInt32 handle);
void snd_StopHandler(UInt32 handle, SInt32 and_child, SInt32 silence, BOOL vlimit_stop);
void snd_PauseHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child);
void snd_ContinueHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child);
void snd_PauseAllSoundsInGroup(UInt32 groups);
void snd_StopAllSoundsInGroup(UInt32 groups);
void snd_ContinueAllSoundsInGroup(UInt32 groups);
void snd_StopAllSounds();
void snd_StopHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child, SInt32 silence, BOOL vlimit_stop);
void snd_StopAllHandlersForSound(SoundPtr snd, SInt32 silence, BOOL vlimit_stop);
BOOL snd_KillChildrenWithSound(GSoundHandlerPtr handler, void *sfx);
SInt32 snd_GetNextHandlerVoice(GSoundHandlerPtr snd, SInt32 start_v);
void snd_UpdateHandlers();
SInt32 snd_UpdateEffect(EffectChainPtr effect, GSoundHandlerPtr owner);
void snd_FreeEffectChain(EffectChainPtr effect);
void snd_RemoveEffect(GSoundHandlerPtr handler, SInt32 type_flag);
EffectChainPtr snd_FindEffect(GSoundHandlerPtr handler, SInt32 type_flag);
BasicEffectPtr snd_GetFreeBasicEffect();
void snd_AddEffectToHandler(GSoundHandlerPtr handler, EffectChainPtr effect);
void snd_RemoveEffectFromHandler(GSoundHandlerPtr handler, EffectChainPtr effect);

// *****************************************************************************
// FILE -- sram.c
// *****************************************************************************

void snd_SRAMInitMalloc();
UInt32 snd_SRAMMalloc(UInt32 size);
UInt32 snd_SRAMMallocRev(UInt32 size);
void snd_SRAMFree(UInt32 loc, UInt32 size);
UInt32 snd_SRAMMarkUsed(UInt32 loc, UInt32 size);
void snd_SRAMRemoveNode(sSRAMNodePtr node);
void snd_SRAMAddNode(sSRAMNodePtr node);
sSRAMNodePtr snd_SRAMSearch(sSRAMNodePtr root, UInt32 size);
SInt32 snd_SRAMGetFreeMem();
SInt32 snd_SRAMMaxFreeBlockSize();
void snd_SRAMDumpFreeBlocks();
SInt32 snd_GetFreeBlockSize(SInt32 start, UInt32 *loc);

// *****************************************************************************
// FILE -- stick.c
// *****************************************************************************

void snd_LockMasterTick(UInt32 ownerID);
void snd_UnlockMasterTick();
SInt32 snd_SoundTickHandler();
UInt32 snd_GetTick();
void snd_SetTick(UInt32 val);

// *****************************************************************************
// FILE -- stream.c
// *****************************************************************************

BOOL snd_InitVAGStreamingEx(SInt32 num_channels, SInt32 buffer_size, UInt32 read_mode, BOOL enable_streamsafe_from_ee);
void snd_SetVAGStreamChannelRange(SInt32 vol_group, SInt32 ch_min, SInt32 ch_max);
SInt32 snd_AllocateStreamResources(VAGStreamHeader **stream1, VAGStreamHeader **stream2, SInt32 stereo, SInt32 get_voices, SInt32 vol_group);
UInt32 snd_PlayVAGStreamByNameEx(char *name1, char *name2, UInt32 offset1, UInt32 offset2, SInt32 vol, SInt32 pan, SInt32 vol_group, UInt32 queue, SInt32 sub_group, UInt32 flags);
UInt32 snd_PlayVAGStreamByLocEx(SInt32 loc1, SInt32 loc2, SInt32 offset1, SInt32 offset2, SInt32 vol, SInt32 pan, SInt32 vol_group, UInt32 queue, SInt32 sub_group, UInt32 flags);
SInt32 snd_GetVAGStreamQueueCount(UInt32 handle);
void snd_TryPreBufferQueElement(VAGStreamHandlerPtr handler, UInt32 load_flags);
void snd_SetupVAGStream(VAGStreamHandlerPtr handler, VAGStreamHeader *stream, SInt32 loc, SInt32 offset, SInt32 part_of_queue, UInt32 flags);
void snd_StopAllStreams();
void snd_PauseVAGStream(UInt32 handle);
void snd_PauseAllStreams();
void snd_ContinueVAGStream(UInt32 handle);
void snd_StopVAGStream(UInt32 handle);
BOOL snd_StreamHandlerIsBusyWithLoad(VAGStreamHandlerPtr hand);
BOOL snd_StreamHandlerIsBusyWithDMA(VAGStreamHandlerPtr hand);
void snd_KillVAGStreamEx(UInt32 handle, SInt32 from_where);
void snd_ProcessVAGStreamTick(VAGStreamHandlerPtr hand);
SInt32 snd_CheckVAGStreamProgress(VAGStreamHeader *stream, SInt32 *dma_needed);
void snd_FixVAGStreamSamplesPlayed(VAGStreamHeader *stream, UInt32 addr);
UInt32 snd_GetVAGStreamLoc(UInt32 handle);
UInt32 snd_GetVAGStreamTimeRemaining(UInt32 handle);
UInt32 snd_GetVAGStreamTotalTime(UInt32 handle);
SInt32 snd_IsVAGStreamBuffered(UInt32 handle);
void snd_VAGStreamDataLoadThread();
SInt32 snd_StreamSafeCdRead(UInt32 lbn, UInt32 sectors, void *buf);
SInt32 snd_StreamSafeCdBreak();
SInt32 snd_StreamSafeCdSearchFile(sceCdlFILE *fp, char *name);
SInt32 snd_StreamSafeCdReadEEm(UInt32 lbn, UInt32 sectors, void *buf);
SInt32 snd_StreamSafeCdSync(SInt32 mode);
SInt32 snd_StreamSafeCdGetError();
void snd_KickDataRead();
void snd_KickVAGRead(SInt32 offset);
SInt32 snd_GetStreamDataFromIOPMemory(SInt32 offset, SInt32 readbytes, UInt32 readsectors);
SInt32 snd_GetStreamDataFromEEMemory(SInt32 offset, SInt32 readbytes, UInt32 readsectors);
SInt32 snd_GetStreamDataFromMemoryStream(SInt32 offset, SInt32 readbytes, UInt32 readsectors, UInt32 flags);
SInt32 snd_GetStreamDataFromStdio(SInt32 offset, SInt32 readbytes, UInt32 readsectors);
SInt32 snd_GetStreamDataFromCD(SInt32 offset, SInt32 readbytes, UInt32 readsectors);
void snd_VAGStreamLoadDoneThread();
void snd_RestartInterleavedStream(VAGStreamHandler *handler);
void snd_SignalDataReadDone();
void snd_VAGStreamDMAThread();
void snd_KickVAGDMA(SInt32 ch);
void snd_VAGStreamDMADoneThread();
void snd_RepairIOPMemoryBuffer(VAGBuffer *buff);
void snd_VAGDMADone(SInt32 ch);
void snd_StartVAGStreamSounding(VAGStreamHeader *stream, SInt32 do_sync_list);
SInt32 snd_MakeVAGLoop(SInt8 *vag, SInt32 size, SInt32 final, SInt32 sl);
VAGStreamHeader* snd_GetFreeVAGStream(SInt32 vol_group);
SInt32 snd_AddVAGStreamDMABuffer(VAGBuffer *buffer);
BOOL snd_AddVAGStreamLoadBuffer(VAGBuffer *buffer);
void snd_RemoveVAGStreamLoadBuffer(VAGBuffer *buffer);
void snd_RemoveVAGStreamDMABuffer(VAGBuffer *buffer);
void snd_AddQueEntryToHandler(VAGStreamHandlerPtr handler, VAGStreamQueEntry *que);
VAGStreamQueEntry* snd_GetFreeQueSlot();
void snd_FreeQueChain(VAGStreamQueEntry *que, BOOL is_stdio);
void snd_CleanUpThread(SInt32 *tid);
void snd_CleanUpSema(SInt32 *sema);
void snd_FreeVAGStreamResources();
void snd_CloseVAGStreaming();
SInt32 snd_StreamSafeCheckCDIdle(SInt32 block);
VAGStreamHandlerPtr snd_GetFreeVAGStreamHandler();
void snd_AddStreamToHandler(VAGStreamHandlerPtr handler, VAGStreamHeader *stream);
void snd_AddStreamToHandlerQueue(VAGStreamHandlerPtr handler, VAGStreamHeader *stream);
SInt32 snd_GetNumStreamChannels(VAGStreamHeader *stream);
SInt32 snd_FixVol(SInt32 orig_vol, SInt32 new_vol);
SInt32 snd_FixPan(SInt32 orig_pan, SInt32 new_pan);
void snd_SetVAGStreamSubGroupVolPan(UInt32 handle, SInt32 group, SInt32 vol, SInt32 pan);
void snd_SetVAGStreamVolPan(UInt32 handle, SInt32 vol, SInt32 pan);
VAGStreamHeaderPtr snd_GetFirstOfAdjacentStreamHeaders(VAGStreamHeaderPtr consider, SInt32 num_channels);
SInt32 snd_GetFreeStreamLoop(VAGStreamHeaderPtr start, VAGStreamHeaderPtr consider, SInt32 num_channels, VAGStreamHeaderPtr *last_tested, SInt32 vol_group);
BOOL snd_StreamIsFreeForVolGroup(VAGStreamHeaderPtr stream, SInt32 vol_group);
void snd_SetVagStreamPitchModifier(UInt32 handle, SInt16 mod);
UInt16 snd_ModifyRawPitch(SInt32 pm, UInt32 pitch);
void snd_DeferredFileCloseWatcher();
void snd_CloseAllDeferredFiles();
BOOL snd_AllStdioFilesClosed();
void snd_AddDeferredCloseFile(SInt32 handle, int from);

// *****************************************************************************
// FILE -- valloc.c
// *****************************************************************************

void snd_InitVoiceAllocator();
void snd_CleanupVoiceAllocator();
void snd_SetMixerMode(SInt32 channel_mode, SInt32 reverb_mode);
void snd_SetGroupVoiceRange(SInt32 group, SInt32 min, SInt32 max);
SInt32 snd_LockVoiceAllocatorEx(BOOL block, UInt32 ownerID);
SInt32 snd_IsVoiceAllocatorLocked();
SInt32 snd_CheckAllocatorCueTick();
void snd_UnlockVoiceAllocator();
SInt32 snd_ExternVoiceAlloc(SInt32 vol_group, SInt32 priority);
void snd_ExternVoiceFree(SInt32 voice);
void snd_SetVoiceNoiseOrWave(SInt32 voice, BOOL wave);
SInt32 snd_AllocateNoiseVoice(SInt32 vol_group, SInt32 priority);
SInt32 snd_AllocateVoice(SInt32 vol_group, SInt32 priority);
void snd_StartVAGVoice(SInt32 voice, BOOL noise);
void snd_StartVoices(VoiceFlags *voices);
void snd_StartPendingVoices();
void snd_StopPendingVoices();
void snd_GetKeyedOnVoices(VoiceFlags *voices);
void snd_GetKeyedOffVoices(VoiceFlags *voices);
void snd_VoiceIsDone(SInt32 voice);
UInt32 snd_GetVoiceAge(SInt32 voice);
SInt32 snd_GetVoiceStatus(SInt32 voice);
SInt32 snd_GetVoicePriority(SInt32 voice);
void snd_KeyOffVoice(SInt32 voice);
void snd_KeyOffVoicesEx(VoiceFlags *voices, BOOL do_owner_proc);
void snd_SilenceVoicesEx(VoiceFlags *voices, BOOL do_owner_proc);
void snd_PauseVoices(VoiceFlags *voices);
void snd_PauseVoicesOwnedWithOwner(GSoundHandlerPtr owner);
void snd_UnPauseVoices(VoiceFlags *voices);
void snd_UnPauseVoicesOwnedWithOwner(GSoundHandlerPtr owner);
VoiceAttributes* snd_GetVoiceStatusPtr(SInt32 voice);
UInt16 PS1Note2Pitch(SInt8 center_note, SInt8 center_fine, UInt16 note, SInt16 fine);
void snd_PitchBendTone(TonePtr tone, SInt32 pb, SInt32 pm, SInt32 note, SInt32 fine, SInt32 *new_note, SInt32 *new_fine);
void snd_MarkVoicePlaying(SInt32 voice);
void snd_MarkVoiceFree(SInt32 voice);
void snd_HardFreeVoice(SInt32 voice);
void snd_KeyOnVoiceRaw(int core, int voice, BOOL reverb);
void snd_KeyOffVoiceRaw(int core, int voice);

// *****************************************************************************
// FILE -- vol.c
// *****************************************************************************

void snd_InitDuckers();
void snd_SetMasterVolume(SInt32 which, SInt32 vol);
SInt32 snd_GetMasterVolume(SInt32 which);
void snd_SetPlaybackMode(SInt32 mode);
SInt32 snd_GetPlaybackMode();
void snd_MakeVolumes(SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan, SInt32 tone_vol, SInt32 tone_pan, SpuVolume *spu_vol);
void snd_MakeVolumesB(SInt32 bank_vol, SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan, SInt32 tone_vol, SInt32 tone_pan, SpuVolume *spu_vol);
void snd_CalcStereoBalance3d(SInt32 snd_vol, SInt32 snd_pan, SpuVolume *spu_vol);
SInt16 snd_AdjustVolToGroup(SInt16 vol, SInt32 group);
void snd_AdjustAllChannelsMasterVolume(UInt32 flags);
SInt16 snd_MIDITo360Pan(SInt8 pan);
void snd_CheckDuckers();
void snd_SetMasterVolumeDucker(SInt32 which, DuckerDefPtr state);

#endif