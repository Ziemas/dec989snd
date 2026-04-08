#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#define TH_SELF		0
#define TPRI_RUN	0
 
/*
 * Thread priority convention
 *
 *  HIGHEST .. HIGHSYS_RESERVED		reserved for debug system
 *  MODULE_INIT-3 .. MODULE_INIT	for module manager
 *  MODULE_INIT				for each module initialize routine
 *
 *  USER_HIGHEST..USER_LOWEST		for user & drivers
 *
 *  LOWSYS_RESERVED .. LOWEST		reserved for debug system
 */

#define	LOWEST_PRIORITY			(32*4-2)
#define	HIGHEST_PRIORITY		1

#define	HIGHSYS_RESERVED_PRIORITY	4
#define	MODULE_INIT_PRIORITY		8
#define	USER_HIGHEST_PRIORITY		9

#define	LOWSYS_RESERVED_PRIORITY	(LOWEST_PRIORITY-2)
#define	USER_LOWEST_PRIORITY		(LOWSYS_RESERVED_PRIORITY-1)


struct SysClock {
    u_int	low;
    u_int	hi;
};

/* For Thread Management */
struct ThreadParam {
    u_int	attr;		/* °�� 			*/
    u_int	option;		/* �桼�����ղþ���  		*/
    void	*entry;		/* ����ȥꥢ�ɥ쥹 		*/
    int		stackSize;	/* �����å������� 		*/
    int		initPriority;	/* ͥ���� ����� 		*/
};

struct ThreadInfo {
    u_int	attr;		/* °�� 			*/
    u_int	option;		/* �桼�����ղþ���  		*/
    int		status;		/* ����åɤξ��� 		*/
    void	*entry;		/* ����ȥꥢ�ɥ쥹 		*/
    void	*stack;		/* �����å��ΰ����Ƭ 		*/
    int		stackSize;	/* �����å������� 		*/
    void	*gpReg;		/* gp �쥸������ 		*/
    int		initPriority;	/* ͥ���� ����� 		*/
    int		currentPriority;/* ͥ���� ������ 		*/
    int		waitType;	/* �Ԥ����֤μ���		*/
    int		waitId;		/* waittype ���Ԥ��оݤ� ID	*/
    int		wakeupCount;	/* ̤������ wakeup ���		*/
    u_long	*regContext;
    int		reserved1;
    int		reserved2;
    int		reserved3;
    int		reserved4;
};

typedef struct ThreadRunStatus {
    int		status;		/* ����åɤξ��� 		*/
    int		currentPriority;/* ͥ���� ������ 		*/
    int		waitType;	/* �Ԥ����֤μ���		*/
    int		waitId;		/* waittype ���Ԥ��оݤ� ID	*/
    int		wakeupCount;	/* ̤������ wakeup ���		*/
    u_long	*regContext;
    struct SysClock runClocks;		/* �¹Ի���                     */
    u_int	intrPreemptCount;	/* ����߽����Ǽ¹Ը���å��줿��� */
    u_int	threadPreemptCount;	/* �¹Ը���å��줿���         */
    u_int	releaseCount;		/* �¹Ը�������������         */
} ThreadRunStatus;

/* ThreadInfo.status */
#define	THS_RUN		0x01
#define THS_READY	0x02
#define THS_WAIT	0x04
#define THS_SUSPEND	0x08
#define THS_WAITSUSPEND	0x0c
#define THS_DORMANT	0x10
#define THS_DEAD	0x20

/* ThreadInfo.waitType */
#define TSW_SLEEP	1	/* SleepThread()�ˤ���Ԥ�����		*/
#define TSW_DELAY	2	/* DelayThread()�ˤ���Ԥ�����		*/
#define TSW_SEMA	3	/* ���ޥե����Ԥ�����			*/
#define TSW_EVENTFLAG	4	/* ���٥�ȥե饰���Ԥ�����		*/
#define TSW_MBX		5	/* ��å������ܥå������Ԥ�����		*/
#define TSW_VPL		6	/* ����Ĺ����֥��å��������Ԥ�����	*/
#define TSW_FPL		7	/* ����Ĺ����֥��å��������Ԥ�����	*/

