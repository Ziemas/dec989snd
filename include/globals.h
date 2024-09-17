#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"

// loader.c
extern char *gEEStatusAddr;

// init.c
extern BOOL gPrefs_Silent;

// loader.c
extern BOOL gLimit2Meg;
extern ExternSndIOPAlloc gAllocProc;
extern ExternSndIOPFree gFreeProc;

// stick.c
extern UInt32 gMasterTickOwner[32];

// valloc.c
extern UInt32 gVAllocOwnerID;

#endif