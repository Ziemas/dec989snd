// File written by stdump development version on 2024-09-13
// 
// Input file:
//   989SND_D.IRX
// Toolchain version(s):
//   unknown
// Built-in types:
//   char                     8-bit integer
//   complex double           64-bit floating point
//   complex float            32-bit floating point
//   complex long double      64-bit floating point
//   double                   64-bit floating point
//   float                    32-bit floating point
//   int                      32-bit signed integer
//   long double              64-bit floating point
//   long int                 32-bit signed integer
//   long long int            64-bit signed integer
//   long long unsigned int   64-bit unsigned integer
//   long unsigned int        32-bit unsigned integer
//   short int                16-bit signed integer
//   short unsigned int       16-bit unsigned integer
//   signed char              8-bit signed integer
//   unsigned char            8-bit unsigned integer
//   unsigned int             32-bit unsigned integer
//   void                     void

#include "include_asm.h"

typedef struct { // 0x8
	/* 0x0 */ int real;
	/* 0x4 */ int imag;
} complex int;

typedef unsigned int size_t;
typedef int ssize_t;
typedef char *caddr_t;
typedef unsigned char u_char;
typedef short unsigned int u_short;
typedef unsigned int u_int;
typedef long unsigned int u_long;
typedef unsigned char unchar;
typedef short unsigned int ushort;
typedef unsigned int uint;
typedef long unsigned int ulong;

enum KernelErrorCode {
	KE_OK = 0,
	KE_ERROR = -1,
	KE_ILLEGAL_EXPCODE = -50,
	KE_EXPHANDLER_NOUSE = -51,
	KE_EXPHANDLER_USED = -52,
	KE_ILLEGAL_CONTEXT = -100,
	KE_ILLEGAL_INTRCODE = -101,
	KE_CPUDI = -102,
	KE_INTRDISABLE = -103,
	KE_FOUND_HANDLER = -104,
	KE_NOTFOUND_HANDLER = -105,
	KE_NO_TIMER = -150,
	KE_ILLEGAL_TIMERID = -151,
	KE_ILLEGAL_SOURCE = -152,
	KE_ILLEGAL_PRESCALE = -153,
	KE_TIMER_BUSY = -154,
	KE_TIMER_NOT_SETUP = -155,
	KE_TIMER_NOT_INUSE = -156,
	KE_UNIT_USED = -160,
	KE_UNIT_NOUSE = -161,
	KE_NO_ROMDIR = -162,
	KE_LINKERR = -200,
	KE_ILLEGAL_OBJECT = -201,
	KE_UNKNOWN_MODULE = -202,
	KE_NOFILE = -203,
	KE_FILEERR = -204,
	KE_MEMINUSE = -205,
	KE_ALREADY_STARTED = -206,
	KE_NOT_STARTED = -207,
	KE_ALREADY_STOPPED = -208,
	KE_CAN_NOT_STOP = -209,
	KE_NOT_STOPPED = -210,
	KE_NOT_REMOVABLE = -211,
	KE_LIBRARY_FOUND = -212,
	KE_LIBRARY_NOTFOUND = -213,
	KE_ILLEGAL_LIBRARY = -214,
	KE_LIBRARY_INUSE = -215,
	KE_ALREADY_STOPPING = -216,
	KE_ILLEGAL_OFFSET = -217,
	KE_ILLEGAL_POSITION = -218,
	KE_ILLEGAL_ACCESS = -219,
	KE_ILLEGAL_FLAG = -220,
	KE_NO_MEMORY = -400,
	KE_ILLEGAL_ATTR = -401,
	KE_ILLEGAL_ENTRY = -402,
	KE_ILLEGAL_PRIORITY = -403,
	KE_ILLEGAL_STACK_SIZE = -404,
	KE_ILLEGAL_MODE = -405,
	KE_ILLEGAL_THID = -406,
	KE_UNKNOWN_THID = -407,
	KE_UNKNOWN_SEMID = -408,
	KE_UNKNOWN_EVFID = -409,
	KE_UNKNOWN_MBXID = -410,
	KE_UNKNOWN_VPLID = -411,
	KE_UNKNOWN_FPLID = -412,
	KE_DORMANT = -413,
	KE_NOT_DORMANT = -414,
	KE_NOT_SUSPEND = -415,
	KE_NOT_WAIT = -416,
	KE_CAN_NOT_WAIT = -417,
	KE_RELEASE_WAIT = -418,
	KE_SEMA_ZERO = -419,
	KE_SEMA_OVF = -420,
	KE_EVF_COND = -421,
	KE_EVF_MULTI = -422,
	KE_EVF_ILPAT = -423,
	KE_MBOX_NOMSG = -424,
	KE_WAIT_DELETE = -425,
	KE_ILLEGAL_MEMBLOCK = -426,
	KE_ILLEGAL_MEMSIZE = -427,
	KE_ILLEGAL_SPADADDR = -428,
	KE_SPAD_INUSE = -429,
	KE_SPAD_NOT_INUSE = -430,
	KE_ILLEGAL_TYPE = -431,
	KE_ILLEGAL_SIZE = -432
};

struct _libhead { // 0x14
	/* 0x00 */ _libhead *next;
	/* 0x04 */ _libcaller *client;
	/* 0x08 */ short unsigned int version;
	/* 0x0a */ short unsigned int flags;
	/* 0x0c */ char name[8];
};

typedef _libhead libhead;

struct _libcaller { // 0x14
	/* 0x00 */ long unsigned int magic;
	/* 0x04 */ _libcaller *client;
	/* 0x08 */ short unsigned int version;
	/* 0x0a */ short unsigned int flags;
	/* 0x0c */ char name[8];
};

typedef _libcaller libcaller;

struct _moduleinfo { // 0x8
	/* 0x0 */ char *name;
	/* 0x4 */ short unsigned int version;
};

typedef _moduleinfo ModuleInfo;

struct _libinfo { // 0x14
	/* 0x00 */ long unsigned int dummy[2];
	/* 0x08 */ short unsigned int version;
	/* 0x0a */ short unsigned int dummy2;
	/* 0x0c */ char name[8];
};

typedef _libinfo LibInfo;

struct _modulestatus { // 0x60
	/* 0x00 */ char name[56];
	/* 0x38 */ u_short version;
	/* 0x3a */ u_short flags;
	/* 0x3c */ int id;
	/* 0x40 */ u_long entry_addr;
	/* 0x44 */ u_long gp_value;
	/* 0x48 */ u_long text_addr;
	/* 0x4c */ u_long text_size;
	/* 0x50 */ u_long data_size;
	/* 0x54 */ u_long bss_size;
	/* 0x58 */ u_long lreserve[2];
};

typedef _modulestatus ModuleStatus;

struct _ldfilefunc { // 0x20
	/* 0x00 */ int (*beforeOpen)(/* parameters unknown */);
	/* 0x04 */ int (*afterOpen)(/* parameters unknown */);
	/* 0x08 */ int (*close)(/* parameters unknown */);
	/* 0x0c */ int (*setBufSize)(/* parameters unknown */);
	/* 0x10 */ int (*beforeRead)(/* parameters unknown */);
	/* 0x14 */ int (*read)(/* parameters unknown */);
	/* 0x18 */ int (*lseek)(/* parameters unknown */);
	/* 0x1c */ int (*getfsize)(/* parameters unknown */);
};

typedef _ldfilefunc LDfilefunc;

struct _lmwooption { // 0x20
	/* 0x00 */ char position;
	/* 0x01 */ char access;
	/* 0x02 */ char creserved[2];
	/* 0x04 */ void *distaddr;
	/* 0x08 */ int distoffset;
	/* 0x0c */ LDfilefunc *functable;
	/* 0x10 */ void *funcopt;
	/* 0x14 */ int ireserved[3];
};

typedef _lmwooption LMWOoption;

enum EXCEP {
	EXCEP_Int = 0,
	EXCEP_MOD = 1,
	EXCEP_TLBL = 2,
	EXCEP_TLBS = 3,
	EXCEP_AdEL = 4,
	EXCEP_AdES = 5,
	EXCEP_IBE = 6,
	EXCEP_DBE = 7,
	EXCEP_Sys = 8,
	EXCEP_Bp = 9,
	EXCEP_RI = 10,
	EXCEP_CpU = 11,
	EXCEP_Ovf = 12,
	EXCEP_reserv1 = 13,
	EXCEP_reserv2 = 14,
	EXCEP_HDB = 15,
	EXCEP_MAX = 16
};