/* ThreadParam.attr, ThreadInfo.attr */
#define TH_ASM		0x01000000
#define TH_C		0x02000000
#define TH_UMODE	0x00000008
#define TH_NO_FILLSTACK	0x00100000
#define TH_CLEAR_STACK	0x00200000

extern int CreateThread( struct ThreadParam *param );
extern int DeleteThread( int thid );
extern int StartThread( int thid, u_long arg );
extern int StartThreadArgs( int thid, int args, void *argp );
extern int ExitThread(void);
extern int ExitDeleteThread(void);
extern int TerminateThread( int thid );
extern int iTerminateThread( int thid );
/*
 *	extern int DisableDispatchThread(void);
 *	extern int EnableDispatchThread(void);
 */
extern int ChangeThreadPriority( int thid, int priority );
extern int iChangeThreadPriority( int thid, int priority );
extern int RotateThreadReadyQueue( int priority );
extern int iRotateThreadReadyQueue( int priority );
extern int ReleaseWaitThread( int thid );
extern int iReleaseWaitThread( int thid );
extern int GetThreadId(void);
extern int GetThreadCurrentPriority(void);
extern int CheckThreadStack(void);
extern int GetThreadStackFreeSize(int thid);
extern int ReferThreadStatus( int thid, struct ThreadInfo *info );
extern int iReferThreadStatus( int thid, struct ThreadInfo *info );
extern int ReferThreadRunStatus( int thid, struct ThreadRunStatus *stat,
				 size_t statsize );
/* ------ Ʊ�����̿���ǽ ------ */

/* XXXX->attr common */
#define AT_THFIFO	0
#define AT_THPRI	1
#define AT_SINGLE	0
#define AT_MULTI	2
#define AT_MSFIFO	0
#define AT_MSPRI	4

/* --- Thread direct --- */
extern int SleepThread(void);
extern int WakeupThread( int thid );
extern int iWakeupThread( int thid );
extern int CancelWakeupThread( int thid );
extern int iCancelWakeupThread( int thid );
/*
 *	extern int SuspendThread( int thid );
 *	extern int iSuspendThread( int thid );
 *	extern int ResumeThread( int thid );
 *	extern int iResumeThread( int thid );
 */

/* --- Semaphore --- */
struct SemaParam {
    u_int	attr;		/* ���ޥե���°��		*/
    u_int	option;		/* �桼�����ղþ���  		*/
    int		initCount;	/* ���ޥե��ν����		*/
    int		maxCount;	/* ���ޥե��κ�����		*/
};

struct SemaInfo {
    u_int	attr;		/* ���ޥե���°��		*/
    u_int	option;		/* �桼�����ղþ���  		*/
    int		initCount;	/* ���ޥե��ν����		*/
    int		maxCount;	/* ���ޥե��κ�����		*/
    int		currentCount;	/* ���ޥե��θ�����		*/
    int		numWaitThreads;	/* ���ޥե��Ԥ�����åɿ�	*/
    int		reserved1;
    int		reserved2;
};

/* SemaParam->attr, SemaInfo->attr */
#define SA_THFIFO	AT_THFIFO
#define SA_THPRI	AT_THPRI
#define SA_IHTHPRI	0x100

extern int CreateSema( struct SemaParam *param );
extern int DeleteSema( int semid );
extern int SignalSema( int semid );
extern int iSignalSema( int semid );
extern int WaitSema( int semid );
extern int PollSema( int semid );
extern int ReferSemaStatus( int semid, struct SemaInfo *info );
extern int iReferSemaStatus( int semid, struct SemaInfo *info );

/* --- EventFlag --- */
struct EventFlagParam {
    u_int	attr;		/* ���٥�ȥե饰��°��		*/
    u_int	option;		/* �桼�����ղþ���  		*/
    u_int	initPattern;	/* ���٥�ȥե饰�ν����	*/
};

