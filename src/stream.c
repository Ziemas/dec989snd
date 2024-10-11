#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libcdvd.h"
#include "libsd.h"
#include "sifrpc.h"
#include "stdio.h"
#include "thread.h"
#include "sif.h"
#include <string.h>

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

BOOL snd_InitVAGStreamingEx(SInt32 num_channels, SInt32 buffer_size,
                            UInt32 read_mode, BOOL enable_streamsafe_from_ee) {
    SInt32 x;
    SInt32 y;
    VAGStreamHeader *header;
    VAGStreamHeaderPtr *list;
    struct ThreadParam thread;
    struct SemaParam sema;
    SInt8 *iop_buffer;
    UInt32 spu_buffer;
    SInt32 intr_state;
    SInt32 dis;

    if (VAGStreamList) {
        snd_ShowError(37, 0, 0, 0, 0);
        return 0;
    }

    VAGStreamLoadList = NULL;
    VAGStreamLoadListTail = NULL;
    VAGStreamDMAList = NULL;
    VAGStreamDMAListTail = NULL;
    gVAGReadBusy = 0;
    gDMABusy[0] = 0;
    gDMABusy[1] = 0;
    sceCdSync(0x10);
    gCdSpinMode = read_mode;
    gNumVAGStreams = num_channels;

    x = buffer_size / 4096;
    if (x < 2) {
        x = 2;
    }
    buffer_size = x * 4096;
    gHalfBufferSize = buffer_size >> 1;

    thread.attr = 0x2000000;
    thread.entry = snd_VAGStreamDMAThread;
    thread.initPriority = gThreadPriority_STREAM;
    thread.stackSize = 2048;
    thread.option = 0;
    gDMAThread = CreateThread(&thread);
    if (gDMAThread <= 0) {
        snd_ShowError(38, 0, 0, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    thread.entry = snd_VAGStreamDataLoadThread;
    thread.initPriority = gThreadPriority_STREAM + 2;
    gDataLoadThread = CreateThread(&thread);
    if (gDataLoadThread <= 0) {
        snd_ShowError(39, 0, 0, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    thread.entry = snd_VAGStreamDMADoneThread;
    thread.initPriority = gThreadPriority_STREAM + 1;
    gDMADoneThread = CreateThread(&thread);
    if (gDMADoneThread <= 0) {
        snd_ShowError(40, 0, 0, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    thread.entry = snd_VAGStreamLoadDoneThread;
    thread.initPriority = gThreadPriority_STREAM + 3;
    gLoadDoneThread = CreateThread(&thread);
    if (gLoadDoneThread <= 0) {
        snd_ShowError(41, 0, 0, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    thread.entry = snd_DeferredFileCloseWatcher;
    thread.initPriority = gThreadPriority_STREAM + 4;
    gDeferredFileCloseThread = CreateThread(&thread);
    if (gDeferredFileCloseThread <= 0) {
        snd_ShowError(109, 0, 0, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    sema.attr = 1;
    sema.initCount = 0;
    sema.maxCount = 1;
    gStartDataLoadSema = CreateSema(&sema);
    gStartDMASema = CreateSema(&sema);
    gDoneDMASema = CreateSema(&sema);
    gDoneLoadSema = CreateSema(&sema);
    gSearchFileSema = CreateSema(&sema);
    gDeferredFileCloseSema = CreateSema(&sema);
    gVAGStreamHandler =
        gAllocProc(sizeof(*gVAGStreamHandler) * num_channels, 4, 0);
    if (!gVAGStreamHandler) {
        snd_ShowError(42, num_channels, 88, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    memset(gVAGStreamHandler, 0, sizeof(*gVAGStreamHandler) * num_channels);
    for (x = 0; x < num_channels; x++) {
        gVAGStreamHandler[x].SH.OwnerID = 1;
        gVAGStreamHandler[x].SH.OwnerID |= 0x4000000 | (x << 16);
        gVAGStreamHandler[x].SH.Voices.core[0] =
            gVAGStreamHandler[x].SH.Voices.core[1] = 0;
        gVAGStreamHandler[x].doubling_voice = -1;
        gVAGStreamHandler[x].pre_buffer_queue = 1;
        for (y = 0; y < 16; y++) {
            gVAGStreamHandler[x].group_vols[y] = 127;
        }
    }

    iop_buffer = gAllocProc(buffer_size * num_channels, 5, 0);
    if (!iop_buffer) {
        snd_ShowError(43, buffer_size, num_channels, 0, 0);
        snd_FreeVAGStreamResources();
        return 0;
    }

    dis = CpuSuspendIntr(&intr_state);
    spu_buffer = snd_SRAMMalloc(buffer_size * num_channels);
    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    if (!spu_buffer) {
        snd_ShowError(44, 0, 0, 0, 0);
        gFreeProc(iop_buffer);
        snd_FreeVAGStreamResources();
        return 0;
    }

    list = &VAGStreamList;
    for (x = 0; x < num_channels; x++) {
        header = gAllocProc(sizeof(*header), 4, 0);
        if (!header) {
            snd_ShowError(45, 0, 0, 0, 0);
            dis = CpuSuspendIntr(&intr_state);
            snd_SRAMFree(spu_buffer, buffer_size * num_channels);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
            gFreeProc(iop_buffer);
            snd_FreeVAGStreamResources();
            return 0;
        }

        memset(header, 0, sizeof(*header));

        header->flags = 1;
        header->buff_size = buffer_size;
        header->buff[0].IOPbuff = (iop_buffer + buffer_size / 2 * x);
        header->buff[1].IOPbuff =
            (buffer_size / 2 * num_channels + iop_buffer + buffer_size / 2 * x);
        header->buff[0].OrigIOPbuff = header->buff[0].IOPbuff;
        header->buff[1].OrigIOPbuff = header->buff[1].IOPbuff;
        header->buff[0].SPUbuff = (SInt8 *)(spu_buffer + buffer_size / 2 * x);
        header->buff[1].SPUbuff = (SInt8 *)(buffer_size / 2 * num_channels +
                                            spu_buffer + buffer_size / 2 * x);
        header->buff[0].owner = header;
        header->buff[1].owner = header;
        *list = header;
        list = &header->next;
    }

    if (!enable_streamsafe_from_ee) {
        gMasterReadBufferSize = 0;
        gMasterReadBuffer = 0;
    } else {
        gMasterReadBufferSize = gHalfBufferSize;
        gMasterReadBuffer = gAllocProc(gMasterReadBufferSize, 5, 0);
        if (!gMasterReadBuffer) {
            snd_ShowError(46, 0, 0, 0, 0);
            snd_FreeVAGStreamResources();
            return 0;
        }
        gMasterReadBufferSize /= 2048;
    }

    for (x = 0; x < 64; x++) {
        gVAGStreamQue[x].flags = 0;
    }

    StartThread(gDMAThread, 0);
    StartThread(gDataLoadThread, 0);
    StartThread(gDMADoneThread, 0);
    StartThread(gLoadDoneThread, 0);
    StartThread(gDeferredFileCloseThread, 0);

    for (x = 0; x < 16; x++) {
        gStreamChannelRange[x].min = 0;
        gStreamChannelRange[x].max = num_channels - 1;
    }

    gVAGStreamingInited = 1;

    return 1;
}

void snd_SetVAGStreamChannelRange(SInt32 vol_group, SInt32 ch_min,
                                  SInt32 ch_max) {
    if (vol_group >= 16) {
        return;
    }

    gStreamChannelRange[vol_group].min = ch_min;
    gStreamChannelRange[vol_group].max = ch_max;
}

SInt32 snd_AllocateStreamResources(VAGStreamHeader **stream1,
                                   VAGStreamHeader **stream2, SInt32 stereo,
                                   SInt32 get_voices, SInt32 vol_group) {
    *stream1 = snd_GetFreeVAGStream(vol_group);
    if (!*stream1) {
        return 0;
    }

    (*stream1)->sync_list = NULL;

    if (stereo) {
        *stream2 = snd_GetFreeVAGStream(vol_group);
        if (!*stream2) {
            (*stream1)->flags = 1;
            return 0;
        }

        (*stream2)->sync_list = NULL;
    }

    if (!get_voices) {
        (*stream1)->voice = -1;
        if (stereo) {
            (*stream2)->voice = -1;
        }
        return 1;
    }

    snd_LockVoiceAllocatorEx(1, 90);
    (*stream1)->voice = snd_AllocateVoice(vol_group, 127);

    if ((*stream1)->voice == -1) {
        (*stream1)->flags = 1;
        if (stereo) {
            (*stream2)->flags = 1;
        }

        snd_UnlockVoiceAllocator();
        return 0;
    } else {
        gChannelStatus[(*stream1)->voice].Status = 4;
    }

    if (stereo) {
        (*stream2)->voice = snd_AllocateVoice(vol_group, 127);
        if ((*stream2)->voice == -1) {
            (*stream1)->flags = 1;
            (*stream2)->flags = 1;
            snd_MarkVoiceFree((*stream1)->voice);
            snd_UnlockVoiceAllocator();
            return 0;
        } else {
            gChannelStatus[(*stream2)->voice].Status = 4;
        }
    }

    snd_UnlockVoiceAllocator();
    return 1;
}

UInt32 snd_PlayVAGStreamByNameEx(char *name1, char *name2, UInt32 offset1,
                                 UInt32 offset2, SInt32 vol, SInt32 pan,
                                 SInt32 vol_group, UInt32 queue,
                                 SInt32 sub_group, UInt32 flags) {
    SInt32 fh1;
    SInt32 fh2;

    fh1 = -1;
    fh2 = -1;

    if (name1 && (fh1 = open(name1, 1), fh1 < 0)) {
        if (!gPrefs_Silent) {
            printf("989snd: Couldn't open file -> %s as stream\n", name1);
        }
        return 0;
    }

    if (name2 && (fh2 = open(name2, 1), fh2 < 0)) {
        if (!gPrefs_Silent) {
            printf("989snd: Couldn't open file -> %s as stream\n", name2);
        }
        close(fh1);
        return 0;
    }

    flags |= 0x1000;

    return snd_PlayVAGStreamByLocEx(fh1, fh2, offset1, offset2, vol, pan,
                                    vol_group, queue, sub_group, flags);
}

UInt32 snd_PlayVAGStreamByLocEx(SInt32 loc1, SInt32 loc2, SInt32 offset1,
                                SInt32 offset2, SInt32 vol, SInt32 pan,
                                SInt32 vol_group, UInt32 queue,
                                SInt32 sub_group, UInt32 flags) {
    VAGStreamHandlerPtr handler;
    VAGStreamHeader *thestream;
    VAGStreamHeader *thestreamR;
    SInt32 core;
    SInt32 c_v;
    VAGStreamQueEntry *slot1;
    VAGStreamQueEntry *slot2;
    SInt32 x;

    thestream = NULL;
    thestreamR = NULL;

    if (flags & 0x1000) {
        loc1 += 1;
        loc2 += 1;
    }

    if (!VAGStreamList) {
        snd_ShowError(47, 0, 0, 0, 0);
        return 0;
    }

    snd_LockMasterTick(80);
    if (queue) {
        handler = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(queue);
        if (!handler) {
            snd_UnlockMasterTick();
            if ((flags & 0x10) != 0) {
                if ((flags & 0x1000) != 0) {
                    if (loc1) {
                        snd_AddDeferredCloseFile(loc1 - 1, 1);
                    }
                    if (loc2) {
                        snd_AddDeferredCloseFile(loc2 - 1, 2);
                    }
                }

                return 0;
            }

            if ((flags & 0x1000) != 0) {
                loc1--;
                loc2--;
            }

            return snd_PlayVAGStreamByLocEx(loc1, loc2, offset1, offset2, vol,
                                            pan, vol_group, 0, sub_group,
                                            flags);
        }

        if (flags & 8) {
            handler->pre_buffer_queue = 0;
        }
        if (pan == -1 || pan == -2) {
            pan = handler->SH.Current_Pan;
        }
        if (vol == 0x7FFFFFFF) {
            vol = (handler->SH.Current_Vol * 1024) / 127;
        }

        thestream = handler->SH.Sound;
        if (thestream->sync_list && loc2) {
            slot1 = snd_GetFreeQueSlot();
            slot2 = snd_GetFreeQueSlot();
            if (!slot1 || !slot2) {
                if (slot1) {
                    slot1->flags = 0;
                }
                if (slot2) {
                    slot2->flags = 0;
                }

                snd_UnlockMasterTick();

                if ((flags & 0x1000) != 0) {
                    if (loc1) {
                        snd_AddDeferredCloseFile(loc1 - 1, 3);
                    }
                    if (loc2) {
                        snd_AddDeferredCloseFile(loc2 - 1, 4);
                    }
                }
                return 0;
            }

            slot1->loc = loc1;
            slot1->offset = offset1;
            slot2->loc = loc2;
            slot2->offset = offset2;
            slot1->sub_group = sub_group;
            slot2->sub_group = sub_group;
            if (flags & 0x40) {
                handler->SH.flags |= 0x10u;
                slot1->vol = slot2->vol = (127 * vol) >> 10;
                slot1->pan = slot2->pan = pan;
            } else {
                slot1->vol = slot2->vol = -1;
                slot1->pan = slot2->pan = -1;
            }

            if (flags & 2) {
                slot1->flags |= 2u;
                slot2->flags |= 2u;
            }
            if (flags & 4) {
                slot1->flags |= 4u;
                slot2->flags |= 4u;
            }

            snd_AddQueEntryToHandler(handler, slot1);
            snd_AddQueEntryToHandler(handler, slot2);
        } else {
            slot1 = snd_GetFreeQueSlot();
            if (!slot1) {
                snd_UnlockMasterTick();
                if (flags & 0x1000) {
                    if (loc1) {
                        snd_AddDeferredCloseFile(loc1 - 1, 5);
                    }
                    if (loc2) {
                        snd_AddDeferredCloseFile(loc2 - 1, 6);
                    }
                }
                return 0;
            }

            slot1->loc = loc1;
            slot1->offset = offset1;
            slot1->sub_group = sub_group;

            if (flags & 0x40) {
                handler->SH.flags |= 0x10u;
                slot1->vol = (127 * vol) >> 10;
                slot1->pan = pan;
            } else {
                slot1->vol = -1;
                slot1->pan = -1;
            }

            if (flags & 2) {
                slot1->flags |= 2u;
            }

            if (flags & 4) {
                slot1->flags |= 4u;
            }

            snd_AddQueEntryToHandler(handler, slot1);
        }

        snd_UnlockMasterTick();
        return 1;
    }

    if (!snd_AllocateStreamResources(&thestream, &thestreamR, loc2 != 0, 1,
                                     vol_group)) {
        snd_UnlockMasterTick();
        if (flags & 0x1000) {
            if (loc1) {
                snd_AddDeferredCloseFile(loc1 - 1, 7);
            }
            if (loc2) {
                snd_AddDeferredCloseFile(loc2 - 1, 8);
            }
        }

        return 0;
    }

    handler = snd_GetFreeVAGStreamHandler();
    if (!handler) {
        snd_ShowError(48, 0, 0, 0, 0);
        snd_UnlockMasterTick();
        if (flags & 0x1000) {
            if (loc1) {
                snd_AddDeferredCloseFile(loc1 - 1, 9);
            }
            if (loc2) {
                snd_AddDeferredCloseFile(loc2 - 1, 10);
            }
        }
        return 0;
    }

    handler->SH.Current_Vol = (127 * vol) >> 10;
    handler->SH.Original_Vol = (127 * vol) >> 10;
    if (pan == -1 || pan == -2) {
        pan = 0;
    }

    handler->SH.Current_Pan = pan;
    handler->SH.Original_Pan = pan;
    handler->SH.Current_PM = 0;
    handler->SH.flags = 0;

    if (flags & 1) {
        handler->SH.flags |= 2u;
    }
    handler->SH.VolGroup = vol_group;
    handler->SH.Voices.core[0] = 0;
    handler->SH.Voices.core[1] = 0;
    core = thestream->voice / 24;
    c_v = thestream->voice % 24;
    handler->SH.Voices.core[core] |= 1 << c_v;

    if (flags & 0x20 && !loc2) {
        if (handler->doubling_voice != -1) {
            printf("WOW! doubling voice is allocated 1!\n");
        }

        snd_LockVoiceAllocatorEx(1, 91);
        handler->doubling_voice = snd_AllocateVoice(vol_group, 127);
        if (handler->doubling_voice != -1) {
            gChannelStatus[handler->doubling_voice].Status = 4;
            gChannelStatus[handler->doubling_voice].Owner = &handler->SH;
            core = handler->doubling_voice / 24;
            c_v = handler->doubling_voice % 24;
            handler->SH.Voices.core[core] |= 1 << c_v;
        }

        snd_UnlockVoiceAllocator();
    } else {
        if (handler->doubling_voice != -1) {
            printf("WOW! doubling voice is allocated 2!\n");
        }
    }

    handler->num_streams = 1;
    handler->que_list = 0;
    handler->qued_stream = 0;

    if (flags & 8) {
        handler->pre_buffer_queue = 0;
    } else {
        handler->pre_buffer_queue = 1;
    }

    for (x = 0; x < 16; ++x) {
        handler->group_vols[x] = 127;
    }

    if (loc2) {
        core = thestreamR->voice / 24;
        c_v = thestreamR->voice % 24;
        handler->SH.Voices.core[core] |= 1 << c_v;
        handler->num_streams++;
    }

    snd_SetupVAGStream(handler, thestream, loc1, offset1, 0, flags & 0x1f800);
    if ((flags & 1) != 0) {
        thestream->flags |= 0x10u;
    }
    if ((flags & 2) != 0) {
        thestream->flags |= 0x100u;
    }
    if ((flags & 4) != 0) {
        thestream->flags |= 0x400u;
    }

    thestream->sub_group = sub_group;
    if (loc2) {
        snd_SetupVAGStream(handler, thestreamR, loc2, offset2, 0,
                           flags & 0x1f800);
        if ((flags & 1) != 0) {
            thestreamR->flags |= 0x10u;
        }
        if ((flags & 2) != 0) {
            thestreamR->flags |= 0x100u;
        }
        if ((flags & 4) != 0) {
            thestreamR->flags |= 0x400u;
        }
        thestreamR->sub_group = sub_group;
    }

    if ((flags & 0x40) != 0) {
        handler->SH.flags |= 0x10u;
    }

    snd_AddVAGStreamLoadBuffer(thestream->buff);
    SignalSema(gStartDataLoadSema);
    snd_ActivateHandler(&handler->SH);
    snd_UnlockMasterTick();

    return handler->SH.OwnerID;
}

SInt32 snd_GetVAGStreamQueueCount(UInt32 handle) {
    SInt32 count;
    VAGStreamHandlerPtr hand;
    VAGStreamQueEntry *walk;
    VAGStreamHeader *stream;

    count = 0;
    snd_LockMasterTick(81);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return count;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    if ((stream->flags & 2) != 0 || (stream->flags & 0x10) != 0) {
        count++;
    }

    if (hand->qued_stream) {
        count++;
    }

    for (walk = hand->que_list; walk; walk = walk->next) {
        count++;
    }

    snd_UnlockMasterTick();

    return count;
}

void snd_TryPreBufferQueElement(VAGStreamHandlerPtr handler,
                                UInt32 load_flags) {
    VAGStreamHeader *currstream;
    VAGStreamHeader *thestream;
    VAGStreamHeader *thestreamR;
    SInt32 stereo;

    currstream = NULL;
    thestream = NULL;
    thestreamR = NULL;

    if (handler->qued_stream || !handler->que_list) {
        return;
    }

    currstream = (VAGStreamHeader *)handler->SH.Sound;

    stereo = (snd_GetNumStreamChannels(currstream) == 2 &&
              !(currstream->buff[0].flags & 0x100))
                 ? 1
                 : 0;

    if (!snd_AllocateStreamResources(&thestream, &thestreamR, stereo,
                                     currstream->buff[0].flags & 0x100,
                                     handler->SH.VolGroup)) {
        return;
    }

    thestream->flags |= 0x10u;
    if ((handler->que_list->flags & 2) != 0) {
        thestream->flags |= 0x100u;
    }
    if ((handler->que_list->flags & 4) != 0) {
        thestream->flags |= 0x400u;
    }

    snd_SetupVAGStream(handler, thestream, handler->que_list->loc,
                       handler->que_list->offset, 1, load_flags);
    thestream->sub_group = handler->que_list->sub_group;

    if ((handler->SH.flags & 0x10) != 0) {
        thestream->master_volume = handler->que_list->vol;
        thestream->master_pan = handler->que_list->pan;
    }

    handler->que_list->flags = 0;
    handler->que_list = handler->que_list->next;

    if (stereo) {
        thestreamR->flags |= 0x10u;
        if ((handler->que_list->flags & 2) != 0) {
            thestreamR->flags |= 0x100u;
        }
        if ((handler->que_list->flags & 4) != 0) {
            thestreamR->flags |= 0x400u;
        }

        snd_SetupVAGStream(handler, thestreamR, handler->que_list->loc,
                           handler->que_list->offset, 1, load_flags);
        thestreamR->sub_group = handler->que_list->sub_group;

        if ((handler->SH.flags & 0x10) != 0) {
            thestreamR->master_volume = handler->que_list->vol;
            thestreamR->master_pan = handler->que_list->pan;
        }

        handler->que_list->flags = 0;
        handler->que_list = handler->que_list->next;
    }

    snd_AddVAGStreamLoadBuffer(thestream->buff);
    handler->qued_stream = thestream;
    SignalSema(gStartDataLoadSema);
}

void snd_SetupVAGStream(VAGStreamHandlerPtr handler, VAGStreamHeader *stream,
                        SInt32 loc, SInt32 offset, SInt32 part_of_queue,
                        UInt32 flags) {
    EEVagStreamMonitor vsm;
    IOPVagStreamMonitor *vsmp;
    sceSifReceiveData data_track;

    if (stream->voice > -1) {
        gChannelStatus[stream->voice].Owner = &handler->SH;
    }

    if ((flags & 0x4000) != 0 || (flags & 0x8000) != 0) {
        stream->ee_streammon_loc = loc;
        if (sceSifGetOtherData(&data_track, (void *)loc, &vsm, 64, 0)) {
            snd_ShowError(114, 0, 0, 0, 0);
            loc = -1;
        } else {
            loc = (UInt32)vsm.EEStartAddress;
            stream->ee_BufferSize = vsm.BufferSize;
            stream->ee_EEStartAddress = vsm.EEStartAddress;
            stream->ee_NextIOPReadAddress = vsm.NextIOPReadAddress;
            stream->ee_TotalBytesConsumed = 0;
        }
    } else {
        if ((flags & 0x10000) != 0) {
            stream->ee_streammon_loc = loc;
            vsmp = (IOPVagStreamMonitor *)loc;
            loc = (UInt32)vsmp->IOPStartAddress;
            stream->ee_BufferSize = vsmp->BufferSize;
            stream->ee_EEStartAddress = vsmp->IOPStartAddress;
            stream->ee_NextIOPReadAddress = vsmp->NextIOPReadAddress;
            stream->ee_TotalBytesConsumed = 0;
        } else {
            stream->ee_streammon_loc = 0;
            stream->stdio_file_offset = 0;
        }
    }
    stream->next_read_offset = loc;
    stream->file_start_sector = loc;
    stream->file_start_offset = offset;
    stream->bytes_remaining = 2048;
    stream->buff[0].IOPbuff = stream->buff[0].OrigIOPbuff;
    stream->buff[1].IOPbuff = stream->buff[1].OrigIOPbuff;
    stream->buff[0].flags = 16;
    stream->buff[1].flags = 0;
    stream->buff[0].is_end = 0;
    stream->buff[1].is_end = 0;
    stream->PlayingBuffer = 0;
    stream->LastPosition = 0;
    stream->ErrorAccumulator = 0;
    stream->LastBufferChangeTick = 0;
    stream->pitch = 0;
    stream->handler = handler;
    stream->sample_rate = -1;
    stream->samples_total = 0;
    stream->bytes_played = 0;
    stream->sync_list = 0;
    stream->group = 0;
    stream->sub_group = 0;
    stream->flags |= flags;
    stream->start_error_accumulator = 0;

    if ((handler->SH.flags & 0x10) == 0) {
        stream->master_volume = handler->SH.Current_Vol;
        stream->master_pan = handler->SH.Current_Pan;
    } else {
        stream->master_volume = 0;
        stream->master_pan = 0;
    }

    if ((flags & 0x1000) != 0) {
        stream->file_handle = loc - 1;
        stream->next_read_offset = 0;
        stream->file_start_sector = 0;
        stream->file_start_offset = 0;
        stream->stdio_file_offset = offset;
    }

    if (!part_of_queue) {
        snd_AddStreamToHandler(handler, stream);
    } else {
        snd_AddStreamToHandlerQueue(handler, stream);
    }
}

void snd_StopAllStreams() {
    SInt32 x;
    SInt32 c;
    SInt32 cc;
    BOOL done;

    done = false;
    cc = 0;

    while (!done && cc < 5) {
        for (x = 0; x < gNumVAGStreams; ++x) {
            if (gVAGStreamHandler[x].SH.Sound) {
                if ((gVAGStreamHandler[x].SH.OwnerID & 0x80000000) == 0) {
                    gVAGStreamHandler[x].SH.Sound = 0;
                } else {
                    snd_StopVAGStream(gVAGStreamHandler[x].SH.OwnerID);
                }
            }
        }

        for (c = 0; !done && c < 480; ++c) {
            done = 1;
            for (x = 0; x < gNumVAGStreams && done; x++) {
                if (gVAGStreamHandler[x].SH.Sound)
                    done = 0;
            }

            if (!done) {
                WaitSema(gSTickSema);
            }
        }

        cc++;
        if (cc == 4 && !done) {
            sceCdBreak();
        }
    }

    if (cc == 3 && !done && !gPrefs_Silent) {
        printf(
            "snd_StopAllStreams: ERROR! tried 5 times to stopp all streams!\n");
    }

    while (!snd_AllStdioFilesClosed()) {
        DelayThread(33000);
    }

    if (!gVAGReadBusy && !gDataReadBusy && !VAGStreamLoadList) {
        if (gCDIdleWaiter) {
            WakeupThread(gCDIdleWaiter);
            gCDIdleWaiter = 0;
        }
    }
}

void snd_PauseVAGStream(UInt32 handle) {
    SInt32 core;
    SInt32 voice;
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;
    SInt32 intr_state;
    SInt32 dis;
    SInt32 x;

    snd_LockMasterTick(82);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    if ((hand->SH.flags & 2) != 0) {
        snd_UnlockMasterTick();
        return;
    }

    hand->SH.flags |= 2u;
    stream = (VAGStreamHeader *)hand->SH.Sound;
    dis = CpuSuspendIntr(&intr_state);
    x = 0;
    while (stream) {
        stream->flags |= 0x10u;
        core = stream->voice / 24;
        voice = stream->voice % 24;
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLL, 0);
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLR, 0);
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_PITCH, 0);

        gNumStreamsPlaying--;

        if (!x && hand->doubling_voice != -1) {
            core = hand->doubling_voice / 24;
            voice = hand->doubling_voice % 24;
            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLL, 0);
            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLR, 0);
            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_PITCH, 0);
        }

        stream = stream->sync_list;
        x++;
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    snd_UnlockMasterTick();
}

void snd_PauseAllStreams() {
    SInt32 x;

    for (x = 0; x < gNumVAGStreams; ++x) {
        if (gVAGStreamHandler[x].SH.Sound) {
            snd_PauseVAGStream(gVAGStreamHandler[x].SH.OwnerID);
        }
    }
}

void snd_ContinueVAGStream(UInt32 handle) {
    SInt32 core;
    SInt32 voice;
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;
    SInt32 state;
    SInt32 dis;
    SInt32 x;

    snd_LockMasterTick(83);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    if ((hand->SH.flags & 2) == 0) {
        snd_UnlockMasterTick();
        return;
    }

    dis = CpuSuspendIntr(&state);
    hand->SH.flags &= ~2u;
    stream = (VAGStreamHeader *)hand->SH.Sound;
    x = 0;
    while (stream) {
        if ((stream->flags & 4) != 0) {
            core = stream->voice / 24;
            voice = stream->voice % 24;
            sceSdSetParam(
                (core | SD_VOICE(voice)) | SD_VP_PITCH,
                snd_ModifyRawPitch(hand->SH.Current_PM, stream->pitch));

            if (!x && hand->doubling_voice != -1) {
                core = hand->doubling_voice / 24;
                voice = hand->doubling_voice % 24;
                sceSdSetParam(
                    (core | SD_VOICE(voice)) | SD_VP_PITCH,
                    snd_ModifyRawPitch(hand->SH.Current_PM, stream->pitch));
            }
        }

        stream = stream->sync_list;
        ++x;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    x = 0;
    while (stream) {
        stream->flags &= ~0x10u;
        if ((stream->flags & 4) != 0) {
            core = stream->voice / 24;
            voice = stream->voice % 24;
            sceSdSetParam(core | SD_VOICE(voice) | SD_VP_VOLL,
                          (UInt16)snd_AdjustVolToGroup(
                              gChannelStatus[stream->voice].Volume.left,
                              gChannelStatus[stream->voice].VolGroup) >>
                              1);

            sceSdSetParam(core | SD_VOICE(voice) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(
                              gChannelStatus[stream->voice].Volume.right,
                              gChannelStatus[stream->voice].VolGroup) >>
                              1);

            gNumStreamsPlaying++;
            if (!x && hand->doubling_voice != -1) {
                core = hand->doubling_voice / 24;
                voice = hand->doubling_voice % 24;
                sceSdSetParam(
                    core | SD_VOICE(voice) | SD_VP_VOLL,
                    (UInt16)snd_AdjustVolToGroup(
                        gChannelStatus[hand->doubling_voice].Volume.left,
                        gChannelStatus[hand->doubling_voice].VolGroup) >>
                        1);

                sceSdSetParam(
                    core | SD_VOICE(voice) | SD_VP_VOLR,
                    (UInt16)snd_AdjustVolToGroup(
                        gChannelStatus[hand->doubling_voice].Volume.right,
                        gChannelStatus[hand->doubling_voice].VolGroup) >>
                        1);
            }
        }
        stream = stream->sync_list;
        ++x;
    }

    if (!dis) {
        CpuResumeIntr(state);
    }

    snd_UnlockMasterTick();
}

void snd_StopVAGStream(UInt32 handle) {
    VAGStreamHandlerPtr hand;

    snd_LockMasterTick(84);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    snd_SetVAGStreamVolPan(handle, 0, 0);
    hand->SH.flags |= 4u;

    snd_UnlockMasterTick();
}

BOOL snd_StreamHandlerIsBusyWithLoad(VAGStreamHandlerPtr hand) {
    VAGStreamHeader *stream;

    for (stream = (VAGStreamHeader *)hand->SH.Sound; stream;
         stream = stream->sync_list) {
        if (gVAGReadBusy == stream->buff || gVAGReadBusy == &stream->buff[1]) {
            return true;
        }
    }

    return false;
}

BOOL snd_StreamHandlerIsBusyWithDMA(VAGStreamHandlerPtr hand) {
    VAGStreamHeader *stream;

    for (stream = (VAGStreamHeader *)hand->SH.Sound; stream;
         stream = stream->sync_list) {
        if (gDMABusy[0] == stream->buff || gDMABusy[0] == &stream->buff[1] ||
            gDMABusy[1] == stream->buff || gDMABusy[1] == &stream->buff[1]) {
            return true;
        }
    }

    return false;
}

void snd_KillVAGStreamEx(UInt32 handle, SInt32 from_where) {
    SInt32 core;
    SInt32 voice;
    VAGStreamHeader *master_stream;
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;
    SInt32 dis;
    SInt32 intr_state;
    static int in = 0;

    if (in) {
        return;
    }
    in = 1;

    dis = CpuSuspendIntr(&intr_state);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
        in = 0;
        return;
    }

    master_stream = stream = (VAGStreamHeader *)hand->SH.Sound;
    snd_FreeQueChain(&hand->que_list->flags, (stream->flags >> 12) & 1);
    if (hand->doubling_voice != -1) {
        core = hand->doubling_voice / 24;
        voice = hand->doubling_voice % 24;
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLL, 0);
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLR, 0);
        sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_PITCH, 0);
        snd_KeyOffVoice(hand->doubling_voice);
        snd_MarkVoicePlaying(hand->doubling_voice);
        gChannelStatus[hand->doubling_voice].Priority = 0;
        hand->doubling_voice = -1;
    }

    while (stream) {
        if ((stream->flags & 0x1000) != 0 && stream->file_handle != -1) {
            snd_AddDeferredCloseFile(stream->file_handle, 11);
            stream->file_handle = -1;
        }

        stream->flags = 1;
        snd_RemoveVAGStreamDMABuffer(stream->buff);
        snd_RemoveVAGStreamDMABuffer(&stream->buff[1]);
        snd_RemoveVAGStreamLoadBuffer(stream->buff);
        snd_RemoveVAGStreamLoadBuffer(&stream->buff[1]);
        if (stream->voice != -1) {
            core = stream->voice / 24;
            voice = stream->voice % 24;

            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLL, 0);
            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLR, 0);
            sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_PITCH, 0);
            snd_KeyOffVoice(stream->voice);
            snd_MarkVoicePlaying(stream->voice);
            gChannelStatus[stream->voice].Priority = 0;
            stream->voice = -1;
        }
        gNumStreamsPlaying--;
        stream = stream->sync_list;
    }

    if (hand->qued_stream) {
        for (stream = hand->qued_stream; stream; stream = stream->sync_list) {
            if ((stream->flags & 0x1000) != 0 && stream->file_handle != -1) {
                snd_AddDeferredCloseFile(stream->file_handle, 12);
                stream->file_handle = -1;
            }
            stream->flags = 1;
            snd_RemoveVAGStreamDMABuffer(stream->buff);
            snd_RemoveVAGStreamDMABuffer(&stream->buff[1]);
            snd_RemoveVAGStreamLoadBuffer(stream->buff);
            snd_RemoveVAGStreamLoadBuffer(&stream->buff[1]);
            if (stream->voice != -1) {
                core = stream->voice / 24;
                voice = stream->voice % 24;
                sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLL, 0);
                sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_VOLR, 0);
                sceSdSetParam((core | SD_VOICE(voice)) | SD_VP_PITCH, 0);
                snd_KeyOffVoice(stream->voice);
                snd_MarkVoicePlaying(stream->voice);
                gChannelStatus[stream->voice].Priority = 0;
                stream->voice = -1;
            }
        }
    }

    snd_DeactivateHandler(&hand->SH, 0);

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    SignalSema(gStartDataLoadSema);
    in = 0;
}

