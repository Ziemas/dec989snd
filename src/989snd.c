#include "common.h"
#include "functions.h"

/* data 0 */ UInt16 g989Version;
/* data 4 */ ModuleInfo Module;
/* data c */ Extern989MonitorInfo *g989Monitor;
/* data 10 */ SInt32 gThreadPriority_TICK;
/* data 14 */ SInt32 gThreadPriority_RPC;
/* data 18 */ SInt32 gThreadPriority_STREAM;
/* data 1c */ SInt32 gMainRPCThreadId;
/* data 20 */ SInt32 gEEDMADoneSema;
/* data 24 */ SInt32 gFileReadMutex;
/* data 28 */ BOOL gDoRPC;
/* data 2c */ SInt8 *snd_MESSAGE_RECIEVE_BUFFER;
/* data 30 */ UInt32 *snd_MESSAGE_RETURN_BUFFER;
/* data 34 */ UInt32 *gWriteBackdataOffset;
/* data 38 */ Extern989HandlerPtr gExternHandlersList;
/* data 3c */ CommandHandler gCommandFunc[0];
/* data 1e4 */ SndErrorDisplayFunc gErrorDisplayFunc;
/* bss 0 */ SInt8 snd_LOADER_MESSAGE_RECIEVE_BUFFER[256];
/* bss 100 */ UInt32 snd_LOADER_MESSAGE_RETURN_BUFFER[4];

//INCLUDE_ASM("asm/nonmatchings/989snd", snd_RegisterExternProcHandler);
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

//INCLUDE_ASM("asm/nonmatchings/989snd", snd_UnregisterExternProcHandler);
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

INCLUDE_ASM("asm/nonmatchings/989snd", snd_FindExternProcHandler);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_DoExternCall);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_DoExternCallWithData);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_INIT);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CLOSE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_LOADBANK);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_LOADBANKBYLOC);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_BANKLOADFROMEE);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_BANKLOADFROMIOP);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_LOADMMD);

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

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETALLSOUNDREG);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_SETMASTERVOLUMEDUCKER);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_CDSTATUS);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_CMD_SL_COMMAND_BATCH);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_EEMessageParser);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_StartEEMessaging);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_EELoaderMessageParser);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_StartEELoaderMessaging);

INCLUDE_ASM("asm/nonmatchings/989snd", start);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_ParseCommandLineArg);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_GetVal);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_DumpVersionAndInfo);

//INCLUDE_ASM("asm/nonmatchings/989snd", snd_RegisterErrorDisplayFunc);
void snd_RegisterErrorDisplayFunc(SndErrorDisplayFunc func) {
    gErrorDisplayFunc = func;
}

INCLUDE_ASM("asm/nonmatchings/989snd", snd_ShowError);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_EEDMADone);

INCLUDE_ASM("asm/nonmatchings/989snd", snd_Install989Monitor);