struct EventFlagInfo {
    u_int	attr;		/* ���٥�ȥե饰��°��		*/
    u_int	option;		/* �桼�����ղþ���  		*/
    u_int	initPattern;	/* ���٥�ȥե饰�ν����	*/
    u_int	currentPattern;	/* ���٥�ȥե饰�θ�����	*/
    int		numWaitThreads;	/* ���٥�ȥե饰�Ԥ�����åɿ� */
    int		reserved1;
    int		reserved2;
};

/* EventFlagParam->attr, EventFlagInfo->attr */
#define EA_SINGLE	AT_SINGLE
#define EA_MULTI	AT_MULTI

/* waitmode */
#define EW_AND		0
#define EW_OR		1
#define EW_CLEAR	0x10

extern int CreateEventFlag( struct EventFlagParam *param );
extern int DeleteEventFlag( int evfid );
extern int SetEventFlag( int evfid, u_long bitpattern );
extern int iSetEventFlag( int evfid, u_long bitpattern );
extern int ClearEventFlag( int evfid, u_long bitpattern );
extern int iClearEventFlag( int evfid, u_long bitpattern );
extern int WaitEventFlag( int evfid, u_long bitpattern, int waitmode,
			   u_long *resultpat );
extern int PollEventFlag( int evfid, u_long bitpattern, int waitmode,
		   u_long *resultpat );
extern int ReferEventFlagStatus( int evfid, struct EventFlagInfo *info );
extern int iReferEventFlagStatus( int evfid, struct EventFlagInfo *info );

/* --- MessageBox --- */
struct MbxParam {
    u_int	attr;		/* ��å������ܥå�����°��	*/
    u_int	option;		/* �桼�����ղþ���  		*/
};

struct MbxInfo {
    u_int	attr;		/* ��å������ܥå�����°��	*/
    u_int	option;		/* �桼�����ղþ���  		*/
    int		numWaitThreads;	/* ��å������Ԥ�����åɿ�	*/
    int		numMessage;	/* ������Ԥ���å�������	*/
    struct MsgPacket	*topPacket; /* ������Ԥ���Ƭ��å�����	*/
    int		reserved1;
    int		reserved2;
};

struct MsgPacket {
    struct MsgPacket *next;
    u_char	      msgPriority;
    u_char	      dummy[3];
};

/* MbxParam->attr, MbxInfo->attr */
#define MBA_THFIFO	AT_THFIFO
#define MBA_THPRI	AT_THPRI
#define MBA_MSFIFO	AT_MSFIFO
#define MBA_MSPRI	AT_MSPRI

extern int CreateMbx( struct MbxParam *param );
extern int DeleteMbx( int mbxid );
extern int SendMbx( int mbxid, struct MsgPacket *sendmsg );
extern int iSendMbx( int mbxid, struct MsgPacket *sendmsg );
extern int ReceiveMbx( struct MsgPacket **recvmsg, int mbxid );
extern int PollMbx( struct MsgPacket **recvmsg, int mbxid );
extern int ReferMbxStatus( int mbxid, struct MbxInfo *info );
extern int iReferMbxStatus( int mbxid, struct MbxInfo *info );

/* --- Variable zize memory pool --- */
struct VplParam {
    u_int	attr;		/* ����Ĺ����ס����°��		*/
    u_int	option;		/* �桼�����ղþ���			*/
    int		size;		/* ����Ĺ����ס���Υ�����		*/
};

struct VplInfo {
    u_int	attr;		/* ����Ĺ����ס����°��		*/
    u_int	option;		/* �桼�����ղþ���  			*/
    int		size;		/* ����Ĺ����ס���Υ�����		*/
    int		freeSize;	/* ����Ĺ����ס����̤���ѥ�����	*/
    int		numWaitThreads;	/* �����Ԥ�����åɿ�			*/
    int		reserved1;
    int		reserved2;
    int		reserved3;
};

/* VplParam.attr, VplInfo.attr */
#define VA_THFIFO	AT_THFIFO
#define VA_THPRI	AT_THPRI
#define VA_MEMBTM	0x200

