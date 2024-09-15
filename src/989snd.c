#include "common.h"
#include "functions.h"
#include "globals.h"

//INCLUDE_RODATA("asm/data/", 989snd);

/* data 0 */ UInt16 g989Version = 0x301;
/* data 4 */ ModuleInfo Module  = {"989snd_Library", 0x301};
/* data c */ Extern989MonitorInfo *g989Monitor = NULL;
/* data 10 */ SInt32 gThreadPriority_TICK = 0x9;
/* data 14 */ SInt32 gThreadPriority_RPC = 0x3a;
/* data 18 */ SInt32 gThreadPriority_STREAM = 0x3c;
/* data 1c */ SInt32 gMainRPCThreadId = 0;
/* data 20 */ SInt32 gEEDMADoneSema = 0;
/* data 24 */ SInt32 gFileReadMutex = 0;
/* data 28 */ BOOL gDoRPC = true;
/* data 2c */ SInt8 *snd_MESSAGE_RECIEVE_BUFFER = NULL;
/* data 30 */ UInt32 *snd_MESSAGE_RETURN_BUFFER = NULL;
/* data 34 */ UInt32 *gWriteBackdataOffset = NULL;
/* data 38 */ Extern989HandlerPtr gExternHandlersList = NULL;
/* data 3c */ CommandHandler gCommandFunc[] = {
    snd_CMD_SL_INIT,
    snd_CMD_SL_CLOSE,
    snd_CMD_SL_LOADBANK,
    snd_CMD_SL_LOADBANKBYLOC,
    snd_CMD_SL_LOADMMD,
    snd_CMD_SL_LOADMMDBYLOC,
    snd_CMD_SL_UNLOADBANK_A,
    snd_CMD_SL_UNLOADMIDI_A,
    snd_CMD_SL_RESOLVEBANKS,
    snd_CMD_SL_SETMASTERVOL_A,
    snd_CMD_SL_GETMASTERVOL,
    snd_CMD_SL_SETPLAYBACKMODE_A,
    snd_CMD_SL_GETPLAYBACKMODE,
    snd_CMD_SL_SETMIXERMODE_A,
    snd_CMD_SL_SETREVERBTYPE,
    snd_CMD_SL_SETREVERBDEPTH,
    snd_CMD_SL_AUTOVERB,
    snd_CMD_SL_PLAYSOUND,
    snd_CMD_SL_PLAYSOUND_A,
    snd_CMD_SL_PAUSESOUND,
    snd_CMD_SL_CONTINUESOUND,
    snd_CMD_SL_STOPSOUND,
    snd_CMD_SL_PAUSEALLSOUNDS,
    snd_CMD_SL_CONTINUEALLSOUNDS,
    snd_CMD_SL_STOPALLSOUNDS,
    snd_CMD_SL_SOUNDISSTILLPLAYING,
    snd_CMD_SL_ISSOUNDALOOPER,
    snd_CMD_SL_SETSOUNDVOLPAN_A,
    snd_CMD_SL_GETSOUNDORIGPITCH,
    snd_CMD_SL_GETSOUNDCURRPITCH,
    snd_CMD_SL_SETSOUNDPITCH_A,
    snd_CMD_SL_SETSOUNDPITCHBEND_A,
    snd_CMD_SL_SETSOUNDPITCHMOD_A,
    snd_CMD_SL_SETSOUNDPARAMS,
    snd_CMD_SL_AUTOVOL_A,
    snd_CMD_SL_AUTOPAN_A,
    snd_CMD_SL_AUTOPITCH_A,
    snd_CMD_SL_AUTOPITCHBEND_A,
    snd_CMD_SL_GETGLOBALEXCITE,
    snd_CMD_SL_SETGLOBALEXCITE_A,
    snd_CMD_SL_GETMIDIREGISTER,
    snd_CMD_SL_SETMIDIREGISTER_A,
    snd_CMD_SL_INITVAGSTREAMINGEX,
    snd_CMD_SL_PLAYVAGSTREAMBYNAME,
    snd_CMD_SL_PLAYVAGSTREAMBYLOC,
    snd_CMD_SL_PAUSEVAGSTREAM,
    snd_CMD_SL_CONTINUEVAGSTREAM,
    snd_CMD_SL_STOPVAGSTREAM,
    snd_CMD_SL_GETVAGSTREAMQUEUECOUNT,
    snd_CMD_SL_GETVAGSTREAMLOC,
    snd_CMD_SL_GETVAGSTREAMTIMEREMAINING,
    snd_CMD_SL_GETVAGSTREAMTOTALTIME,
    snd_CMD_SL_STOPALLVAGSTREAMS,
    snd_CMD_SL_CLOSEVAGSTREAMING,
    snd_CMD_SL_STREAMSAFECHECKCDIDLE,
    snd_CMD_SL_STREAMSAFECDBREAK,
    snd_CMD_SL_STREAMSAFECDREAD,
    snd_CMD_SL_ALLOCIOPRAM,
    snd_CMD_SL_FREEIOPRAM,
    snd_CMD_SL_INITMOVIESOUND,
    snd_CMD_SL_CLOSEMOVIESOUND,
    snd_CMD_SL_RESETMOVIESOUND,
    snd_CMD_SL_STARTMOVIESOUND,
    snd_CMD_SL_PAUSEMOVIESOUND,
    snd_CMD_SL_GETTRANSSTATUS,
    snd_CMD_SL_GETSTICK,
    snd_CMD_SL_GETVOICEALLOC,
    snd_CMD_SL_LOCKVALLOC,
    snd_CMD_SL_EXTERNVALLOC,
    snd_CMD_SL_EXTERNVFREE,
    snd_CMD_SL_UNLOCKVALLOC,
    snd_CMD_SL_SRAMMALLOC,
    snd_CMD_SL_SRAMMARKUSED,
    snd_CMD_SL_SRAMFREE,
    snd_CMD_SL_SRAMGETFREE,
    snd_CMD_SL_SRAMMAXFREE,
    snd_CMD_SL_EXTERNCALL,
    snd_CMD_SL_COMMAND_BATCH,
    snd_CMD_SL_SETGROUPVOICERANGE,
    snd_CMD_SL_ISVAGSTREAMBUFFERED,
    snd_CMD_SL_SETREVERBEX,
    snd_CMD_SL_PREALLOCREVERBWORKAREA,
    snd_CMD_SL_GETLASTLOADERROR,
    snd_CMD_SL_SETPANMODE,
    snd_CMD_SL_PLAYSOUNDBYNAME,
    snd_CMD_SL_PLAYSOUNDBYNAME_A,
    snd_CMD_SL_GETSOUNDUD,
    snd_CMD_SL_BANKLOADFROMEE,
    snd_CMD_SL_SETMOVIESOUNDVOLPAN,
    snd_CMD_SL_BANKLOADFROMIOP,
    snd_CMD_SL_UPDATEMOVIEADPCM,
    snd_CMD_SL_GETMOVIENAX,
    snd_CMD_SL_PLAYSOUNDEX,
    snd_CMD_SL_GETSOUNDREG,
    snd_CMD_SL_SETSOUNDREG,
    snd_CMD_SL_SETALLSOUNDREG,
    snd_CMD_SL_SETMASTERVOLUMEDUCKER,
    snd_CMD_SL_STOPALLSOUNDSINGROUP,
    snd_CMD_SL_SETVAGSTREAMCHANNELRANGE,
    snd_CMD_SL_SETVAGSTREAMSUBGROUPVOLPAN,
    snd_CMD_SL_SETREVERBMODE,
    snd_CMD_SL_CDSTATUS,
    snd_CMD_SL_GETSFXGLOBALREG,
    snd_CMD_SL_SETSFXGLOBALREG,
    snd_CMD_SL_EXTERNCALLWITHDATA,
    snd_CMD_SL_STREAMSAFECDSEARCHFILE,
};
/* data 1e4 */ SndErrorDisplayFunc gErrorDisplayFunc = NULL;
/* bss 0 */ SInt8 snd_LOADER_MESSAGE_RECIEVE_BUFFER[256];
/* bss 100 */ UInt32 snd_LOADER_MESSAGE_RETURN_BUFFER[4];