enum INUM {
	INUM_VBLANK = 0,
	INUM_GM = 1,
	INUM_CDROM = 2,
	INUM_DMA = 3,
	INUM_RTC0 = 4,
	INUM_RTC1 = 5,
	INUM_RTC2 = 6,
	INUM_SIO0 = 7,
	INUM_SIO1 = 8,
	INUM_SPU = 9,
	INUM_PIO = 10,
	INUM_EVBLANK = 11,
	INUM_DVD = 12,
	INUM_PCMCIA = 13,
	INUM_RTC3 = 14,
	INUM_RTC4 = 15,
	INUM_RTC5 = 16,
	INUM_SIO2 = 17,
	INUM_HTR0 = 18,
	INUM_HTR1 = 19,
	INUM_HTR2 = 20,
	INUM_HTR3 = 21,
	INUM_USB = 22,
	INUM_EXTR = 23,
	INUM_FWRE = 24,
	INUM_FDMA = 25,
	INUM_DMA_0 = 32,
	INUM_DMA_1 = 33,
	INUM_DMA_2 = 34,
	INUM_DMA_3 = 35,
	INUM_DMA_4 = 36,
	INUM_DMA_5 = 37,
	INUM_DMA_6 = 38,
	INUM_DMA_BERR = 39,
	INUM_DMA_7 = 40,
	INUM_DMA_8 = 41,
	INUM_DMA_9 = 42,
	INUM_DMA_10 = 43,
	INUM_DMA_11 = 44,
	INUM_DMA_12 = 45,
	INUM_MAX = 64
};

typedef int (*intr_handler)(/* parameters unknown */);

struct SysClock { // 0x8
	/* 0x0 */ u_int low;
	/* 0x4 */ u_int hi;
};

struct ThreadParam { // 0x14
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ void *entry;
	/* 0x0c */ int stackSize;
	/* 0x10 */ int initPriority;
};

struct ThreadInfo { // 0x44
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ int status;
	/* 0x0c */ void *entry;
	/* 0x10 */ void *stack;
	/* 0x14 */ int stackSize;
	/* 0x18 */ void *gpReg;
	/* 0x1c */ int initPriority;
	/* 0x20 */ int currentPriority;
	/* 0x24 */ int waitType;
	/* 0x28 */ int waitId;
	/* 0x2c */ int wakeupCount;
	/* 0x30 */ u_long *regContext;
	/* 0x34 */ int reserved1;
	/* 0x38 */ int reserved2;
	/* 0x3c */ int reserved3;
	/* 0x40 */ int reserved4;
};

typedef struct { // 0x2c
	/* 0x00 */ int status;
	/* 0x04 */ int currentPriority;
	/* 0x08 */ int waitType;
	/* 0x0c */ int waitId;
	/* 0x10 */ int wakeupCount;
	/* 0x14 */ u_long *regContext;
	/* 0x18 */ SysClock runClocks;
	/* 0x20 */ u_int intrPreemptCount;
	/* 0x24 */ u_int threadPreemptCount;
	/* 0x28 */ u_int releaseCount;
} ThreadRunStatus;

struct SemaParam { // 0x10
	/* 0x0 */ u_int attr;
	/* 0x4 */ u_int option;
	/* 0x8 */ int initCount;
	/* 0xc */ int maxCount;
};

struct SemaInfo { // 0x20
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ int initCount;
	/* 0x0c */ int maxCount;
	/* 0x10 */ int currentCount;
	/* 0x14 */ int numWaitThreads;
	/* 0x18 */ int reserved1;
	/* 0x1c */ int reserved2;
};

struct EventFlagParam { // 0xc
	/* 0x0 */ u_int attr;
	/* 0x4 */ u_int option;
	/* 0x8 */ u_int initPattern;
};

struct EventFlagInfo { // 0x1c
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ u_int initPattern;
	/* 0x0c */ u_int currentPattern;
	/* 0x10 */ int numWaitThreads;
	/* 0x14 */ int reserved1;
	/* 0x18 */ int reserved2;
};

struct MbxParam { // 0x8
	/* 0x0 */ u_int attr;
	/* 0x4 */ u_int option;
};

struct MbxInfo { // 0x1c
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ int numWaitThreads;
	/* 0x0c */ int numMessage;
	/* 0x10 */ MsgPacket *topPacket;
	/* 0x14 */ int reserved1;
	/* 0x18 */ int reserved2;
};

struct MsgPacket { // 0x8
	/* 0x0 */ MsgPacket *next;
	/* 0x4 */ u_char msgPriority;
	/* 0x5 */ u_char dummy[3];
};

struct VplParam { // 0xc
	/* 0x0 */ u_int attr;
	/* 0x4 */ u_int option;
	/* 0x8 */ int size;
};

struct VplInfo { // 0x20
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ int size;
	/* 0x0c */ int freeSize;
	/* 0x10 */ int numWaitThreads;
	/* 0x14 */ int reserved1;
	/* 0x18 */ int reserved2;
	/* 0x1c */ int reserved3;
};

struct FplParam { // 0x10
	/* 0x0 */ u_int attr;
	/* 0x4 */ u_int option;
	/* 0x8 */ int blockSize;
	/* 0xc */ int numBlocks;
};

struct FplInfo { // 0x28
	/* 0x00 */ u_int attr;
	/* 0x04 */ u_int option;
	/* 0x08 */ int blockSize;
	/* 0x0c */ int numBlocks;
	/* 0x10 */ int freeBlocks;
	/* 0x14 */ int numWaitThreads;
	/* 0x18 */ int reserved1;
	/* 0x1c */ int reserved2;
	/* 0x20 */ int reserved3;
	/* 0x24 */ int reserved4;
};

typedef u_int (*AlarmHandler)(/* parameters unknown */);

typedef struct { // 0x40
	/* 0x00 */ u_int status;
	/* 0x04 */ int systemLowTimerWidth;
	/* 0x08 */ SysClock idleClocks;
	/* 0x10 */ SysClock kernelClocks;
	/* 0x18 */ u_int comesOutOfIdleCount;
	/* 0x1c */ u_int threadSwitchCount;
	/* 0x20 */ u_int reserved[8];
} SystemStatus;

enum IdListType {
	TMID_Thread = 1,
	TMID_Semaphore = 2,
	TMID_EventFlag = 3,
	TMID_Mbox = 4,
	TMID_Vpl = 5,
	TMID_Fpl = 6,
	TMID_SleepThread = 7,
	TMID_DelayThread = 8,
	TMID_DormantThread = 9
};

typedef struct { // 0x10
	/* 0x0 */ unsigned int data;
	/* 0x4 */ unsigned int addr;
	/* 0x8 */ unsigned int size;
	/* 0xc */ unsigned int mode;
} sceSifDmaData;

struct _sif_rpc_data { // 0x10
	/* 0x0 */ void *paddr;
	/* 0x4 */ unsigned int pid;
	/* 0x8 */ int tid;
	/* 0xc */ unsigned int mode;
};

typedef _sif_rpc_data sceSifRpcData;
typedef void (*sceSifEndFunc)(/* parameters unknown */);

struct _sif_client_data { // 0x28
	/* 0x00 */ _sif_rpc_data rpcd;
	/* 0x10 */ unsigned int command;
	/* 0x14 */ void *buff;
	/* 0x18 */ void *cbuff;
	/* 0x1c */ sceSifEndFunc func;
	/* 0x20 */ void *para;
	/* 0x24 */ _sif_serve_data *serve;
};

typedef _sif_client_data sceSifClientData;

struct _sif_receive_data { // 0x1c
	/* 0x00 */ _sif_rpc_data rpcd;
	/* 0x10 */ void *src;
	/* 0x14 */ void *dest;
	/* 0x18 */ int size;
};

typedef _sif_receive_data sceSifReceiveData;
typedef void* (*sceSifRpcFunc)(/* parameters unknown */);

struct _sif_serve_data { // 0x44
	/* 0x00 */ unsigned int command;
	/* 0x04 */ sceSifRpcFunc func;
	/* 0x08 */ void *buff;
	/* 0x0c */ int size;
	/* 0x10 */ sceSifRpcFunc cfunc;
	/* 0x14 */ void *cbuff;
	/* 0x18 */ int csize;
	/* 0x1c */ sceSifClientData *client;
	/* 0x20 */ void *paddr;
	/* 0x24 */ unsigned int fno;
	/* 0x28 */ void *receive;
	/* 0x2c */ int rsize;
	/* 0x30 */ int rmode;
	/* 0x34 */ unsigned int rid;
	/* 0x38 */ _sif_serve_data *link;
	/* 0x3c */ _sif_serve_data *next;
	/* 0x40 */ _sif_queue_data *base;
};

