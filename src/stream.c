#include "common.h"
#include "libcdvd.h"

/* data 1cbc */ StreamChannelRange gStreamChannelRange[16] = {
    {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47},
    {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47},
};

/* data 1cfc */ VAGStreamHandler *gVAGStreamHandler = NULL;
/* data 1d00 */ SInt32 gNumVAGStreams = 0;
/* data 1d04 */ SInt32 gVAGStreamingInited = 0;
/* data 1d08 */ UInt32 gCdSpinMode = 1;
/* data 1d0c */ SInt32 gHalfBufferSize = 0;
/* data 1d10 */ SInt32 gMasterReadBufferSize = 0;
/* data 1d14 */ SInt32 gMasterReadBufferFirstSector = 0;
/* data 1d18 */ SInt32 gMasterReadBufferLastSector = 0;
/* data 1d1c */ void *gMasterReadBuffer = NULL;
/* data 1d20 */ void *gCurrentReadBuffer = NULL;
/* data 1d24 */ UInt32 gSSNeedSectors = 0;
/* data 1d28 */ UInt32 gSSReadSectors = 0;
/* data 1d2c */ UInt32 gSSReadLoc = 0;
/* data 1d30 */ UInt32 gBreakDataRead = 0;
/* data 1d34 */ SInt32 gLastVAGReadError = 0;
/* data 1d38 */ SInt32 gLastDATAReadError = 0;
/* data 1d3c */ SInt32 gReadWaitThread = -1;
/* data 1d40 */ SInt32 gReadIsEE = 0;
/* data 1d44 */ UInt32 gLastReadSector = 0;
/* data 1d48 */ UInt32 gLastReadAmount = 0;
/* data 1d4c */ UInt32 gLastReadIOPLoc = 0;
/* data 1d50 */ VAGStreamHeader *VAGStreamList = NULL;
/* data 1d54 */ VAGBuffer *VAGStreamLoadList = NULL;
/* data 1d58 */ VAGBuffer *VAGStreamLoadListTail = NULL;
/* data 1d5c */ VAGBuffer *VAGStreamDMAList = NULL;
/* data 1d60 */ VAGBuffer *VAGStreamDMAListTail = NULL;
/* data 1d64 */ VAGBuffer *gVAGReadBusy = NULL;
/* data 1d68 */ SInt32 gDataReadBusy = 0;
/* data 1d6c */ static VAGBuffer *gDMABusy[2] = {0, 0};
/* data 1d74 */ static SInt32 gCDIdleWaiter = 0;
/* data 1d78 */ static SInt32 gStartDataLoadSema = 0;
/* data 1d7c */ SInt32 gStartDMASema = 0;
/* data 1d80 */ char *gSearchFileName = NULL;
/* data 1d84 */ sceCdlFILE *gSearchFileParams = NULL;
/* data 1d88 */ SInt32 gSearchFileSema = 0;
/* data 1d8c */ SInt32 gSearchFileResult = 0;
/* data 1d90 */ SInt32 gDMAThread = 0;
/* data 1d94 */ SInt32 gDataLoadThread = 0;
/* data 1d98 */ SInt32 gDMADoneThread = 0;
/* data 1d9c */ SInt32 gLoadDoneThread = 0;
/* data 1da0 */ SInt32 gDeferredFileCloseThread = 0;
/* data 1da4 */ SInt32 gDeferredFileCloseSema = 0;
/* data 1da8 */ SInt32 gDeferredCloseHandles[16] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
/* data 1de8 */ SInt32 gNumStreamsPlaying = 0;
/* data 1dec */ SInt32 gShuttingDownStreaming = 0;
/* bss 8dd0 */ VAGStreamQueEntry gVAGStreamQue[64];
/* bss 8d90 */ EEVagStreamMonitor vsm;

static int in_48 = 0;
static int in_func_76 = 0;
static int in_func_86 = 0;

INCLUDE_ASM("asm/nonmatchings/stream", snd_InitVAGStreamingEx);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetVAGStreamChannelRange);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AllocateStreamResources);

INCLUDE_ASM("asm/nonmatchings/stream", snd_PlayVAGStreamByNameEx);

INCLUDE_RODATA("asm/nonmatchings/stream", D_0003ADF4);

INCLUDE_ASM("asm/nonmatchings/stream", snd_PlayVAGStreamByLocEx);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetVAGStreamQueueCount);

