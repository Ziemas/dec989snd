#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"

// 989snd.c
extern Extern989MonitorInfo *g989Monitor;
extern SInt32 gThreadPriority_TICK;
extern SInt32 gEEDMADoneSema;
extern SInt32 gFileReadMutex;

// init.c
extern UInt32 gMicsPerTick;
extern BOOL gPrefs_ShowMissingSounds;
extern BOOL gPrefs_Silent;

// intr_code.c
extern BOOL gKillTimer;
extern SInt32 gTransfering;
extern SpuTransferCallbackProc gTransferDoneCallback;
extern SInt32 gSPURAMTransSema;
extern SInt32 gDMAInUse[2];
extern SInt32 gWaitingDMAComplete;

// loader.c
extern BOOL gLimit2Meg;
extern char *gEEStatusAddr;
extern ExternSndIOPAlloc gAllocProc;
extern ExternSndIOPFree gFreeProc;
extern SoundBankPtr gBankListHead;
extern SFXBlock2Ptr gBlockListHead;

// playsnd.c
extern SInt32 gGlobalExcite;

// reverb.c
extern SInt16 gReverbDepthL[2];
extern SInt16 gReverbDepthR[2];

// sndhand.c
extern GSoundHandlerPtr gActiveSoundListHead;

// stick.c
extern SInt32 gLockMasterTick;
extern UInt32 gMasterTickOwner[32];

// stream.c
extern SInt32 gNumVAGStreams;
extern VAGStreamHandler *gVAGStreamHandler;
extern VAGBuffer *VAGStreamDMAList;
extern SInt32 gStartDMASema;

// valloc.c
extern UInt32 gVAllocOwnerID;
extern VoiceAttributes *gPlayingListHead;
extern VoiceAttributes gChannelStatus[48];
extern UInt32 gAwaitingKeyOff[2];

#endif