typedef _sif_serve_data sceSifServeData;

struct _sif_queue_data { // 0x18
	/* 0x00 */ int key;
	/* 0x04 */ int active;
	/* 0x08 */ _sif_serve_data *link;
	/* 0x0c */ _sif_serve_data *start;
	/* 0x10 */ _sif_serve_data *end;
	/* 0x14 */ _sif_queue_data *next;
};

typedef _sif_queue_data sceSifQueueData;
typedef void (*CdlCB)(/* parameters unknown */);

typedef struct { // 0x4
	/* 0x0 */ u_char trycount;
	/* 0x1 */ u_char spindlctrl;
	/* 0x2 */ u_char datapattern;
	/* 0x3 */ u_char pad;
} sceCdRMode;

typedef struct { // 0x4
	/* 0x0 */ u_char minute;
	/* 0x1 */ u_char second;
	/* 0x2 */ u_char sector;
	/* 0x3 */ u_char track;
} sceCdlLOCCD;

typedef struct { // 0x24
	/* 0x00 */ u_int lsn;
	/* 0x04 */ u_int size;
	/* 0x08 */ char name[16];
	/* 0x18 */ u_char date[8];
	/* 0x20 */ u_int flag;
} sceCdlFILE;

typedef struct { // 0x8
	/* 0x0 */ u_char stat;
	/* 0x1 */ u_char second;
	/* 0x2 */ u_char minute;
	/* 0x3 */ u_char hour;
	/* 0x4 */ u_char pad;
	/* 0x5 */ u_char day;
	/* 0x6 */ u_char month;
	/* 0x7 */ u_char year;
} sceCdCLOCK;

typedef struct { // 0xc
	/* 0x0 */ u_int bufmax;
	/* 0x4 */ u_int bankmax;
	/* 0x8 */ u_int iop_bufaddr;
} sceCdStmInit;

typedef char SInt8;
typedef unsigned char UInt8;
typedef short int SInt16;
typedef short unsigned int UInt16;
typedef int SInt32;
typedef unsigned int UInt32;
typedef long int SInt64;
typedef long unsigned int UInt64;
typedef int BOOL;

typedef struct { // 0x4
	/* 0x0 */ SInt16 left;
	/* 0x2 */ SInt16 right;
} SpuVolume;

typedef struct { // 0x4
	/* 0x0 */ UInt16 adsr1;
	/* 0x2 */ UInt16 adsr2;
} ADSRSettings;

typedef struct { // 0x8
	/* 0x0 */ UInt32 core[2];
} VoiceFlags;

typedef struct { // 0x30
	/* 0x00 */ UInt32 ID;
	/* 0x04 */ UInt32 Version;
	/* 0x08 */ UInt32 Reserved1;
	/* 0x0c */ UInt32 DataSize;
	/* 0x10 */ UInt32 SampleRate;
	/* 0x14 */ UInt32 Reserved2;
	/* 0x18 */ UInt32 Reserved3;
	/* 0x1c */ UInt32 Reserved4;
	/* 0x20 */ char Name[16];
} SCEIVAGHeader;

typedef struct { // 0x18
	/* 0x00 */ SInt8 Priority;
	/* 0x01 */ SInt8 Vol;
	/* 0x02 */ SInt8 CenterNote;
	/* 0x03 */ SInt8 CenterFine;
	/* 0x04 */ SInt16 Pan;
	/* 0x06 */ SInt8 MapLow;
	/* 0x07 */ SInt8 MapHigh;
	/* 0x08 */ SInt8 PBLow;
	/* 0x09 */ SInt8 PBHigh;
	/* 0x0a */ UInt16 ADSR1;
	/* 0x0c */ UInt16 ADSR2;
	/* 0x0e */ SInt16 Flags;
	/* 0x10 */ void *VAGInSR;
	/* 0x14 */ UInt32 reserved1;
} Tone;

typedef struct *TonePtr { // 0x18
	/* 0x00 */ SInt8 Priority;
	/* 0x01 */ SInt8 Vol;
	/* 0x02 */ SInt8 CenterNote;
	/* 0x03 */ SInt8 CenterFine;
	/* 0x04 */ SInt16 Pan;
	/* 0x06 */ SInt8 MapLow;
	/* 0x07 */ SInt8 MapHigh;
	/* 0x08 */ SInt8 PBLow;
	/* 0x09 */ SInt8 PBHigh;
	/* 0x0a */ UInt16 ADSR1;
	/* 0x0c */ UInt16 ADSR2;
	/* 0x0e */ SInt16 Flags;
	/* 0x10 */ void *VAGInSR;
	/* 0x14 */ UInt32 reserved1;
};

typedef struct { // 0x8
	/* 0x0 */ SInt8 NumTones;
	/* 0x1 */ SInt8 Vol;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ TonePtr FirstTone;
} Prog;

typedef struct *ProgPtr { // 0x8
	/* 0x0 */ SInt8 NumTones;
	/* 0x1 */ SInt8 Vol;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ TonePtr FirstTone;
};

typedef struct { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ void *OrigBank;
	/* 0x0c */ SInt8 OrigType;
	/* 0x0d */ SInt8 Prog;
	/* 0x0e */ SInt8 Note;
	/* 0x0f */ SInt8 Fine;
	/* 0x10 */ SInt16 Vol;
	/* 0x12 */ SInt8 pad1;
	/* 0x13 */ SInt8 VolGroup;
	/* 0x14 */ SInt16 Pan;
	/* 0x16 */ SInt8 XREFSound;
	/* 0x17 */ SInt8 pad2;
	/* 0x18 */ UInt16 Flags;
	/* 0x1a */ UInt16 pad3;
} Sound;

typedef struct *SoundPtr { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ void *OrigBank;
	/* 0x0c */ SInt8 OrigType;
	/* 0x0d */ SInt8 Prog;
	/* 0x0e */ SInt8 Note;
	/* 0x0f */ SInt8 Fine;
	/* 0x10 */ SInt16 Vol;
	/* 0x12 */ SInt8 pad1;
	/* 0x13 */ SInt8 VolGroup;
	/* 0x14 */ SInt16 Pan;
	/* 0x16 */ SInt8 XREFSound;
	/* 0x17 */ SInt8 pad2;
	/* 0x18 */ UInt16 Flags;
	/* 0x1a */ UInt16 pad3;
};

typedef struct { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ SInt8 IDs[20];
} CompoundSound;

typedef struct *CompoundSoundPtr { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ SInt8 IDs[20];
};

typedef struct { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ void *OrigBank;
	/* 0x0c */ UInt32 MIDIID;
	/* 0x10 */ SInt16 Vol;
	/* 0x12 */ SInt8 Repeats;
	/* 0x13 */ SInt8 VolGroup;
	/* 0x14 */ SInt16 Pan;
	/* 0x16 */ SInt8 Index;
	/* 0x17 */ UInt8 Flags;
	/* 0x18 */ void *MIDIBlock;
} MIDISound;

typedef struct *MIDISoundPtr { // 0x1c
	/* 0x00 */ SInt32 Type;
	/* 0x04 */ SoundBank_tag *Bank;
	/* 0x08 */ void *OrigBank;
	/* 0x0c */ UInt32 MIDIID;
	/* 0x10 */ SInt16 Vol;
	/* 0x12 */ SInt8 Repeats;
	/* 0x13 */ SInt8 VolGroup;
	/* 0x14 */ SInt16 Pan;
	/* 0x16 */ SInt8 Index;
	/* 0x17 */ UInt8 Flags;
	/* 0x18 */ void *MIDIBlock;
};

struct SoundBank_tag { // 0x34
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ UInt32 Version;
	/* 0x08 */ UInt32 Flags;
	/* 0x0c */ UInt32 BankID;
	/* 0x10 */ SInt8 BankNum;
	/* 0x11 */ SInt8 pad1;
	/* 0x12 */ SInt16 pad2;
	/* 0x14 */ SInt16 NumSounds;
	/* 0x16 */ SInt16 NumProgs;
	/* 0x18 */ SInt16 NumTones;
	/* 0x1a */ SInt16 NumVAGs;
	/* 0x1c */ SoundPtr FirstSound;
	/* 0x20 */ ProgPtr FirstProg;
	/* 0x24 */ TonePtr FirstTone;
	/* 0x28 */ void *VagsInSR;
	/* 0x2c */ UInt32 VagDataSize;
	/* 0x30 */ SoundBank_tag *NextBank;
};