BOOL snd_RegisterExternProcHandler(Extern989HandlerPtr hand) {
    Extern989HandlerPtr work;

    if (snd_FindExternProcHandler(hand->proc_id)) {
        snd_ShowError(1, hand->proc_id, hand->proc_id, 0, 0);
        return false;
    }

    hand->reserved1 = 0;
    hand->reserved2 = 0;

    if (gExternHandlersList == NULL) {
        gExternHandlersList = hand;
    } else {
        work = gExternHandlersList;
        while (work->reserved1 != 0) {
            work = (Extern989HandlerPtr)work->reserved1;
        }
        work->reserved1 = (UInt32)hand;
    }

    return true;
}

BOOL snd_UnregisterExternProcHandler(Extern989HandlerPtr hand) {
    Extern989HandlerPtr work;

    if (!snd_FindExternProcHandler(hand->proc_id)) {
        snd_ShowError(2, hand->proc_id, hand->proc_id, 0, 0);
        return false;
    }

    if (gExternHandlersList == hand) {
        gExternHandlersList = (Extern989HandlerPtr)hand->reserved1;
    } else {
        work = gExternHandlersList;
        while (work && work->reserved1 != (UInt32)hand) {
            work = (Extern989HandlerPtr)work->reserved1;
        }
        work->reserved1 = (UInt32)((Extern989HandlerPtr)work->reserved1)->reserved1;
    }

    return true;
}