extern int  CreateVpl( struct VplParam *param );
extern int  DeleteVpl( int vplid );
extern void *AllocateVpl( int vplid, int size );
extern void *pAllocateVpl( int vplid, int size );
extern void *ipAllocateVpl( int vplid, int size );
extern int  FreeVpl( int vplid, void *block );
extern int  ReferVplStatus( int vplid, struct VplInfo *info );
extern int  iReferVplStatus( int vplid, struct VplInfo *info );

/* --- Fixed zize memory pool --- */
struct FplParam {
    u_int	attr;		/* ����Ĺ����ס����°��		*/
    u_int	option;		/* �桼�����ղþ���  			*/
    int		blockSize;	/* ����Ĺ����ס���Υ֥��å�������	*/
    int		numBlocks;	/* ����Ĺ����ס���Υ֥��å���	*/
};

struct FplInfo {
    u_int	attr;		/* ����Ĺ����ס����°��		*/
    u_int	option;		/* �桼�����ղþ���  			*/
    int		blockSize;	/* ����Ĺ����ס���Υ֥��å�������	*/
    int		numBlocks;	/* ����Ĺ����ס���Υ֥��å���	*/
    int		freeBlocks;	/* ����Ĺ����ס����̤���ѥ֥��å�	*/
    int		numWaitThreads;	/* �����Ԥ�����åɿ�			*/
    int		reserved1;
    int		reserved2;
    int		reserved3;
    int		reserved4;
};

/* FplParam.attr, FplInfo.attr */
#define FA_THFIFO	AT_THFIFO
#define FA_THPRI	AT_THPRI
#define FA_MEMBTM	0x200

extern int  CreateFpl( struct FplParam *param );
extern int  DeleteFpl( int fplid );
extern void *AllocateFpl( int fplid );
extern void *pAllocateFpl( int fplid );
extern void *ipAllocateFpl( int fplid );
extern int FreeFpl( int fplid, void *block );
extern int ReferFplStatus( int fplid, struct FplInfo *info );
extern int iReferFplStatus( int fplid, struct FplInfo *info );

/* ����/�����޴�����ǽ */
extern int DelayThread(unsigned int usec);

typedef u_int (*AlarmHandler)(void*);

extern int GetSystemTime(struct SysClock *clock);
extern u_int GetSystemTimeLow(void);

extern int SetAlarm( struct SysClock *clock, AlarmHandler handler,
		     void *common);
extern int iSetAlarm( struct SysClock *clock, AlarmHandler handler,
		      void *common);
extern int CancelAlarm( AlarmHandler handler, void *common );
extern int iCancelAlarm( AlarmHandler handler, void *common );

extern void USec2SysClock(unsigned int usec, struct SysClock *clock);
extern void SysClock2USec(struct SysClock *clock, int *sec, int *usec);

/* �����ƥ���֤μ�����ǽ */

typedef struct SystemStatus {
    u_int		status;
    int			systemLowTimerWidth;	/* 32 bit or 16 bit */
    struct SysClock	idleClocks;
    struct SysClock	kernelClocks;		/* not yet. reserved */
    u_int		comesOutOfIdleCount;	/* comes out of idle count */
    u_int		threadSwitchCount;	/* total thread switch count */
    u_int		reserved[8];
} SystemStatus;

/* SystemStatus->status */
#define	TSS_THREAD		0
#define	TSS_DISABLEDISPATCH	1
#define	TSS_DISABLEINTR		3
#define	TSS_NOTHREAD		4

extern int ReferSystemStatus( struct SystemStatus *info, size_t infosize );
extern int GetThreadmanIdList(int type,
			      int *readbuf, int readbufsize, int *objectcount);
enum IdListType {
    TMID_Thread = 1,
    TMID_Semaphore,
    TMID_EventFlag,
    TMID_Mbox,
    TMID_Vpl,
    TMID_Fpl,
    TMID_SleepThread,
    TMID_DelayThread,
    TMID_DormantThread
};


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* _THREAD_H_ */