typedef SoundBank_tag SoundBank;
typedef SoundBank_tag *SoundBankPtr;

typedef struct { // 0x30
	/* 0x00 */ UInt16 type;
	/* 0x02 */ UInt8 target;
	/* 0x03 */ UInt8 target_extra;
	/* 0x04 */ UInt8 setup_flags;
	/* 0x05 */ UInt8 running_flags;
	/* 0x06 */ UInt16 depth;
	/* 0x08 */ UInt32 orig_depth;
	/* 0x0c */ SInt32 next_step;
	/* 0x10 */ UInt32 step_size;
	/* 0x14 */ UInt32 orig_step_size;
	/* 0x18 */ SInt32 state_hold1;
	/* 0x1c */ SInt32 state_hold2;
	/* 0x20 */ SInt32 range;
	/* 0x24 */ SInt32 last_lfo;
	/* 0x28 */ BlockSoundHandler *handler;
	/* 0x2c */ LFOTracker *next;
} LFOTracker;

typedef struct { // 0x20
	/* 0x00 */ char blank[32];
} LargestGrainParamStruct;

typedef struct { // 0x10
	/* 0x0 */ UInt32 BankID;
	/* 0x4 */ UInt32 SoundIndex;
	/* 0x8 */ SInt32 PitchMod;
	/* 0xc */ UInt32 Flags;
} XREFGrainParams;

typedef struct { // 0x4
	/* 0x0 */ SInt32 Amount;
} RandDelayParams;

typedef struct { // 0x20
	/* 0x00 */ UInt32 id;
	/* 0x04 */ UInt32 index;
	/* 0x08 */ UInt8 data[24];
} PluginParams;

typedef struct { // 0x10
	/* 0x0 */ UInt8 which_lfo;
	/* 0x1 */ UInt8 target;
	/* 0x2 */ UInt8 target_extra;
	/* 0x3 */ UInt8 shape;
	/* 0x4 */ UInt16 duty_cycle;
	/* 0x6 */ UInt16 depth;
	/* 0x8 */ UInt16 flags;
	/* 0xa */ UInt16 start_offset;
	/* 0xc */ UInt32 step_size;
} LFOParams;

typedef struct { // 0x8
	/* 0x0 */ SInt16 param[4];
} ControlParams;

typedef struct { // 0x20
	/* 0x00 */ SInt32 vol;
	/* 0x04 */ SInt32 pan;
	/* 0x08 */ SInt8 reg_settings[4];
	/* 0x0c */ SInt32 sound_id;
	/* 0x10 */ char snd_name[16];
} PlaySoundParams;

typedef union { // 0x20
	/* 0x00 */ Tone tone;
	/* 0x00 */ XREFGrainParams xref;
	/* 0x00 */ RandDelayParams delay;
	/* 0x00 */ ControlParams control;
	/* 0x00 */ LFOParams lfo;
	/* 0x00 */ PlaySoundParams play_sound;
	/* 0x00 */ PluginParams plugin_params;
	/* 0x00 */ LargestGrainParamStruct junk;
} GrainParams;

typedef struct { // 0x28
	/* 0x00 */ UInt32 Type;
	/* 0x04 */ SInt32 Delay;
	/* 0x08 */ GrainParams Params;
} SFXGrain;

typedef struct *SFXGrainPtr { // 0x28
	/* 0x00 */ UInt32 Type;
	/* 0x04 */ SInt32 Delay;
	/* 0x08 */ GrainParams Params;
};

struct _opd1 { // 0x4
	/* 0x0 */ SInt8 Arg[3];
	/* 0x3 */ UInt8 Type;
};

union _op_data { // 0x4
	/* 0x0 */ _opd1 MicroOp;
	/* 0x0 */ UInt32 Opcode;
};

typedef struct { // 0x8
	/* 0x0 */ _op_data OpcodeData;
	/* 0x4 */ SInt32 Delay;
} SFXGrain2;

typedef struct *SFXGrain2Ptr { // 0x8
	/* 0x0 */ _op_data OpcodeData;
	/* 0x4 */ SInt32 Delay;
};

typedef struct { // 0xc
	/* 0x0 */ SInt8 Vol;
	/* 0x1 */ SInt8 VolGroup;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ SInt8 NumGrains;
	/* 0x5 */ SInt8 InstanceLimit;
	/* 0x6 */ UInt16 Flags;
	/* 0x8 */ SFXGrainPtr FirstGrain;
} SFX;

typedef struct *SFXPtr { // 0xc
	/* 0x0 */ SInt8 Vol;
	/* 0x1 */ SInt8 VolGroup;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ SInt8 NumGrains;
	/* 0x5 */ SInt8 InstanceLimit;
	/* 0x6 */ UInt16 Flags;
	/* 0x8 */ SFXGrainPtr FirstGrain;
};

typedef struct { // 0xc
	/* 0x0 */ SInt8 Vol;
	/* 0x1 */ SInt8 VolGroup;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ SInt8 NumGrains;
	/* 0x5 */ SInt8 InstanceLimit;
	/* 0x6 */ UInt16 Flags;
	/* 0x8 */ SFXGrain2Ptr FirstGrain;
} SFX2;

typedef struct *SFX2Ptr { // 0xc
	/* 0x0 */ SInt8 Vol;
	/* 0x1 */ SInt8 VolGroup;
	/* 0x2 */ SInt16 Pan;
	/* 0x4 */ SInt8 NumGrains;
	/* 0x5 */ SInt8 InstanceLimit;
	/* 0x6 */ UInt16 Flags;
	/* 0x8 */ SFXGrain2Ptr FirstGrain;
};

typedef struct { // 0x10
	/* 0x0 */ UInt32 data[4];
} SFXUserData;

typedef SFXUserData *SFXUserDatPtr;

typedef struct { // 0x14
	/* 0x00 */ UInt32 Name[4];
	/* 0x10 */ SInt16 Index;
	/* 0x12 */ SInt16 reserved;
} SFXName;

typedef SFXName *SFXNamePtr;

typedef struct { // 0x1c
	/* 0x00 */ UInt32 Name[4];
	/* 0x10 */ UInt32 Offset;
	/* 0x14 */ UInt32 res1;
	/* 0x18 */ UInt32 res2;
} VAGName;

typedef VAGName *VAGNamePtr;

typedef struct { // 0x20
	/* 0x00 */ UInt32 BlockName[2];
	/* 0x08 */ UInt32 VAGName[4];
	/* 0x18 */ UInt32 VAGLocation;
	/* 0x1c */ UInt32 VAGSR;
} VAGImport;

typedef VAGImport *VAGImportPtr;

typedef struct { // 0x18
	/* 0x00 */ UInt32 VAGName[4];
	/* 0x10 */ UInt32 VAGLocation;
	/* 0x14 */ UInt32 VAGSR;
} VAGExport;

typedef VAGExport *VAGExportPtr;

typedef struct { // 0x98
	/* 0x00 */ UInt32 BlockName[2];
	/* 0x08 */ UInt32 SFXNameTableOffset;
	/* 0x0c */ UInt32 VAGNameTableOffset;
	/* 0x10 */ UInt32 VAGImportsTableOffset;
	/* 0x14 */ UInt32 VAGExportsTableOffset;
	/* 0x18 */ SInt16 SFXHashOffsets[32];
	/* 0x58 */ SInt16 VAGHashOffsets[32];
} SFXBlockNames;

typedef SFXBlockNames *SFXBlockNamesPtr;

struct SNDModelNodeHeader_t { // 0x14
	/* 0x00 */ UInt8 Type;
	/* 0x01 */ UInt8 pad;
	/* 0x02 */ UInt16 Flags;
	/* 0x04 */ UInt32 OutputRef;
	/* 0x08 */ UInt32 InputRef;
	/* 0x0c */ SInt32 WorkVal;
	/* 0x10 */ SNDModelNodeHeader_t *Next;
};

typedef SNDModelNodeHeader_t SNDModelNodeHeader;

typedef struct { // 0x24
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ UInt32 NameHash;
	/* 0x18 */ SInt32 MyOut;
	/* 0x1c */ SInt32 InitialValue;
	/* 0x20 */ SInt32 InstanceRecordOffset;
} SNDModelController;