Extern989HandlerPtr snd_FindExternProcHandler(UInt32 id) {
    Extern989HandlerPtr work;
    
    for (work = gExternHandlersList; work; work = (Extern989HandlerPtr)work->reserved1) {
        if (work->proc_id == id) {
            return work;
        }
    }

    return work;
}

SInt32 snd_DoExternCall(UInt32 proc_id, SInt32 func_index, SInt32 arg1, SInt32 arg2, SInt32 arg3, SInt32 arg4, SInt32 arg5) {
    Extern989HandlerPtr work;

    work = snd_FindExternProcHandler(proc_id);
    if (work == NULL) {
        snd_ShowError(3, proc_id, proc_id, 0, 0);
        return 0;
    }

    if (func_index >= work->num_funcs) {
        snd_ShowError(4, proc_id, 0, 0, 0);
        return 0;
    }

    return work->procs[func_index](arg1, arg2, arg3, arg4, arg5);
}

SInt32 snd_DoExternCallWithData(UInt32 proc_id, SInt32 func_index, SInt32 data_size, void *data_ptr) {
    Extern989HandlerPtr work;

    work = snd_FindExternProcHandler(proc_id);
    if (work == NULL) {
        snd_ShowError(3, proc_id, proc_id, 0, 0);
        return 0;
    }

    if (func_index >= work->num_funcs) {
        snd_ShowError(4, proc_id, 0, 0, 0);
        return 0;
    }

    return work->procs[func_index](data_size, (SInt32)data_ptr, 0, 0, 0);
}

void snd_CMD_SL_INIT(SInt8 *msg_data) {
    UInt32 *data;

    data = (UInt32*)msg_data;
    gEEStatusAddr = (char *)(*(UInt32 *)msg_data);
    snd_StartSoundSystemEx(data[1]);
}

void snd_CMD_SL_CLOSE(SInt8 *msg_data) {
    snd_StopSoundSystem();
}

void snd_CMD_SL_LOADBANK(SInt8 *msg_data) {
    *gWriteBackdataOffset = (UInt32)snd_BankLoadEx(&msg_data[4], *(SInt32 *)msg_data, 0, 0);
}

void snd_CMD_SL_LOADBANKBYLOC(SInt8 *msg_data) {
    SInt32 *data;

    data = (SInt32 *)msg_data;
    *gWriteBackdataOffset = (UInt32)snd_BankLoadByLocEx(data[0], data[1], 0, 0);
}

void snd_CMD_SL_BANKLOADFROMEE(SInt8 *msg_data) {
    UInt32 *data;

    data = (SInt32 *)msg_data;
    *gWriteBackdataOffset = (UInt32)snd_BankLoadFromEEEx(data[0], 0, 0);
}

void snd_CMD_SL_BANKLOADFROMIOP(SInt8 *msg_data) {
    UInt32 *data;

    data = (SInt32 *)msg_data;
    *gWriteBackdataOffset = (UInt32)snd_BankLoadFromIOPEx((void *)data[0], 0, 0);
}

