#ifndef STREAM_H_
#define STREAM_H_

#include "common.h"

#include <libcdvd.h>

BOOL snd_InitVAGStreamingEx(SInt32 num_channels, SInt32 buffer_size, UInt32 read_mode, BOOL enable_streamsafe_from_ee);
void snd_SetVAGStreamChannelRange(SInt32 vol_group, SInt32 ch_min, SInt32 ch_max);
SInt32 snd_AllocateStreamResources(VAGStreamHeader **stream1, VAGStreamHeader **stream2, SInt32 stereo,
                                   SInt32 get_voices, SInt32 vol_group);
UInt32 snd_PlayVAGStreamByNameEx(char *name1, char *name2, UInt32 offset1, UInt32 offset2, SInt32 vol, SInt32 pan,
                                 SInt32 vol_group, UInt32 queue, SInt32 sub_group, UInt32 flags);
UInt32 snd_PlayVAGStreamByLocEx(SInt32 loc1, SInt32 loc2, SInt32 offset1, SInt32 offset2, SInt32 vol, SInt32 pan,
                                SInt32 vol_group, UInt32 queue, SInt32 sub_group, UInt32 flags);
SInt32 snd_GetVAGStreamQueueCount(UInt32 handle);
void snd_TryPreBufferQueElement(VAGStreamHandlerPtr handler, UInt32 load_flags);
void snd_SetupVAGStream(VAGStreamHandlerPtr handler, VAGStreamHeader *stream, SInt32 loc, SInt32 offset,
                        SInt32 part_of_queue, UInt32 flags);
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
VAGStreamHeader *snd_GetFreeVAGStream(SInt32 vol_group);
SInt32 snd_AddVAGStreamDMABuffer(VAGBuffer *buffer);
BOOL snd_AddVAGStreamLoadBuffer(VAGBuffer *buffer);
void snd_RemoveVAGStreamLoadBuffer(VAGBuffer *buffer);
void snd_RemoveVAGStreamDMABuffer(VAGBuffer *buffer);
void snd_AddQueEntryToHandler(VAGStreamHandlerPtr handler, VAGStreamQueEntry *que);
VAGStreamQueEntry *snd_GetFreeQueSlot();
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
SInt32 snd_GetFreeStreamLoop(VAGStreamHeaderPtr start, VAGStreamHeaderPtr consider, SInt32 num_channels,
                             VAGStreamHeaderPtr *last_tested, SInt32 vol_group);
BOOL snd_StreamIsFreeForVolGroup(VAGStreamHeaderPtr stream, SInt32 vol_group);
void snd_SetVagStreamPitchModifier(UInt32 handle, SInt16 mod);
UInt16 snd_ModifyRawPitch(SInt32 pm, UInt32 pitch);
void snd_DeferredFileCloseWatcher();
void snd_CloseAllDeferredFiles();
BOOL snd_AllStdioFilesClosed();
void snd_AddDeferredCloseFile(SInt32 handle, int from);

#endif // STREAM_H_