typedef struct { // 0x20
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ UInt32 NameHash;
	/* 0x18 */ UInt32 Duration;
	/* 0x1c */ SInt32 InstanceRecordOffset;
} SNDModelEvent;

typedef struct { // 0x1c
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ UInt8 MixType;
	/* 0x18 */ SNDModelNodeHeader **Inputs;
} SNDModelMixer;

typedef struct { // 0x8
	/* 0x0 */ SInt32 Out;
	/* 0x4 */ SInt32 In;
} SNDModelCurvePoint;

typedef struct { // 0x1c
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ UInt32 Flags;
	/* 0x18 */ SNDModelCurvePoint *Point;
} SNDModelCurve;

typedef struct { // 0x18
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ UInt16 Target;
	/* 0x16 */ UInt16 TargetExtra;
} SNDModelParamOut;

typedef struct { // 0x30
	/* 0x00 */ SNDModelNodeHeader Header;
	/* 0x14 */ SFXName SoundID;
	/* 0x28 */ UInt32 Flags;
	/* 0x2c */ SNDModelParamOut *Params;
} SNDModelSoundElement;

typedef struct { // 0x20
	/* 0x00 */ UInt32 Flags;
	/* 0x04 */ SNDModelController *Controller;
	/* 0x08 */ SNDModelEvent *Event;
	/* 0x0c */ SNDModelNodeHeader *SNDModelTickNodes;
	/* 0x10 */ SNDModelNodeHeader *MiddleNodes;
	/* 0x14 */ SNDModelSoundElement *Element;
	/* 0x18 */ SInt32 InstanceRecordSize;
	/* 0x1c */ SInt32 *InstanceTableHead;
} SNDModel;

typedef struct { // 0x3c
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ UInt32 Version;
	/* 0x08 */ UInt32 Flags;
	/* 0x0c */ UInt32 BlockID;
	/* 0x10 */ SInt8 BlockNum;
	/* 0x11 */ SInt8 pad1;
	/* 0x12 */ SInt16 pad2;
	/* 0x14 */ SInt16 pad3;
	/* 0x16 */ SInt16 NumSounds;
	/* 0x18 */ SInt16 NumGrains;
	/* 0x1a */ SInt16 NumVAGs;
	/* 0x1c */ SFXPtr FirstSound;
	/* 0x20 */ SFXGrainPtr FirstGrain;
	/* 0x24 */ void *VagsInSR;
	/* 0x28 */ UInt32 VagDataSize;
	/* 0x2c */ UInt32 SRAMAllocSize;
	/* 0x30 */ SFXBlock *NextBlock;
	/* 0x34 */ SFXBlockNames *BlockNames;
	/* 0x38 */ SFXUserData *SFXUD;
} SFXBlock;

typedef SFXBlock *SFXBlockPtr;

typedef struct { // 0x40
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ UInt32 Version;
	/* 0x08 */ UInt32 Flags;
	/* 0x0c */ UInt32 BlockID;
	/* 0x10 */ SInt8 BlockNum;
	/* 0x11 */ SInt8 pad1;
	/* 0x12 */ SInt16 pad2;
	/* 0x14 */ SInt16 pad3;
	/* 0x16 */ SInt16 NumSounds;
	/* 0x18 */ SInt16 NumGrains;
	/* 0x1a */ SInt16 NumVAGs;
	/* 0x1c */ SFX2Ptr FirstSound;
	/* 0x20 */ SFXGrain2Ptr FirstGrain;
	/* 0x24 */ void *VagsInSR;
	/* 0x28 */ UInt32 VagDataSize;
	/* 0x2c */ UInt32 SRAMAllocSize;
	/* 0x30 */ SFXBlock2 *NextBlock;
	/* 0x34 */ void *GrainData;
	/* 0x38 */ SFXBlockNames *BlockNames;
	/* 0x3c */ SFXUserData *SFXUD;
} SFXBlock2;

typedef SFXBlock2 *SFXBlock2Ptr;

typedef struct { // 0x28
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ SInt16 Version;
	/* 0x06 */ SInt8 Flags;
	/* 0x07 */ SInt8 pad1;
	/* 0x08 */ UInt32 ID;
	/* 0x0c */ void *NextMIDIBlock;
	/* 0x10 */ UInt32 BankID;
	/* 0x14 */ SoundBankPtr BankPtr;
	/* 0x18 */ SInt8 *DataStart;
	/* 0x1c */ SInt8 *MultiMIDIParent;
	/* 0x20 */ UInt32 Tempo;
	/* 0x24 */ SInt32 PPQ;
} MIDIBlockHeader;

typedef struct *MIDIBlockHeaderPtr { // 0x28
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ SInt16 Version;
	/* 0x06 */ SInt8 Flags;
	/* 0x07 */ SInt8 pad1;
	/* 0x08 */ UInt32 ID;
	/* 0x0c */ void *NextMIDIBlock;
	/* 0x10 */ UInt32 BankID;
	/* 0x14 */ SoundBankPtr BankPtr;
	/* 0x18 */ SInt8 *DataStart;
	/* 0x1c */ SInt8 *MultiMIDIParent;
	/* 0x20 */ UInt32 Tempo;
	/* 0x24 */ SInt32 PPQ;
};

typedef struct { // 0x14
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ SInt16 Version;
	/* 0x06 */ SInt8 Flags;
	/* 0x07 */ SInt8 NumMIDIBlocks;
	/* 0x08 */ UInt32 ID;
	/* 0x0c */ void *NextMIDIBlock;
	/* 0x10 */ SInt8 *BlockPtr[1];
} MultiMIDIBlockHeader;

typedef struct *MultiMIDIBlockHeaderPtr { // 0x14
	/* 0x00 */ UInt32 DataID;
	/* 0x04 */ SInt16 Version;
	/* 0x06 */ SInt8 Flags;
	/* 0x07 */ SInt8 NumMIDIBlocks;
	/* 0x08 */ UInt32 ID;
	/* 0x0c */ void *NextMIDIBlock;
	/* 0x10 */ SInt8 *BlockPtr[1];
};

typedef struct { // 0x14
	/* 0x00 */ UInt32 flags;
	/* 0x04 */ SInt32 loc;
	/* 0x08 */ SInt32 offset;
	/* 0x0c */ SInt8 vol;
	/* 0x0d */ SInt8 sub_group;
	/* 0x0e */ SInt16 pan;
	/* 0x10 */ VAGStreamQueEntry *next;
} VAGStreamQueEntry;

typedef struct { // 0x4
	/* 0x0 */ SInt8 vol;
	/* 0x1 */ SInt8 group;
	/* 0x2 */ SInt16 pan;
} VPKFileVolPanPair;

typedef struct { // 0x1c
	/* 0x00 */ UInt32 type;
	/* 0x04 */ UInt32 size;
	/* 0x08 */ UInt32 data_offset;
	/* 0x0c */ UInt32 buff_size;
	/* 0x10 */ UInt32 sample_rate;
	/* 0x14 */ UInt32 num_channels;
	/* 0x18 */ VPKFileVolPanPair vol_pan[1];
} VPKFileHead;

typedef VPKFileHead *VPKFileHeadPtr;

typedef struct { // 0x2c
	/* 0x00 */ UInt32 flags;
	/* 0x04 */ SInt8 *IOPbuff;
	/* 0x08 */ SInt8 *SPUbuff;
	/* 0x0c */ VAGBuffer *list;
	/* 0x10 */ VAGStreamHeader *owner;
	/* 0x14 */ SInt32 is_end;
	/* 0x18 */ UInt32 bytes;
	/* 0x1c */ char *shift_from;
	/* 0x20 */ char *shift_to;
	/* 0x24 */ UInt32 shift_amount;
	/* 0x28 */ SInt8 *OrigIOPbuff;
} VAGBuffer;

typedef VAGBuffer *VAGBufferPtr;