void snd_CMD_SL_LOADMMD(SInt8 *msg_data) {
    *gWriteBackdataOffset = (UInt32)snd_MMDLoad((char *)&msg_data[4], *(SInt32 *)msg_data);
}

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_LOADMMDBYLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_UNLOADBANK_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_UNLOADMIDI_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_RESOLVEBANKS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETMASTERVOL_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETMASTERVOL);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETPLAYBACKMODE_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETPLAYBACKMODE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETMIXERMODE_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETREVERBMODE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETGROUPVOICERANGE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETREVERBTYPE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETREVERBDEPTH);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_AUTOVERB);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYSOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYSOUND_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYSOUNDEX);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PAUSESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CONTINUESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STOPSOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PAUSEALLSOUNDS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CONTINUEALLSOUNDS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STOPALLSOUNDS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STOPALLSOUNDSINGROUP);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SOUNDISSTILLPLAYING);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_ISSOUNDALOOPER);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDVOLPAN_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSOUNDORIGPITCH);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSOUNDCURRPITCH);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDPITCH_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDPITCHBEND_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDPITCHMOD_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDPARAMS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_AUTOVOL_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_AUTOPAN_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_AUTOPITCH_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_AUTOPITCHBEND_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETGLOBALEXCITE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETGLOBALEXCITE_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETMIDIREGISTER);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETMIDIREGISTER_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_INITVAGSTREAMINGEX);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETVAGSTREAMCHANNELRANGE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETVAGSTREAMSUBGROUPVOLPAN);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYVAGSTREAMBYNAME);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYVAGSTREAMBYLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PAUSEVAGSTREAM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CONTINUEVAGSTREAM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STOPVAGSTREAM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETVAGSTREAMQUEUECOUNT);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETVAGSTREAMLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETVAGSTREAMTIMEREMAINING);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETVAGSTREAMTOTALTIME);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_ISVAGSTREAMBUFFERED);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STOPALLVAGSTREAMS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CLOSEVAGSTREAMING);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STREAMSAFECHECKCDIDLE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STREAMSAFECDBREAK);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STREAMSAFECDREAD);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STREAMSAFECDSEARCHFILE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_ALLOCIOPRAM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_FREEIOPRAM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_INITMOVIESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CLOSEMOVIESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_RESETMOVIESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_STARTMOVIESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PAUSEMOVIESOUND);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETMOVIESOUNDVOLPAN);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_UPDATEMOVIEADPCM);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETMOVIENAX);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETTRANSSTATUS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSTICK);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETVOICEALLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_LOCKVALLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_EXTERNVALLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_EXTERNVFREE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_UNLOCKVALLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SRAMMALLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SRAMMARKUSED);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SRAMFREE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SRAMGETFREE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SRAMMAXFREE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_EXTERNCALL);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_EXTERNCALLWITHDATA);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETREVERBEX);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PREALLOCREVERBWORKAREA);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETLASTLOADERROR);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETPANMODE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYSOUNDBYNAME);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_PLAYSOUNDBYNAME_A);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSOUNDUD);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSOUNDREG);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSOUNDREG);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_GETSFXGLOBALREG);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETSFXGLOBALREG);

void snd_CMD_SL_SETALLSOUNDREG(SInt8 *msg_data) {
    UInt32 *data;

    data = (UInt32 *)msg_data;
    snd_SetAllSoundReg(data[0], &data[1]);
}

void snd_CMD_SL_SETMASTERVOLUMEDUCKER(SInt8 *msg_data) {
    SInt32 *data;
    DuckerDefPtr state;

    data = (SInt32 *)msg_data;
    state = (DuckerDefPtr)&data[1];
    if (state->source_group == -1) {
        state = NULL;
    }

    snd_SetMasterVolumeDucker(*data, state);
}

void snd_CMD_SL_CDSTATUS(SInt8 *msg_data) {
    *gWriteBackdataOffset = sceCdStatus();
}