void snd_ProcessVAGStreamTick(VAGStreamHandlerPtr hand) {
    SInt32 dma_needed;
    VAGStreamHeader *stream;
    VAGStreamHeader *walk;
    SInt32 done;
    SInt32 voice[48];
    SInt32 voice_count;
    SInt32 start_with_voices;
    SInt32 sync_error;
    UInt32 load_flags;
    SInt32 dis;
    SInt32 intr_state;
    SInt32 read_sectors;
    SInt32 played_sectors;
    UInt32 diff;

    dma_needed = 0;
    start_with_voices = 0;
    sync_error = 0;
    load_flags = 0;
    if ((hand->SH.flags & 4) != 0) {
        if (!snd_StreamHandlerIsBusyWithDMA(hand) &&
            !snd_StreamHandlerIsBusyWithLoad(hand)) {
            snd_KillVAGStreamEx(hand->SH.OwnerID, 1);
        }
        return;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    load_flags = stream->flags & 0x1F800;
    walk = stream;
    done = 0;
    while (walk && !done) {
        done = snd_CheckVAGStreamProgress(walk, &dma_needed);
        walk = walk->sync_list;
    }

    if (0) {
        UInt32 hold;
        diff = 0;
        walk = stream->sync_list;
        while (walk) {
            hold = (stream->bytes_played > walk->bytes_played)
                       ? stream->bytes_played - walk->bytes_played
                       : walk->bytes_played - stream->bytes_played;

            if (hold > diff) {
                diff = hold;
            }

            walk = walk->sync_list;
        }

        if (diff > 0x320) {
            stream->ErrorAccumulator++;
            sync_error = 1;
            if (stream->ErrorAccumulator >= 5) {
                done = -2;
                snd_ShowError(0x32, hand->SH.OwnerID, 0, 0, 0);
            }
        }

        if (0) {
            walk = stream;
            while ((walk) && (!done) && !(walk->buff[0].flags & 0x100)) {
                read_sectors = walk->next_read_offset - walk->file_start_sector;
                played_sectors = walk->bytes_played / 0x800;
                if ((read_sectors - played_sectors) >
                    (walk->buff_size >> 0xA) + 1) {
                    done = -2;
                    snd_ShowError(0x33, hand->SH.OwnerID, 0, 0, 0);
                }

                walk = walk->sync_list;
            }
        }
    }

    if (!sync_error) {
        stream->ErrorAccumulator = 0;
    }

    if (done == -1) {
        if ((stream->flags & 0x400) != 0) {
            snd_RestartInterleavedStream(hand);
        } else {
            BOOL part_of_interleave = 0;
            walk = stream;
            part_of_interleave = stream->buff[0].flags & 0x100;
            voice_count = 0;
            while (walk) {
                voice[voice_count] = walk->voice;
                voice_count++;

                if ((walk->flags & 0x1000) != 0 && walk->file_handle != -1) {
                    snd_AddDeferredCloseFile(walk->file_handle, 13);
                    walk->file_handle = -1;
                }
                walk->flags = 1;

                if (part_of_interleave) {
                    snd_KeyOffVoice(walk->voice);
                    snd_MarkVoicePlaying(walk->voice);
                    gChannelStatus[walk->voice].Priority = 0;
                    walk->voice = -1;
                }
                walk = walk->sync_list;
            }

            if (hand->que_list) {
                snd_TryPreBufferQueElement(hand, load_flags);
            }

            if (hand->qued_stream) {
                hand->SH.Sound = (MIDISoundPtr)hand->qued_stream;
                walk = hand->qued_stream;
                voice_count = 0;
                while (walk) {
                    if (!part_of_interleave) {
                        walk->voice = voice[voice_count];
                    }
                    walk->flags &= ~0x10u;
                    voice_count++;

                    if ((walk->flags & 0x80) != 0) {
                        walk->flags &= ~0x80u;
                        start_with_voices = 1;
                    }

                    walk = walk->sync_list;
                }

                if (start_with_voices) {
                    snd_StartVAGStreamSounding(
                        (VAGStreamHeader *)hand->SH.Sound, 1);
                } else {
                    hand->SH.flags |= 2u;
                    snd_ContinueVAGStream(hand->SH.OwnerID);
                }

                hand->qued_stream = 0;
            } else {
                dis = CpuSuspendIntr(&intr_state);
                if (hand->doubling_voice != -1) {
                    snd_KeyOffVoice(hand->doubling_voice);
                    snd_MarkVoicePlaying(hand->doubling_voice);
                    gChannelStatus[hand->doubling_voice].Priority = 0;
                    hand->doubling_voice = -1;
                }

                for (walk = stream; walk; walk = walk->sync_list) {
                    if ((walk->flags & 0x1000) != 0 &&
                        walk->file_handle != -1) {
                        if (!dis) {
                            CpuResumeIntr(intr_state);
                        }

                        snd_AddDeferredCloseFile(walk->file_handle, 14);
                        walk->file_handle = -1;
                        dis = CpuSuspendIntr(&intr_state);
                    }

                    walk->flags = 1;
                    if (walk->voice != -1) {
                        snd_KeyOffVoice(walk->voice);
                        snd_MarkVoicePlaying(walk->voice);
                        gChannelStatus[walk->voice].Priority = 0;
                        walk->voice = -1;
                    }

                    gNumStreamsPlaying--;
                }

                if (!dis) {
                    CpuResumeIntr(intr_state);
                }

                snd_DeactivateHandler(&hand->SH, 0);
            }
        }
    } else {
        if (done == -2) {
            if (!snd_StreamHandlerIsBusyWithDMA(hand) &&
                !snd_StreamHandlerIsBusyWithLoad(hand)) {
                snd_KillVAGStreamEx(hand->SH.OwnerID, 1);
            } else {
                hand->SH.flags |= 4u;
            }

            return;
        } else {
            if (dma_needed) {
                SignalSema(gStartDMASema);
            }
        }
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    if (hand->pre_buffer_queue) {
        snd_TryPreBufferQueElement(hand, load_flags);
    }
}

SInt32 snd_CheckVAGStreamProgress(VAGStreamHeader *stream, SInt32 *dma_needed) {
    SInt32 core;
    SInt32 voice;
    UInt32 addr;
    UInt32 addr2;
    UInt32 addr3;
    UInt32 ends;
    SInt32 in_buffer;
    SInt32 done;
    SInt32 local_dma_needed;
    SInt32 state;
    SInt32 dis;

    done = 0;
    local_dma_needed = 0;
    if ((stream->flags & 8) != 0 && (stream->flags & 0x10) == 0) {
        core = stream->voice / 24;
        voice = stream->voice % 24;
        dis = CpuSuspendIntr(&state);
        addr = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
        addr2 = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
        addr3 = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
        if (!dis) {
            CpuResumeIntr(state);
        }

        if (addr != addr2 && addr2 != addr3) {
            dis = CpuSuspendIntr(&state);
            addr = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
            addr2 = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
            addr3 = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
            if (!dis) {
                CpuResumeIntr(state);
            }

            if (addr != addr2 && addr2 != addr3) {
                addr = stream->LastPosition;
            } else if (addr2 == addr3) {
                addr = addr2;
            }
        } else if (addr2 == addr3) {
            addr = addr2;
        }

        if (stream->buff[0].SPUbuff - 1 < addr &&
            addr < stream->buff[1].SPUbuff) {
            in_buffer = 0;
        } else {
            if (stream->buff[1].SPUbuff - 1 < addr &&
                addr < &stream->buff[1].SPUbuff[stream->buff_size >> 1]) {
                in_buffer = 1;
            } else {
                stream->ErrorAccumulator++;
                if (stream->ErrorAccumulator >= 9) {
                    snd_ShowError(52, stream->handler->SH.OwnerID, 0, 0, 0);
                    return -2;
                }

                if (!stream->bytes_played || !stream->LastPosition) {
                    addr = stream->buff[0].SPUbuff;
                    in_buffer = 0;
                } else {
                    snd_ShowError(53, stream->handler->SH.OwnerID, (SInt32)addr,
                                  0, 0);
                    return -1;
                }
            }
        }

        if ((stream->flags & 0x200) != 0) {
            dis = CpuSuspendIntr(&state);
            sceSdSetAddr((core | SD_VOICE(voice)) | SD_VA_LSAX,
                         (UInt32)stream->buff[1].SPUbuff);
            if (stream->handler->doubling_voice != -1) {
                sceSdSetAddr(((stream->handler->doubling_voice / 24) |
                              SD_VOICE(stream->handler->doubling_voice % 24)) |
                                 SD_VA_LSAX,
                             (UInt32)stream->buff[1].SPUbuff);
            }

            stream->flags &= ~0x200u;
            if (!dis) {
                CpuResumeIntr(state);
            }
        }

        if ((stream->flags & 0x40) == 0) {
            if (in_buffer != stream->PlayingBuffer) {

                if (!stream->LastBufferChangeTick ||
                    snd_GetTick() > stream->LastBufferChangeTick + 10) {

                    if (((stream->buff[0].flags & 0x100) == 0 ||
                         (stream->buff[0].flags & 0x200) != 0) &&
                        !snd_AddVAGStreamDMABuffer(
                            &stream->buff[stream->PlayingBuffer].flags)) {
                        done = -2;
                    } else {
                        local_dma_needed = 1;
                        stream->PlayingBuffer = in_buffer;
                        stream->LastBufferChangeTick = snd_GetTick();
                    }
                } else {
                    stream->ErrorAccumulator++;
                    if (stream->ErrorAccumulator >= 9) {
                        snd_ShowError(54, stream->handler->SH.OwnerID, 0, 0, 0);
                        return -2;
                    }
                }

            } else {
                if (0) {
                    snd_ShowError(55, stream->handler->SH.OwnerID, 0, 0, 0);
                    done = -2;
                }
            }

            stream->LastPosition = (UInt32)addr;
        } else {
            if (stream->bytes_played > 0x20) {
                core = stream->voice / 24;
                voice = stream->voice % 24;
                ends = sceSdGetParam((core | SD_VOICE(voice)) | SD_VP_ENVX);
                addr = sceSdGetAddr((core | SD_VOICE(voice)) | SD_VA_NAX);
                if (!ends) {
                    done = -1;
                }
            } else {
                stream->start_error_accumulator++;
                if (stream->start_error_accumulator > 120) {
                    printf("989snd: Error starting VAG! Killinig stream "
                           "(0x%8.8x)\n",
                           stream);
                    done = -2;
                }
            }

            stream->PlayingBuffer = in_buffer;
        }

        if (!done) {
            snd_FixVAGStreamSamplesPlayed(stream, addr);
        }
    }

    if ((stream->flags & 2) != 0 && (stream->flags & 4) != 0 &&
        (stream->flags & 0x10) == 0) {
        stream->flags &= ~2u;
        stream->flags |= 8u;
    }

    if (local_dma_needed) {
        *dma_needed = 1;
    }

    return done;
}

void snd_FixVAGStreamSamplesPlayed(VAGStreamHeader *stream, UInt32 addr) {
    SInt32 last_buffer;

    if ((UInt32)&stream->buff[1].SPUbuff[stream->buff_size >> 1] < addr) {
        addr = (UInt32)&stream->buff[1].SPUbuff[stream->buff_size >> 1];
    }

    last_buffer =
        (stream->last_loc > (&stream->buff[0].SPUbuff[0] - 1) &&
         stream->last_loc < &stream->buff[0].SPUbuff[stream->buff_size >> 1])
            ? 0
            : 1;

    if (last_buffer == stream->PlayingBuffer) {
        stream->bytes_played += addr - stream->last_loc;
    } else {
        stream->bytes_played += (UInt32)&stream->buff[last_buffer]
                                    .SPUbuff[(stream->buff_size >> 1)] -
                                stream->last_loc;
        stream->bytes_played +=
            addr - (UInt32)stream->buff[stream->PlayingBuffer].SPUbuff;
    }

    stream->last_loc = addr;
}

UInt32 snd_GetVAGStreamLoc(UInt32 handle) {
    UInt32 loc;
    UInt32 seconds;
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;

    loc = 0;
    seconds = 0;

    snd_LockMasterTick(85);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return seconds;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    if (stream->sample_rate == -1) {
        snd_UnlockMasterTick();
        return seconds;
    }

    loc = 28 * (stream->bytes_played >> 4);
    if ((stream->bytes_played & 0xF) >= 5) {
        loc += 2 * ((stream->bytes_played & 0xF) - 4);
    }

    seconds = (loc / stream->sample_rate);
    loc = loc - (seconds * stream->sample_rate);
    loc = loc << 10;
    seconds = (seconds << 10) + (loc / stream->sample_rate);

    snd_UnlockMasterTick();

    return seconds;
}

UInt32 snd_GetVAGStreamTimeRemaining(UInt32 handle) {
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;
    UInt32 current_pos;
    UInt32 total_time;

    snd_LockMasterTick(86);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return 0;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    if (stream->flags & 0x400) {
        snd_UnlockMasterTick();
        return 0;
    }

    snd_UnlockMasterTick();
    current_pos = snd_GetVAGStreamLoc(handle);
    total_time = snd_GetVAGStreamTotalTime(handle);

    return total_time - current_pos;
}

UInt32 snd_GetVAGStreamTotalTime(UInt32 handle) {
    VAGStreamHeader *stream;
    VAGStreamHandlerPtr hand;
    UInt32 seconds;
    UInt32 samples;

    seconds = 0;

    snd_LockMasterTick(87);
    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return seconds;
    }

    stream = (VAGStreamHeader *)hand->SH.Sound;
    if (stream->sample_rate == -1) {
        snd_UnlockMasterTick();
        return seconds;
    }

    samples = stream->samples_total;
    seconds = samples / stream->sample_rate;
    samples = samples % stream->sample_rate;
    seconds = (seconds << 10) + ((samples << 10) / stream->sample_rate);

    snd_UnlockMasterTick();

    return seconds;
}

SInt32 snd_IsVAGStreamBuffered(UInt32 handle) {
    VAGStreamHandlerPtr hand;

    if (!(hand = (VAGStreamHandlerPtr)snd_CheckHandlerStillActive(handle))) {
        return -1;
    }

    if ((hand->SH.flags & 8) == 0) {
        return 0;
    }

    return 1;
}

void snd_VAGStreamDataLoadThread() {
    SInt32 intr_state;
    SInt32 dis;
    SInt32 ws_ret;

    ws_ret = 0;

    while (1) {
        if (!VAGStreamLoadList && !gSSNeedSectors || gVAGReadBusy ||
            gDataReadBusy) {
            ws_ret = WaitSema(gStartDataLoadSema);
        }

        if (ws_ret) {
            snd_ShowError(56, ws_ret, 0, 0, 0);
        }

        if (gBreakDataRead && !gDataReadBusy) {
            snd_SignalDataReadDone();
        }

        dis = CpuSuspendIntr(&intr_state);
        if (!gVAGReadBusy && !gDataReadBusy) {
            if (VAGStreamLoadList) {
                while (!gVAGReadBusy && VAGStreamLoadList) {
                    gVAGReadBusy = VAGStreamLoadList;
                    VAGStreamLoadList = VAGStreamLoadList->list;
                    if (!VAGStreamLoadList) {
                        VAGStreamLoadListTail = NULL;
                    }

                    if (gVAGReadBusy &&
                        (gVAGReadBusy->owner->handler->SH.flags & 4) != 0) {
                        gVAGReadBusy = NULL;
                    }
                }

                if (gVAGReadBusy && (gVAGReadBusy->owner->flags & 0x20) == 0 &&
                    (gVAGReadBusy->owner->flags & 1) == 0) {
                    if (!dis) {
                        CpuResumeIntr(intr_state);
                    }

                    snd_KickVAGRead(0);
                    dis = CpuSuspendIntr(&intr_state);
                } else {
                    gVAGReadBusy = 0;
                }
            } else if (gSSNeedSectors) {
                gDataReadBusy = 1;
                if (!dis) {
                    CpuResumeIntr(intr_state);
                }

                snd_KickDataRead();
                dis = CpuSuspendIntr(&intr_state);
            } else if (gSearchFileName) {
                if (!dis) {
                    CpuResumeIntr(intr_state);
                }

                gSearchFileResult =
                    sceCdSearchFile(gSearchFileParams, gSearchFileName);
                gSearchFileName = 0;
                gSearchFileParams = 0;
                SignalSema(gSearchFileSema);
                dis = CpuSuspendIntr(&intr_state);
            }
        }

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        if (!gVAGReadBusy && !gDataReadBusy && !VAGStreamLoadList) {
            if (gCDIdleWaiter) {
                WakeupThread(gCDIdleWaiter);
                gCDIdleWaiter = 0;
            }
        }
    }
}

SInt32 snd_StreamSafeCdRead(UInt32 lbn, UInt32 sectors, void *buf) {
    SInt32 intr_state;
    SInt32 dis;
    sceCdRMode mode;
    static SInt32 in_func = 0;

    in_func++;
    if (in_func >= 2) {
        in_func--;
        return 0;
    }

    if (gShuttingDownStreaming) {
        gLastDATAReadError = 19;
        in_func--;
        return 0;
    }

    if (!VAGStreamList) {
        mode.trycount = 100;
        mode.spindlctrl = gCdSpinMode;
        mode.datapattern = 0;
        in_func--;
        return sceCdRead(lbn, sectors, buf, &mode);
    }

    if (snd_StreamSafeCdSync(17) == 1) {
        gLastDATAReadError = 19;
        in_func--;
        return 0;
    }

    dis = CpuSuspendIntr(&intr_state);
    gReadWaitThread = -1;
    gCurrentReadBuffer = buf;
    gSSReadLoc = lbn;
    gReadIsEE = 0;
    gSSNeedSectors = sectors;
    gBreakDataRead = 0;
    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    SignalSema(gStartDataLoadSema);
    in_func--;

    return 1;
}

SInt32 snd_StreamSafeCdBreak() {
    if (!VAGStreamList) {
        return sceCdBreak();
    }

    if (!gSSNeedSectors) {
        return 1;
    }

    gBreakDataRead = 1;
    return 1;
}

SInt32 snd_StreamSafeCdSearchFile(sceCdlFILE *fp, char *name) {
    SInt32 dis;
    SInt32 intr_state;

    if (gShuttingDownStreaming) {
        gLastDATAReadError = 19;
        return 0;
    }

    if (!VAGStreamList) {
        snd_ShowError(57, 0, 0, 0, 0);
        snd_SetCDSifReg(0, -1);
        gLastDATAReadError = -1;
        return 0;
    }

    if (!gMasterReadBuffer) {
        snd_ShowError(58, 0, 0, 0, 0);
        snd_SetCDSifReg(0, -1);
        gLastDATAReadError = -1;
        return 0;
    }

    if (snd_StreamSafeCdSync(17) == 1) {
        snd_ShowError(59, 0, 0, 0, 0);
        snd_SetCDSifReg(1, 19);
        gLastDATAReadError = 19;
        return 0;
    }

    dis = CpuSuspendIntr(&intr_state);
    gSearchFileName = name;
    gSearchFileParams = fp;
    gSearchFileResult = 0;
    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    SignalSema(gStartDataLoadSema);
    WaitSema(gSearchFileSema);
    return gSearchFileResult;
}

SInt32 snd_StreamSafeCdReadEEm(UInt32 lbn, UInt32 sectors, void *buf) {
    SInt32 intr_state;
    SInt32 dis;
    static SInt32 in_func = 0;

    in_func++;
    if (in_func >= 2) {
        in_func--;
        return 0;
    }

    if (gShuttingDownStreaming) {
        gLastDATAReadError = 19;
        in_func--;
        return 0;
    }

    if (!VAGStreamList) {
        snd_ShowError(57, 0, 0, 0, 0);
        snd_SetCDSifReg(0, -1);
        gLastDATAReadError = -1;
        in_func--;
        return 0;
    }

    if (!gMasterReadBuffer) {
        snd_ShowError(58, 0, 0, 0, 0);
        snd_SetCDSifReg(0, -1);
        gLastDATAReadError = -1;
        in_func--;
        return 0;
    }

    if (snd_StreamSafeCdSync(17) == 1) {
        snd_ShowError(59, 0, 0, 0, 0);
        snd_SetCDSifReg(1, 19);
        gLastDATAReadError = 19;
        in_func--;
        return 0;
    }

    if (!sectors) {
        snd_ShowError(94, 0, 0, 0, 0);
        snd_SetCDSifReg(0, 34);
        gLastDATAReadError = 34;
        in_func--;
        return 0;
    }

    dis = CpuSuspendIntr(&intr_state);
    gReadWaitThread = -1;
    gCurrentReadBuffer = buf;
    gSSReadLoc = lbn;
    gReadIsEE = 1;
    gSSNeedSectors = sectors;
    gBreakDataRead = 0;
    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    SignalSema(gStartDataLoadSema);
    in_func--;

    return 1;
}

SInt32 snd_StreamSafeCdSync(SInt32 mode) {
    if (!VAGStreamList) {
        return sceCdSync(mode);
    }

    if (!gSSNeedSectors && !gDataReadBusy) {
        return 0;
    }

    if (mode == 1 || mode == 17) {
        return 1;
    }

    if (gReadWaitThread != -1 && (!mode || mode == 16)) {
        snd_ShowError(60, 0, 0, 0, 0);
        return 1;
    }

    while (gSSNeedSectors || gDataReadBusy) {
        gReadWaitThread = GetThreadId();
        SleepThread();
        gReadWaitThread = -1;
    }

    return 0;
}

SInt32 snd_StreamSafeCdGetError() {
    if (!VAGStreamList) {
        return sceCdGetError();
    }

    return gLastDATAReadError;
}

void snd_KickDataRead() {
    sceCdRMode mode;
    SInt32 ret_code;

    gSSReadSectors = gSSNeedSectors;
    if (gHalfBufferSize / 2048 < gSSReadSectors) {
        gSSReadSectors = gHalfBufferSize / 2048;
    }

    gMasterReadBufferFirstSector = gSSReadLoc;
    gMasterReadBufferLastSector = gSSReadLoc + (gSSReadSectors - 1);
    sceCdSync(16);
    sceCdCallback(snd_DoneVAGReadCB);
    mode.trycount = 50;
    mode.spindlctrl = gCdSpinMode;
    mode.datapattern = 0;

    if (!gReadIsEE) {
        gLastReadSector = gSSReadLoc;
        gLastReadAmount = gSSReadSectors;
        gLastReadIOPLoc = (UInt32)gCurrentReadBuffer;
        if (gCurrentReadBuffer == gFileLoadBuffer && gSSReadSectors >= 2) {
            gLastDATAReadError = -1;
            snd_ShowError(102, gSSReadSectors, 0, 0, 0);
            gDataReadBusy = 0;
            gSSNeedSectors = 0;
            if (gReadWaitThread != -1) {
                SInt32 th_id = gReadWaitThread;
                gReadWaitThread = -1;
                WakeupThread(th_id);
            }

            return;
        }

        gCdBusy = 2;
        gCdBusyTime = 0;
        ret_code =
            sceCdRead(gSSReadLoc, gSSReadSectors, gCurrentReadBuffer, &mode);
    } else {
        gLastReadSector = gSSReadLoc;
        gLastReadAmount = gSSReadSectors;
        gLastReadIOPLoc = (UInt32)gMasterReadBuffer;
        gCdBusy = 2;
        gCdBusyTime = 0;
        ret_code =
            sceCdRead(gSSReadLoc, gSSReadSectors, gMasterReadBuffer, &mode);
    }

    if (!ret_code) {
        gCdBusy = 0;
        gLastDATAReadError = sceCdGetError();
        if (!gLastDATAReadError) {
            gLastDATAReadError = -1;
            if (!gPrefs_Silent) {
                printf("snd_KickDataRead: sceCdRead returned 0 but "
                       "sceCdGetError returned SCECdErNo!\n");
            }
        }

        if (!gReadIsEE) {
            snd_ShowError(61, gLastDATAReadError, gSSReadLoc, gSSReadSectors,
                          (SInt32)gCurrentReadBuffer);
        } else {
            snd_ShowError(61, gLastDATAReadError, gSSReadLoc,
                          gMasterReadBufferSize, (SInt32)gMasterReadBuffer);
        }

        gDataReadBusy = 0;
        gSSNeedSectors = 0;
        if (gReadWaitThread != -1) {
            SInt32 th_id = gReadWaitThread;
            gReadWaitThread = -1;
            WakeupThread(th_id);
        }

        if (gReadIsEE) {
            snd_SetCDSifReg(0, gLastDATAReadError);
        }
    }
}

void snd_KickVAGRead(SInt32 offset) {
    SInt32 readbytes;
    UInt32 readsectors;

    if (!gVAGReadBusy) {
        return;
    }

    if ((gVAGReadBusy->owner->handler->SH.flags & 4) != 0) {
        gVAGReadBusy = 0;
        return;
    }

    if ((gVAGReadBusy->flags & 0x10) != 0) {
        if (gVAGReadBusy->owner->file_start_offset > 0x7D0) {
            readbytes = 4096;
            readsectors = 2;
        } else {
            readbytes = 2048;
            readsectors = 1;
        }
    } else {
        readbytes = ((gVAGReadBusy->owner->buff_size >> 1) - offset);
        readbytes = readbytes - (readbytes % 2048);

        if (readbytes >= gVAGReadBusy->owner->bytes_remaining) {
            if ((gVAGReadBusy->flags & 0x100) == 0) {
                readbytes = gVAGReadBusy->owner->bytes_remaining;
            }

            gVAGReadBusy->bytes = gVAGReadBusy->owner->bytes_remaining + offset;
            gVAGReadBusy->owner->bytes_remaining = 0;
            gVAGReadBusy->owner->flags |= 0x20u;
            gVAGReadBusy->is_end = 1;
        } else {
            gVAGReadBusy->is_end = 0;
            gVAGReadBusy->bytes = readbytes + offset;
            gVAGReadBusy->owner->bytes_remaining -= readbytes;
        }

        if ((gVAGReadBusy->flags & 0x100) != 0) {
            readsectors = ((gVAGReadBusy->owner->buff_size >> 1) *
                               gVAGReadBusy->owner->handler->num_streams +
                           2047) >>
                          11;
        } else {
            readsectors = (readbytes + 2047) / 2048;
        }
    }

    if ((gVAGReadBusy->owner->flags & 0x800) != 0) {
        snd_GetStreamDataFromIOPMemory(offset, readbytes, readsectors);
    } else if ((gVAGReadBusy->owner->flags & 0x2000) != 0) {
        snd_GetStreamDataFromEEMemory(offset, readbytes, readsectors);
    } else if ((gVAGReadBusy->owner->flags & 0x1C000) != 0) {
        snd_GetStreamDataFromMemoryStream(offset, readbytes, readsectors,
                                          gVAGReadBusy->owner->flags);
    } else if ((gVAGReadBusy->owner->flags & 0x1000) != 0) {
        snd_GetStreamDataFromStdio(offset, readbytes, readsectors);
    } else {
        snd_GetStreamDataFromCD(offset, readbytes, readsectors);
    }
}

// TODO, verify variable names
SInt32 snd_GetStreamDataFromIOPMemory(SInt32 offset, SInt32 readbytes,
                                      UInt32 readsectors) {
    SInt32 ret_code;

    ret_code = 0;

    if ((gVAGReadBusy->owner->flags & 0x400) != 0 &&
        (gVAGReadBusy->flags & 0x100) == 0) {
        char *src = (char *)(gVAGReadBusy->owner->file_start_sector +
                             ((gVAGReadBusy->owner->next_read_offset -
                               gVAGReadBusy->owner->file_start_sector) *
                              2048));

        memcpy(&gVAGReadBusy->IOPbuff[offset], src, readsectors * 2048);
    } else if ((gVAGReadBusy->flags & 0x100) == 0) {
        gVAGReadBusy->IOPbuff =
            (SInt8 *)(gVAGReadBusy->owner->file_start_sector +
                      ((gVAGReadBusy->owner->next_read_offset -
                        gVAGReadBusy->owner->file_start_sector) *
                       2048));
        if ((gVAGReadBusy->flags & 0x10) == 0) {
            int ii;
            int back;

            back = gVAGReadBusy->bytes;
            for (ii = 0; ii < 5 && back >= 16; ++ii) {
                memcpy(&gVAGReadBusy->OrigIOPbuff[16 * ii],
                       &gVAGReadBusy->IOPbuff[16 * ii], 0x10u);
                back -= 16;
            }

            if (gVAGReadBusy->bytes >= 0x21) {
                memcpy(&gVAGReadBusy->OrigIOPbuff[gVAGReadBusy->bytes] - 32,
                       &gVAGReadBusy->IOPbuff[gVAGReadBusy->bytes] - 32, 0x20u);
            }
        }
    } else {
        VAGStreamHeader *walk;
        VAGStreamHeader *master;
        int i;
        int ii;
        int back;
        u_int c;

        master = walk = gVAGReadBusy->owner;
        i = gVAGReadBusy != walk->buff;
        c = 0;
        while (walk) {
            if (walk == master) {
                walk->buff[i].IOPbuff = (SInt8 *)(master->file_start_sector +
                                                  ((master->next_read_offset -
                                                    master->file_start_sector) *
                                                   2048));
            } else {
                walk->buff[i].IOPbuff = &master->buff[i].IOPbuff[c];
            }

            if ((gVAGReadBusy->flags & 0x10) == 0) {
                back = gVAGReadBusy->bytes;
                for (ii = 0; ii < 5 && back >= 16; ++ii) {
                    memcpy(&walk->buff[i].OrigIOPbuff[16 * ii],
                           &walk->buff[i].IOPbuff[16 * ii], 0x10u);
                    back -= 16;
                }

                if (gVAGReadBusy->bytes >= 0x21) {
                    memcpy(&walk->buff[i].OrigIOPbuff[gVAGReadBusy->bytes] - 32,
                           &walk->buff[i].IOPbuff[gVAGReadBusy->bytes] - 32,
                           0x20u);
                }
            }

            c += walk->buff_size >> 1;
            walk = walk->sync_list;
        }
    }

    gLastVAGReadError = 0;
    ret_code = 1;
    if ((gVAGReadBusy->flags & 0x10) == 0) {
        gVAGReadBusy->owner->next_read_offset += readsectors;
    }

    SignalSema(gDoneLoadSema);

    return ret_code;
}

// INCLUDE_ASM("asm/nonmatchings/stream", snd_GetStreamDataFromEEMemory);
SInt32 snd_GetStreamDataFromEEMemory(SInt32 offset, SInt32 readbytes,
                                     UInt32 readsectors) {
    sceSifReceiveData data_track;
    char *ee_src;
    SInt32 ret_code;

    ret_code = 0;
    ee_src = (char *)(gVAGReadBusy->owner->file_start_sector +
                      ((gVAGReadBusy->owner->next_read_offset -
                        gVAGReadBusy->owner->file_start_sector) *
                       2048));

    if ((gVAGReadBusy->flags & 0x100) != 0) {
        ret_code = sceSifGetOtherData(&data_track, ee_src,
                                      &gVAGReadBusy->IOPbuff[offset],
                                      readsectors * 2048, 0);
    } else {
        ret_code = sceSifGetOtherData(
            &data_track, ee_src, &gVAGReadBusy->IOPbuff[offset], readbytes, 0);
    }

    if (ret_code < 0) {
        gLastVAGReadError = -1;
        snd_ShowError(110, 0, 0, 0, 0);
        gVAGReadBusy->owner->handler->SH.flags |= 4u;
        gVAGReadBusy = 0;
    } else {
        gLastVAGReadError = 0;
        ret_code = 1;
        if ((gVAGReadBusy->flags & 0x10) == 0) {
            gVAGReadBusy->owner->next_read_offset += readsectors;
        }

        SignalSema(gDoneLoadSema);
    }

    return ret_code;
}

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