typedef struct { // 0xd4
	/* 0x00 */ UInt32 flags;
	/* 0x04 */ SInt8 orig_vol;
	/* 0x05 */ SInt8 group;
	/* 0x06 */ SInt16 orig_pan;
	/* 0x08 */ UInt32 pitch;
	/* 0x0c */ UInt32 buff_size;
	/* 0x10 */ VAGBuffer buff[2];
	/* 0x68 */ SInt32 PlayingBuffer;
	/* 0x6c */ UInt32 LastPosition;
	/* 0x70 */ SInt32 LastBufferChangeTick;
	/* 0x74 */ SInt32 ErrorAccumulator;
	/* 0x78 */ SInt32 start_error_accumulator;
	/* 0x7c */ SInt32 voice;
	/* 0x80 */ SInt32 file_handle;
	/* 0x84 */ UInt32 next_read_offset;
	/* 0x88 */ UInt32 file_start_sector;
	/* 0x8c */ UInt32 file_start_offset;
	/* 0x90 */ UInt32 stdio_file_offset;
	/* 0x94 */ UInt32 ee_streammon_loc;
	/* 0x98 */ UInt32 ee_BufferSize;
	/* 0x9c */ char *ee_EEStartAddress;
	/* 0xa0 */ char *ee_NextIOPReadAddress;
	/* 0xa4 */ UInt32 ee_TotalBytesConsumed;
	/* 0xa8 */ UInt32 bytes_total;
	/* 0xac */ UInt32 bytes_remaining;
	/* 0xb0 */ UInt32 samples_total;
	/* 0xb4 */ UInt32 bytes_played;
	/* 0xb8 */ SInt32 sample_rate;
	/* 0xbc */ UInt32 last_loc;
	/* 0xc0 */ UInt32 cur_pos;
	/* 0xc4 */ SInt8 master_volume;
	/* 0xc5 */ SInt8 sub_group;
	/* 0xc6 */ SInt16 master_pan;
	/* 0xc8 */ VAGStreamHeader *sync_list;
	/* 0xcc */ VAGStreamHeader *next;
	/* 0xd0 */ VAGStreamHandler *handler;
} VAGStreamHeader;

typedef VAGStreamHeader *VAGStreamHeaderPtr;
typedef SInt32 (*EffectProcPtr)(/* parameters unknown */);

typedef struct { // 0x10
	/* 0x0 */ UInt32 Flags;
	/* 0x4 */ SInt16 delta_counter;
	/* 0x6 */ SInt16 delta_type;
	/* 0x8 */ EffectChain *next;
	/* 0xc */ EffectProcPtr proc;
} EffectChain;

typedef EffectChain *EffectChainPtr;

typedef struct { // 0x1c
	/* 0x00 */ EffectChain ec;
	/* 0x10 */ SInt32 value;
	/* 0x14 */ SInt16 delta_value;
	/* 0x16 */ SInt16 delta_time;
	/* 0x18 */ SInt16 destination;
	/* 0x1a */ SInt16 pad;
} BasicEffect;

typedef BasicEffect *BasicEffectPtr;

typedef struct { // 0x20
	/* 0x00 */ EffectChain ec;
	/* 0x10 */ SInt32 num_effects;
	/* 0x14 */ EffectChainPtr first;
	/* 0x18 */ SInt32 num_params;
	/* 0x1c */ SInt32 params[1];
} ModEffectmaster;

typedef struct { // 0x2c
	/* 0x00 */ EffectChain ec;
	/* 0x10 */ SInt32 type;
	/* 0x14 */ SInt32 rate;
	/* 0x18 */ SInt32 depth;
	/* 0x1c */ SInt32 rate_slider_rate;
	/* 0x20 */ SInt32 rate_slider_connect;
	/* 0x24 */ SInt32 depth_slider_rate;
	/* 0x28 */ SInt32 depth_slider_connect;
} VolumeModEffect;

typedef void (*OwnerProcPtr)(/* parameters unknown */);

struct GenericSoundHandler { // 0x34
	/* 0x00 */ UInt32 OwnerID;
	/* 0x04 */ SoundPtr Sound;
	/* 0x08 */ EffectChainPtr Effects;
	/* 0x0c */ SInt16 Original_Vol;
	/* 0x0e */ SInt16 Original_Pan;
	/* 0x10 */ SInt16 Current_Vol;
	/* 0x12 */ SInt16 Current_Pan;
	/* 0x14 */ SInt16 Current_PM;
	/* 0x16 */ UInt8 flags;
	/* 0x17 */ SInt8 VolGroup;
	/* 0x18 */ VoiceFlags Voices;
	/* 0x20 */ GenericSoundHandler *prev;
	/* 0x24 */ GenericSoundHandler *next;
	/* 0x28 */ GenericSoundHandler *parent;
	/* 0x2c */ GenericSoundHandler *first_child;
	/* 0x30 */ GenericSoundHandler *siblings;
};

typedef GenericSoundHandler GSoundHandler;
typedef GenericSoundHandler *GSoundHandlerPtr;

typedef struct { // 0x124
	/* 0x000 */ GSoundHandler SH;
	/* 0x034 */ SInt8 Current_Note;
	/* 0x035 */ SInt8 Current_Fine;
	/* 0x036 */ SInt16 Current_PB;
	/* 0x038 */ SInt16 LFO_Vol;
	/* 0x03a */ SInt16 App_Vol;
	/* 0x03c */ SInt16 LFO_Pan;
	/* 0x03e */ SInt16 App_Pan;
	/* 0x040 */ SInt16 LFO_PB;
	/* 0x042 */ SInt16 App_PB;
	/* 0x044 */ SInt16 LFO_PM;
	/* 0x046 */ SInt16 App_PM;
	/* 0x048 */ SInt16 CountDown;
	/* 0x04a */ SInt16 NextGrain;
	/* 0x04c */ SInt8 Registers[4];
	/* 0x050 */ SInt8 sk_doing_skipping_play;
	/* 0x051 */ SInt8 pad;
	/* 0x052 */ SInt16 sk_grains_to_play;
	/* 0x054 */ SInt16 sk_grains_to_skip;
	/* 0x056 */ UInt16 pad2;
	/* 0x058 */ SFXBlock2Ptr block;
	/* 0x05c */ LFOTracker lfo[4];
	/* 0x11c */ UInt32 start_tick;
	/* 0x120 */ SFX2 *orig_sound;
} BlockSoundHandler;

typedef BlockSoundHandler *BlockSoundHandlerPtr;

typedef struct { // 0x58
	/* 0x00 */ GSoundHandler SH;
	/* 0x34 */ SInt32 num_streams;
	/* 0x38 */ SInt32 doubling_voice;
	/* 0x3c */ SInt8 group_vols[16];
	/* 0x4c */ VAGStreamHeader *qued_stream;
	/* 0x50 */ VAGStreamQueEntry *que_list;
	/* 0x54 */ BOOL pre_buffer_queue;
} VAGStreamHandler;

typedef VAGStreamHandler *VAGStreamHandlerPtr;

struct SoundHandler { // 0x40
	/* 0x00 */ GSoundHandler SH;
	/* 0x34 */ SInt8 Current_Note;
	/* 0x35 */ SInt8 Current_Fine;
	/* 0x36 */ SInt16 Current_PB;
	/* 0x38 */ UInt32 StartTick;
	/* 0x3c */ UInt32 Duration;
};

typedef SoundHandler VAGSoundHandler;
typedef SoundHandler *VAGSoundHandlerPtr;

typedef struct { // 0xc0
	/* 0x00 */ GSoundHandler SH;
	/* 0x34 */ SoundBankPtr BankPtr;
	/* 0x38 */ SInt8 *DataStart;
	/* 0x3c */ SInt8 *PlayPos;
	/* 0x40 */ UInt32 TickCountdown;
	/* 0x44 */ SInt32 TickDelta;
	/* 0x48 */ UInt32 Tempo;
	/* 0x4c */ SInt32 PPT;
	/* 0x50 */ SInt32 TickError;
	/* 0x54 */ SInt16 PPQ;
	/* 0x56 */ SInt16 MuteState;
	/* 0x58 */ SInt8 Prog[16];
	/* 0x68 */ SInt8 Vol[16];
	/* 0x78 */ SInt16 Pan[16];
	/* 0x98 */ SInt16 PitchBend[16];
	/* 0xb8 */ SInt16 DamperState;
	/* 0xba */ SInt8 Flags;
	/* 0xbb */ UInt8 RunningStatus;
	/* 0xbc */ SInt8 Repeats;
	/* 0xbd */ SInt8 pad1;
	/* 0xbe */ SInt16 pad2;
} MIDIHandler;

typedef MIDIHandler *MIDIHandlerPtr;

typedef struct { // 0x34
	/* 0x00 */ SInt8 num_channels;
	/* 0x01 */ SInt8 basis;
	/* 0x02 */ SInt8 pad[2];
	/* 0x04 */ SInt8 channel[16];
	/* 0x14 */ SInt8 excite_min[16];
	/* 0x24 */ SInt8 excite_max[16];
} GroupDescription;

