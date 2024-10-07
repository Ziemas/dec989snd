#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libcdvd.h"
#include "stdio.h"
#include "thread.h"
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

static int in_48 = 0;
static int in_func_76 = 0;
static int in_func_86 = 0;

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
