#include "common.h"

INCLUDE_ASM("asm/nonmatchings/stream", snd_InitVAGStreamingEx);

INCLUDE_ASM("asm/nonmatchings/stream", snd_SetVAGStreamChannelRange);

INCLUDE_ASM("asm/nonmatchings/stream", snd_AllocateStreamResources);

INCLUDE_ASM("asm/nonmatchings/stream", snd_PlayVAGStreamByNameEx);

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