void snd_CMD_SL_COMMAND_BATCH(SInt8 *msg_data) {
    SndCommandBufferPtr batch = (SndCommandBufferPtr)msg_data;
    char *command_buffer_walk;
    SInt32 x;
    SInt16 command;
    SInt16 size;

    command_buffer_walk = batch->buffer;
    for (x = 0; x < batch->num_commands; x++) {
        command = *(SInt16 *)command_buffer_walk;
        command_buffer_walk += 2;
        size = *(SInt16 *)command_buffer_walk;
        command_buffer_walk += 2;

        if ((UInt16)size & 3) {
            size = ((UInt16)size + 4) - (size % 4);
        }

        gCommandFunc[command](command_buffer_walk);
        command_buffer_walk += size;
        gWriteBackdataOffset++;
    }

    gWriteBackdataOffset--;
}

static void* snd_EEMessageParser(UInt32 command, void *data, SInt32 size) {
    gWriteBackdataOffset = snd_MESSAGE_RETURN_BUFFER + 1;
    gCommandFunc[command](data);
    gWriteBackdataOffset[1] = -1;
    return snd_MESSAGE_RETURN_BUFFER;
}

SInt32 snd_StartEEMessaging() {
    sceSifQueueData qd;
    sceSifServeData sd;

    *(SInt32 *)snd_MESSAGE_RETURN_BUFFER = -1;
    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, 0x123456, snd_EEMessageParser, snd_MESSAGE_RECIEVE_BUFFER, NULL, NULL, &qd);
    sceSifRpcLoop(&qd);

    return 0;
}

static void* snd_EELoaderMessageParser(UInt32 command, void *data, SInt32 size) {
    SInt32 ret_val = 0;
    
    switch (command) {
    case SL_LOADBANK:
        ret_val = (SInt32)snd_BankLoadEx((char *)&data[4], *(SInt32 *)&data[0], 0, 0);
        break;
    case SL_LOADBANKBYLOC:
        ret_val = (SInt32)snd_BankLoadByLocEx(*(SInt32 *)&data[0], *(SInt32 *)&data[4], 0, 0);
        break;
    case SL_LOADMMD:
        ret_val = (SInt32)snd_MMDLoad((char *)&data[4], *(SInt32 *)data);
        break;
    case SL_LOADMMDBYLOC:
        ret_val = (SInt32)snd_MMDLoadByLoc(*(SInt32 *)&data[0], *(SInt32 *)&data[4]);
        break;
    case SL_BANKLOADFROMEE:
        ret_val = (SInt32)snd_BankLoadFromEEEx(*(SInt32 *)&data[0], 0, 0);
        break;
    case SL_BANKLOADFROMIOP:
        ret_val = (SInt32)snd_BankLoadFromIOPEx(*(void **)&data[0], 0, 0);
        break;
    }

    snd_LOADER_MESSAGE_RETURN_BUFFER[0] = ret_val;
    return snd_LOADER_MESSAGE_RETURN_BUFFER;
}

SInt32 snd_StartEELoaderMessaging() {
    sceSifQueueData qd;
    sceSifServeData sd;

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, 0x123457, snd_EELoaderMessageParser, snd_LOADER_MESSAGE_RECIEVE_BUFFER, NULL, NULL, &qd);
    sceSifRpcLoop(&qd);

    return 0;
}

SInt32 start(SInt32 argc, SInt8 **argv) {
    struct SemaParam sema;
    struct ThreadParam param;
    SInt32 th, x, dis, oldstat;
    int i;
    char *walk;

    for (x = 1; x < argc; x++) {
        snd_ParseCommandLineArg(argv[x]);
    }

    InitResidentModule();
    snd_DumpVersionAndInfo();
    if (!sceSifCheckInit()) {
        sceSifInit();
    }
    sceSifInitRpc(0);

    if (gDoRPC) {
        dis = CpuSuspendIntr(&oldstat);
        snd_MESSAGE_RECIEVE_BUFFER = AllocSysMemory(0, 0x1000, 0);
        if (!snd_MESSAGE_RECIEVE_BUFFER) {
            snd_ShowError(5, 0, 0, 0, 0);
            return 1;
        }

        walk = snd_MESSAGE_RECIEVE_BUFFER;
        for (i = 0; i < 0x1000U; i++) {
            walk[i] = -85;
        }

        snd_MESSAGE_RETURN_BUFFER = AllocSysMemory(0, 0x420, 0);
        if (!snd_MESSAGE_RETURN_BUFFER) {
            snd_ShowError(6, 0, 0, 0, 0);
            return 1;
        }

        if (!dis) {
            CpuResumeIntr(oldstat);
        }

        gWriteBackdataOffset = snd_MESSAGE_RETURN_BUFFER + 1;

        sema.attr = SA_THFIFO;
        sema.initCount = 0;
        sema.maxCount = 1;

        gEEDMADoneSema = CreateSema(&sema);
        gFileReadMutex = CreateSema(&sema);
        SignalSema(gFileReadMutex);

        param.attr = TH_C;
        param.entry = snd_StartEEMessaging;
        param.initPriority = gThreadPriority_RPC;
        param.stackSize = 0x1000;
        param.option = 0;

        gMainRPCThreadId = CreateThread(&param);
        if (gMainRPCThreadId <= 0) {
            return 1;
        }

        StartThread(gMainRPCThreadId, 0);

        param.attr = TH_C;
        param.entry = snd_StartEELoaderMessaging;
        param.initPriority = gThreadPriority_RPC + 1;
        param.stackSize = 0x800;
        param.option = 0;

        th = CreateThread(&param);
        if (th <= 0) {
            return 1;
        }

        StartThread(th, 0);
    }

    return 0;
}