typedef struct { // 0x3c4
	/* 0x000 */ GSoundHandler SH;
	/* 0x034 */ GroupDescription group[16];
	/* 0x374 */ SInt8 MIDIRegister[16];
	/* 0x384 */ UInt8 *MIDIMacro[16];
} AMEHandler;

typedef AMEHandler *AMEHandlerPtr;

typedef struct { // 0xe8
	/* 0x00 */ MIDIBlockHeader MB;
	/* 0x28 */ MIDIHandler MH;
} AMEStreamHeader;

typedef AMEStreamHeader *AMEStreamHeaderPtr;

struct basic_data { // 0x4
	/* 0x0 */ UInt32 pad1;
};

struct midi_data { // 0xc
	/* 0x0 */ SInt8 MidiChannel;
	/* 0x1 */ UInt8 KeyOnVelocity;
	/* 0x2 */ SInt16 pad2;
	/* 0x4 */ SInt32 ShouldBeOff;
	/* 0x8 */ ProgPtr KeyOnProg;
};

struct block_data { // 0x4
	/* 0x0 */ SInt8 g_vol;
	/* 0x1 */ SInt8 pad;
	/* 0x2 */ SInt16 g_pan;
};

union ownerdata_tag { // 0xc
	/* 0x0 */ basic_data BasicData;
	/* 0x0 */ midi_data MIDIData;
	/* 0x0 */ block_data BlockData;
};

typedef struct { // 0x38
	/* 0x00 */ VoiceAttributes *playlist;
	/* 0x04 */ SInt32 voice;
	/* 0x08 */ UInt32 Status;
	/* 0x0c */ GSoundHandlerPtr Owner;
	/* 0x10 */ OwnerProcPtr OwnerProc;
	/* 0x14 */ TonePtr Tone;
	/* 0x18 */ SInt8 StartNote;
	/* 0x19 */ SInt8 StartFine;
	/* 0x1a */ UInt8 Priority;
	/* 0x1b */ SInt8 VolGroup;
	/* 0x1c */ UInt32 StartTick;
	/* 0x20 */ SpuVolume Volume;
	/* 0x24 */ SInt16 Current_PB;
	/* 0x26 */ SInt16 Current_PM;
	/* 0x28 */ UInt32 Flags;
	/* 0x2c */ ownerdata_tag OwnerData;
} VoiceAttributes;

typedef VoiceAttributes *VoiceAttributesPtr;

typedef struct { // 0x8
	/* 0x0 */ UInt32 offset;
	/* 0x4 */ UInt32 size;
} LocAndSize;

typedef struct { // 0x10
	/* 0x0 */ UInt32 type;
	/* 0x4 */ UInt32 num_chunks;
	/* 0x8 */ LocAndSize where[1];
} FileAttributes;

typedef FileAttributes *FileAttributesPtr;

typedef struct { // 0x18
	/* 0x00 */ SInt32 source_group;
	/* 0x04 */ UInt32 target_groups;
	/* 0x08 */ SInt32 full_duck_mult;
	/* 0x0c */ SInt32 attack_time;
	/* 0x10 */ SInt32 release_time;
	/* 0x14 */ SInt32 current_duck_mult;
} DuckerDef;

typedef DuckerDef *DuckerDefPtr;

union bank_tag { // 0x8
	/* 0x0 */ char name[8];
	/* 0x0 */ SoundBankPtr ptr;
};

union snd_tag { // 0x10
	/* 0x0 */ char name[16];
	/* 0x0 */ SInt16 index;
};

typedef struct { // 0x30
	/* 0x00 */ bank_tag bank_spec;
	/* 0x08 */ SInt32 vol;
	/* 0x0c */ SInt32 pan;
	/* 0x10 */ snd_tag snd_spec;
	/* 0x20 */ SInt16 pitch_mod;
	/* 0x22 */ SInt16 pitch_bend;
	/* 0x24 */ SInt8 reg[4];
	/* 0x28 */ UInt32 mask;
	/* 0x2c */ UInt32 reserved;
} SndPlayParams;

typedef SndPlayParams *SndPlayParamsPtr;

typedef struct { // 0x40
	/* 0x00 */ UInt32 BufferSize;
	/* 0x04 */ char *EEStartAddress;
	/* 0x08 */ char *NextIOPReadAddress;
	/* 0x0c */ UInt32 TotalBytesConsumed;
	/* 0x10 */ char pad[48];
} EEVagStreamMonitor;

typedef struct { // 0x40
	/* 0x00 */ unsigned int BufferSize;
	/* 0x04 */ char *IOPStartAddress;
	/* 0x08 */ char *NextIOPReadAddress;
	/* 0x0c */ int TotalBytesConsumed;
	/* 0x10 */ char pad[48];
} IOPVagStreamMonitor;

typedef SInt32 (*Extern989Proc)(/* parameters unknown */);

typedef struct { // 0x14
	/* 0x00 */ UInt32 proc_id;
	/* 0x04 */ SInt32 num_funcs;
	/* 0x08 */ UInt32 reserved1;
	/* 0x0c */ UInt32 reserved2;
	/* 0x10 */ Extern989Proc procs[1];
} Extern989Handler;

typedef Extern989Handler *Extern989HandlerPtr;

typedef struct { // 0x4
	/* 0x0 */ SInt16 left;
	/* 0x2 */ SInt16 right;
} VolPair;

typedef void* (*ExternSndIOPAlloc)(/* parameters unknown */);
typedef void (*ExternSndIOPFree)(/* parameters unknown */);

typedef struct { // 0x24
	/* 0x00 */ u_int bank;
	/* 0x04 */ int snd_index;
	/* 0x08 */ u_int bank_name[2];
	/* 0x10 */ u_int snd_name[4];
	/* 0x20 */ int *destination;
} GetSoundUserDataCommandStruct;

typedef void (*ExternBankMonitor)(/* parameters unknown */);
typedef void (*ExternVolMonitor)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ ExternBankMonitor bnk_load;
	/* 0x4 */ ExternVolMonitor vol_changed;
} Extern989MonitorInfo;

typedef struct { // 0xa0
	/* 0x00 */ char name1[64];
	/* 0x40 */ char name2[64];
	/* 0x80 */ UInt32 offset1;
	/* 0x84 */ UInt32 offset2;
	/* 0x88 */ SInt32 vol;
	/* 0x8c */ SInt32 pan;
	/* 0x90 */ SInt32 vol_group;
	/* 0x94 */ UInt32 queue;
	/* 0x98 */ SInt32 sub_group;
	/* 0x9c */ UInt32 flags;
} PVSBN_struct;

typedef PVSBN_struct *PVSBN_structPtr;
typedef void (*SndErrorDisplayFunc)(/* parameters unknown */);
typedef int (*sceSdTransIntrHandler)(/* parameters unknown */);
typedef int (*sceSdSpu2IntrHandler)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ short unsigned int func;
	/* 0x2 */ short unsigned int entry;
	/* 0x4 */ unsigned int value;
} sceSdBatch;

typedef struct { // 0x14
	/* 0x00 */ int core;
	/* 0x04 */ int mode;
	/* 0x08 */ short int depth_L;
	/* 0x0a */ short int depth_R;
	/* 0x0c */ int delay;
	/* 0x10 */ int feedback;
} sceSdEffectAttr;

typedef struct { // 0x20
	/* 0x00 */ int data[7];
	/* 0x1c */ int command;
} SndMessageData;

typedef SndMessageData *SndMessageDataPtr;

typedef struct { // 0x4
	/* 0x0 */ short int command;
	/* 0x2 */ short int size;
} SndCommandEntry;

typedef SndCommandEntry *SndCommandEntryPtr;

typedef struct { // 0x1000
	/* 0x000 */ int num_commands;
	/* 0x004 */ char buffer[4092];
} SndCommandBuffer;

typedef SndCommandBuffer *SndCommandBufferPtr;
typedef void (*SndCompleteProc)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ SndCompleteProc done;
	/* 0x4 */ u_long u_data;
} SndCommandReturnDef;

typedef SndCommandReturnDef *SndCommandReturnDefPtr;

typedef struct { // 0x110
	/* 0x000 */ char name[256];
	/* 0x100 */ int stereo;
	/* 0x104 */ int vol;
	/* 0x108 */ int pan;
	/* 0x10c */ int vol_group;
} VAGStreamCommand;