INCLUDE_ASM("asm/nonmatchings/stream", snd_TryPreBufferQueElement);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetupVAGStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StopAllStreams);

INCLUDE_ASM("asm/nonmatchings/stream", snd_PauseVAGStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_PauseAllStreams);

INCLUDE_ASM("asm/nonmatchings/stream", snd_ContinueVAGStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StopVAGStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamHandlerIsBusyWithLoad);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamHandlerIsBusyWithDMA);

INCLUDE_ASM("asm/nonmatchings/stream", snd_KillVAGStreamEx);

INCLUDE_ASM("asm/nonmatchings/stream", snd_ProcessVAGStreamTick);

INCLUDE_ASM("asm/nonmatchings/stream", snd_CheckVAGStreamProgress);

INCLUDE_ASM("asm/nonmatchings/stream", snd_FixVAGStreamSamplesPlayed);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetVAGStreamLoc);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetVAGStreamTimeRemaining);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetVAGStreamTotalTime);

INCLUDE_ASM("asm/nonmatchings/stream", snd_IsVAGStreamBuffered);

INCLUDE_ASM("asm/nonmatchings/stream", snd_VAGStreamDataLoadThread);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdRead);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdBreak);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdSearchFile);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdReadEEm);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdSync);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCdGetError);

INCLUDE_ASM("asm/nonmatchings/stream", snd_KickDataRead);

INCLUDE_ASM("asm/nonmatchings/stream", snd_KickVAGRead);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromIOPMemory);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromEEMemory);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromMemoryStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromStdio);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromCD);

INCLUDE_ASM("asm/nonmatchings/stream", snd_VAGStreamLoadDoneThread);

INCLUDE_ASM("asm/nonmatchings/stream", snd_RestartInterleavedStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SignalDataReadDone);

INCLUDE_ASM("asm/nonmatchings/stream", snd_VAGStreamDMAThread);

INCLUDE_ASM("asm/nonmatchings/stream", snd_KickVAGDMA);

INCLUDE_ASM("asm/nonmatchings/stream", snd_VAGStreamDMADoneThread);

INCLUDE_ASM("asm/nonmatchings/stream", snd_RepairIOPMemoryBuffer);

INCLUDE_ASM("asm/nonmatchings/stream", snd_VAGDMADone);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StartVAGStreamSounding);

INCLUDE_ASM("asm/nonmatchings/stream", snd_MakeVAGLoop);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetFreeVAGStream);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddVAGStreamDMABuffer);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddVAGStreamLoadBuffer);

INCLUDE_ASM("asm/nonmatchings/stream", snd_RemoveVAGStreamLoadBuffer);

INCLUDE_ASM("asm/nonmatchings/stream", snd_RemoveVAGStreamDMABuffer);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddQueEntryToHandler);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetFreeQueSlot);

INCLUDE_ASM("asm/nonmatchings/stream", snd_FreeQueChain);

INCLUDE_ASM("asm/nonmatchings/stream", snd_CleanUpThread);

INCLUDE_ASM("asm/nonmatchings/stream", snd_CleanUpSema);

INCLUDE_ASM("asm/nonmatchings/stream", snd_FreeVAGStreamResources);

INCLUDE_ASM("asm/nonmatchings/stream", snd_CloseVAGStreaming);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamSafeCheckCDIdle);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetFreeVAGStreamHandler);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddStreamToHandler);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddStreamToHandlerQueue);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetNumStreamChannels);

INCLUDE_ASM("asm/nonmatchings/stream", snd_FixVol);

INCLUDE_ASM("asm/nonmatchings/stream", snd_FixPan);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetVAGStreamSubGroupVolPan);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetVAGStreamVolPan);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetFirstOfAdjacentStreamHeaders);

INCLUDE_ASM("asm/nonmatchings/stream", snd_GetFreeStreamLoop);

INCLUDE_ASM("asm/nonmatchings/stream", snd_StreamIsFreeForVolGroup);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetVagStreamPitchModifier);

INCLUDE_ASM("asm/nonmatchings/stream", snd_ModifyRawPitch);

INCLUDE_ASM("asm/nonmatchings/stream", snd_DeferredFileCloseWatcher);

INCLUDE_ASM("asm/nonmatchings/stream", snd_CloseAllDeferredFiles);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AllStdioFilesClosed);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AddDeferredCloseFile);