void snd_ParseCommandLineArg(char *arg) {
    if (!strncmp(arg, "tick_priority", 13)) {
        gThreadPriority_TICK = snd_GetVal(arg + 13);
    } else if (!strncmp(arg, "rpc_priority", 12)) {
        gThreadPriority_RPC = snd_GetVal(arg + 12);
    } else if (!strncmp(arg, "stream_priority", 15)) {
        gThreadPriority_STREAM = snd_GetVal(arg + 15);
    } else if (!strncmp(arg, "do_rpc", 6)) {
        gDoRPC = snd_GetVal(arg + 6) == 1;
    } else if (!strncmp(arg, "limit_2meg", 10)) {
        gLimit2Meg = true;
    } else {
        snd_ShowError(7, (SInt32)arg, 0, 0, 0);
    }
}

SInt32 snd_GetVal(char *st) {
    while (*st != '-' && (*st < '0' || *st >= ':') && *st) {
        st++;
    }

    if (*st == '\0') {
        return 0;
    }

    return strtol(st, NULL, 10);
}

void snd_DumpVersionAndInfo() {
    printf("\n");
    printf("==========================================================\n");
    printf(" 989snd (c)2000-2003 Sony Computer Entertainment America\n");
    printf(" by Buzz Burrowes                               v%d.%d.%d\n", 3, 1, 2);
    printf("                        (built %s at %s)\n", "Nov 19 2003", "14:31:04");
    printf(" (build with sce library rev %d.%d.%d)\n", 2, 7, 0);
    printf("==========================================================\n");
    printf("    Thread Priorities:\n");
    printf("                         Sound Tick = %d\n", gThreadPriority_TICK);
    printf("                           Main RPC = %d\n", gThreadPriority_RPC);
    printf("                        Loading RPC = %d\n", gThreadPriority_RPC + 1);
    printf("                          Streaming = %d, %d, %d, %d\n", 
        gThreadPriority_STREAM, gThreadPriority_STREAM + 1, gThreadPriority_STREAM + 2, gThreadPriority_STREAM + 3);
    printf("   Memory Locations...\n");
    printf("       Voice Allocator Owner  : 0x%8.8x\n", &gVAllocOwnerID);
    printf("       Master Tick Owner List : 0x%8.8x\n", gMasterTickOwner);
    printf("==========================================================\n");
    printf("\n");
}

void snd_RegisterErrorDisplayFunc(SndErrorDisplayFunc func) {
    gErrorDisplayFunc = func;
}

void snd_ShowError(SInt32 num, UInt32 a1, UInt32 a2, UInt32 a3, UInt32 a4) {
    if (!gErrorDisplayFunc) {
        if (!gPrefs_Silent) {
            printf("989snd Error: cause %d -> %u, %u, %u, %u\n", num, a1, a2, a3, a4);
        }

        return;
    }

    gErrorDisplayFunc(num, a1, a2, a3, a4);
}

void snd_EEDMADone(SInt32 *sema_id_ptr) {
    iSignalSema(*sema_id_ptr);
}

void snd_Install989Monitor(Extern989MonitorInfo *mon) {
    g989Monitor = mon;
}