typedef struct { // 0x40
	/* 0x00 */ int cd_busy;
	/* 0x04 */ int pad_0;
	/* 0x08 */ int pad_1;
	/* 0x0c */ int pad_2;
	/* 0x10 */ int cd_error;
	/* 0x14 */ int pad_3;
	/* 0x18 */ int pad_4;
	/* 0x1c */ int pad_5;
	/* 0x20 */ char pad_big[32];
} SndSystemStatus;

enum {
	SL_INIT = 0,
	SL_CLOSE = 1,
	SL_LOADBANK = 2,
	SL_LOADBANKBYLOC = 3,
	SL_LOADMMD = 4,
	SL_LOADMMDBYLOC = 5,
	SL_UNLOADBANK_A = 6,
	SL_UNLOADMIDI_A = 7,
	SL_RESOLVEBANKS = 8,
	SL_SETMASTERVOL_A = 9,
	SL_GETMASTERVOL = 10,
	SL_SETPLAYBACKMODE_A = 11,
	SL_GETPLAYBACKMODE = 12,
	SL_SETMIXERMODE_A = 13,
	SL_SETREVERBTYPE = 14,
	SL_SETREVERBDEPTH = 15,
	SL_AUTOVERB = 16,
	SL_PLAYSOUND = 17,
	SL_PLAYSOUND_A = 18,
	SL_PAUSESOUND = 19,
	SL_CONTINUESOUND = 20,
	SL_STOPSOUND = 21,
	SL_PAUSEALLSOUNDS = 22,
	SL_CONTINUEALLSOUNDS = 23,
	SL_STOPALLSOUNDS = 24,
	SL_SOUNDISSTILLPLAYING = 25,
	SL_ISSOUNDALOOPER = 26,
	SL_SETSOUNDVOLPAN_A = 27,
	SL_GETSOUNDORIGPITCH = 28,
	SL_GETSOUNDCURRPITCH = 29,
	SL_SETSOUNDPITCH_A = 30,
	SL_SETSOUNDPITCHBEND_A = 31,
	SL_SETSOUNDPITCHMOD_A = 32,
	SL_SETSOUNDPARAMS = 33,
	SL_AUTOVOL_A = 34,
	SL_AUTOPAN_A = 35,
	SL_AUTOPITCH_A = 36,
	SL_AUTOPITCHBEND_A = 37,
	SL_GETGLOBALEXCITE = 38,
	SL_SETGLOBALEXCITE_A = 39,
	SL_GETMIDIREGISTER = 40,
	SL_SETMIDIREGISTER_A = 41,
	SL_INITVAGSTREAMINGEX = 42,
	SL_PLAYVAGSTREAMBYNAME = 43,
	SL_PLAYVAGSTREAMBYLOC = 44,
	SL_PAUSEVAGSTREAM = 45,
	SL_CONTINUEVAGSTREAM = 46,
	SL_STOPVAGSTREAM = 47,
	SL_GETVAGSTREAMQUEUECOUNT = 48,
	SL_GETVAGSTREAMLOC = 49,
	SL_GETVAGSTREAMTIMEREMAINING = 50,
	SL_GETVAGSTREAMTOTALTIME = 51,
	SL_STOPALLVAGSTREAMS = 52,
	SL_CLOSEVAGSTREAMING = 53,
	SL_STREAMSAFECHECKCDIDLE = 54,
	SL_STREAMSAFECDBREAK = 55,
	SL_STREAMSAFECDREAD = 56,
	SL_ALLOCIOPRAM = 57,
	SL_FREEIOPRAM = 58,
	SL_INITMOVIESOUND = 59,
	SL_CLOSEMOVIESOUND = 60,
	SL_RESETMOVIESOUND = 61,
	SL_STARTMOVIESOUND = 62,
	SL_PAUSEMOVIESOUND = 63,
	SL_GETTRANSSTATUS = 64,
	SL_GETSTICK = 65,
	SL_GETVOICEALLOC = 66,
	SL_LOCKVALLOC = 67,
	SL_EXTERNVALLOC = 68,
	SL_EXTERNVFREE = 69,
	SL_UNLOCKVALLOC = 70,
	SL_SRAMMALLOC = 71,
	SL_SRAMMARKUSED = 72,
	SL_SRAMFREE = 73,
	SL_SRAMGETFREE = 74,
	SL_SRAMMAXFREE = 75,
	SL_EXTERNCALL = 76,
	SL_COMMAND_BATCH = 77,
	SL_SETGROUPVOICERANGE = 78,
	SL_ISVAGSTREAMBUFFERED = 79,
	SL_SETREVERBEX = 80,
	SL_PREALLOCREVERBWORKAREA = 81,
	SL_GETLASTLOADERROR = 82,
	SL_SETPANMODE = 83,
	SL_PLAYSOUNDBYNAME = 84,
	SL_PLAYSOUNDBYNAME_A = 85,
	SL_GETSOUNDUD = 86,
	SL_BANKLOADFROMEE = 87,
	SL_SETMOVIESOUNDVOLPAN = 88,
	SL_BANKLOADFROMIOP = 89,
	SL_UPDATEMOVIEADPCM = 90,
	SL_GETMOVIENAX = 91,
	SL_PLAYSOUNDEX = 92,
	SL_GETSOUNDREG = 93,
	SL_SETSOUNDREG = 94,
	SL_SETALLSOUNDREG = 95,
	SL_SETMASTERVOLUMEDUCKER = 96,
	SL_STOPALLSOUNDSINGROUP = 97,
	SL_SETVAGSTREAMCHANNELRANGE = 98,
	SL_SETVAGSTREAMSUBGROUPVOLPAN = 99,
	SL_SETREVERBMODE = 100,
	SL_CDSTATUS = 101,
	SL_GETSFXGLOBALREG = 102,
	SL_SETSFXGLOBALREG = 103,
	SL_EXTERNCALLWITHDATA = 104,
	SL_STREAMSAFECDSEARCHFILE = 105
};

typedef void (*SpuTransferCallbackProc)(/* parameters unknown */);
typedef void (*CommandHandler)(/* parameters unknown */);
typedef SInt32 (*LFOFunction)(/* parameters unknown */);
typedef SInt32 (*GrainHandler)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ int *thid;
	/* 0x4 */ int compare;
} timercommon;

typedef struct { // 0x10
	/* 0x0:0 */ unsigned int psize : 8;
	/* 0x1:0 */ unsigned int dsize : 24;
	/* 0x4 */ unsigned int daddr;
	/* 0x8 */ unsigned int fcode;
	/* 0xc */ unsigned int opt;
} sceSifCmdHdr;

typedef void (*sceSifCmdHandler)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ sceSifCmdHandler func;
	/* 0x4 */ void *data;
} sceSifCmdData;

typedef struct { // 0x14
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ void *newaddr;
} sceSifCmdCSData;

typedef struct { // 0x18
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ int rno;
	/* 0x14 */ unsigned int value;
} sceSifCmdSRData;

typedef struct { // 0x68
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ int size;
	/* 0x14 */ int flag;
	/* 0x18 */ char arg[80];
} sceSifCmdResetData;

typedef char *__gnuc_va_list;

enum {
	__no_type_class = -1,
	__void_type_class = 0,
	__integer_type_class = 1,
	__char_type_class = 2,
	__enumeral_type_class = 3,
	__boolean_type_class = 4,
	__pointer_type_class = 5,
	__reference_type_class = 6,
	__offset_type_class = 7,
	__real_type_class = 8,
	__complex_type_class = 9,
	__function_type_class = 10,
	__method_type_class = 11,
	__record_type_class = 12,
	__union_type_class = 13,
	__array_type_class = 14,
	__string_type_class = 15,
	__set_type_class = 16,
	__file_type_class = 17,
	__lang_type_class = 18
};

typedef __gnuc_va_list va_list;

struct sSRAMNode_tag { // 0x18
	/* 0x00 */ UInt32 loc;
	/* 0x04 */ UInt32 size;
	/* 0x08 */ UInt32 in_use;
	/* 0x0c */ sSRAMNode_tag *root;
	/* 0x10 */ sSRAMNode_tag *smaller;
	/* 0x14 */ sSRAMNode_tag *bigger;
};

typedef sSRAMNode_tag sSRAMNode;
typedef sSRAMNode_tag *sSRAMNodePtr;

typedef struct { // 0x4
	/* 0x0 */ SInt16 min;
	/* 0x2 */ SInt16 max;
} StreamChannelRange;

typedef struct { // 0x4
	/* 0x0 */ SInt16 min;
	/* 0x2 */ SInt16 max;
} VoiceRange;
