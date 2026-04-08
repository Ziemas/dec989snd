#ifndef _LIBCDVD_H
#define _LIBCDVD_H

#ifndef _eetypes_h_
#include "types.h"
#endif

/*
 * CD-ROM Basic System
 */

/* Flag for sceOpen() CDVD local usage */
#define SCE_CdSTREAM      0x40000000  /* CD/DVD Media Stream open */
#define SCE_CdCACHE       0x10000000  /* CD/DVD Media Cache  open */

/*
 * Macros for READ Data pattan
 */
#define SCECdSecS2048		0	/* sector size 2048 */
#define SCECdSecS2328		1	/* sector size 2328 System Use */
#define SCECdSecS2340		2	/* sector size 2340 System Use */

/*
 * Macros for CDDA READ Data pattan
 */
#define SCECdSecS2352           0       /* sector size 2352  CD-DA read */

/*
 * Macros for Spindle control
 */
#define SCECdSpinMax		0
#define SCECdSpinNom		1	/* driver�̓ǂ݂�����ō����x�ŉ� */
#define SCECdSpinStm		0	/* �X�g���[���̐������x�ŉ�       */
#define SCECdSpinDvdDL0		0	/* DVD�f���A�����C�A0�������x�ŉ�  */	
#define SCECdSpinX1             2       /* �W�����ŉ� CD-DA read          */

/*
 * Macros for Error code
 */
#define SCECdErFAIL             -1      /* sceCdGetError()�֐���
                                                        ���s�Ɏ��s����      */
#define SCECdErNO		0x00    /* No Error                         */
#define SCECdErEOM		0x32    /* �Đ����ɍŊO���ɒB����	    */
#define SCECdErTRMOPN		0x31	/* �Đ����ɊW��������ꂽ  	    */
#define SCECdErREAD		0x30	/* �ǂ݂�������肪�ɔ�������	    */
#define SCECdErPRM		0x22	/* ��ʃR�}���h�̃p�����[�^�Ɍ��   */
#define SCECdErILI		0x21	/* �]�����w�肪���		    */
#define SCECdErIPI		0x20	/* �]���J�n�A�h���X�w�肪���	    */
#define SCECdErCUD		0x14	/* �h���C�u���̃f�B�X�N�ɂ͕s�K��   */
#define SCECdErNORDY		0x13    /* �R�}���h������		    */
#define SCECdErNODISC		0x12	/* �f�B�X�N������		    */
#define SCECdErOPENS		0x11	/* �W���J���Ă���		    */
#define SCECdErCMD		0x10	/* ���T�|�[�g�̃R�}���h���s	    */
#define SCECdErABRT		0x01	/* �R�}���h�������ɃA�{�[�g���s	    */
#define SCECdErREADCF		0xfd	/* �ǂ݂����R�}���h���s�Ɏ��s�����B */
#define SCECdErREADCFR  	0xfe	/* �ǂ݂����R�}���h���s�Ɏ��s�����B */
#define SCECdErSFRMTNG		0x38    /* �R�}���h�ƃZ�N�^�̃t�H�[�}�b�g�� */
					/* �قȂ�B			    */

/*
 * Macros for sceCdGetDiskType()
 */
#define SCECdIllgalMedia 	0xff
#define SCECdIllegalMedia 	0xff
	/* ILIMEDIA (Illegal Media)
            Disc �� ��PS/��PS2 Disc �ł��邱�Ƃ�\�킷�B */
#define SCECdDVDV		0xfe
	/* DVDV (DVD Video)
            Disc �� ��PS/��PS2 �ł��� DVD Video �ł��邱�Ƃ�\�킷�B*/
#define SCECdCDDA		0xfd
	/* CDDA (CD DA)
            Disc �� ��PS/��PS2 �ł��� DA �g���b�N�܂ނ��Ƃ�\�킷�B*/
#define SCECdPS2DVD		0x14
	/* PS2DVD  Disc �� PS2 �p DVD �ł��邱�Ƃ�\�킷�B*/
#define SCECdPS2CDDA		0x13
	/* PS2CDDA Disc �� PS2 �p CD �ł��� DA �g���b�N�܂ނ��Ƃ�\�킷�B*/
#define SCECdPS2CD		0x12
	/* PS2CD   Disc �� PS2 �p CD �ł��� DA �g���b�N�܂܂Ȃ����Ƃ�\�킷�B*/
#define SCECdPSCDDA 		0x11
	/* PSCDDA  Disc �� PS �p CD �ł��� DA �g���b�N�܂ނ��Ƃ�\�킷�B */
#define SCECdPSCD		0x10
	/* PSCD    Disc �� PS �p CD �ł��� DA �g���b�N�܂܂Ȃ����Ƃ�\�킷�B*/
#define SCECdDETCT		0x01
	/* DETCT (Detecting) Disc ���ʓ��쒆��\�킷�B*/
#define SCECdNODISC 		0x00
	/* NODISC (No disc) Disc �������Ă��Ȃ���Ԃ�\�킷�B*/
#define SCECdUNKNOWN		0x05
	/* UNKNOWN          Disc ���ʕs�\	*/
#define SCECdGDTFUNCFAIL        (-1)
    /* FUNCFAIL			�֐��Ăяo�����s */


/*
 * CD-ROM Primitive Commands
 */

/*
 * Interrupts
 */
#define CdlNoIntr	0x00	/* No interrupt		  */
#define CdlDataReady	0x01	/* Data Ready		  */
#define SCECdComplete	0x02	/* Command Complete 	  */
#define CdlAcknowledge	0x03	/* Acknowledge (reserved) */
#define CdlDataEnd	0x04	/* End of Data Detected   */
#define CdlDiskError	0x05	/* Error Detected 	  */
#define SCECdNotReady	0x06	/* Drive Not Ready	  */

/*
 * Status Contents
 */
#define SCECdStatShellOpen      0x01    /* once shell open 	  */
#define SCECdStatStop	        0x00    /* Stop		 	  */
#define SCECdStatSpin           0x02    /* spindle motor rotating */
#define SCECdStatRead           0x06    /* reading data sectors   */
#define SCECdStatPause          0x0a    /* pause		  */
#define SCECdStatSeek           0x12    /* seeking 		  */
#define SCECdStatEmg	        0x20    /* error detected	  */

/*
 * Library Macros
 */
#ifndef btoi
#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to u_char */
#endif
#ifndef itob
#define itob(i)		((i)/10*16 + (i)%10)		/* u_char to BCD */
#endif

/*
 *	Position
 */
#define CdlMAXTOC	100

/*
 *	Callback Reason
 */
#define SCECdFuncRead		1
#define SCECdFuncReadCDDA       2
#define SCECdFuncGetToc     3
#define SCECdFuncSeek		4
#define SCECdFuncStandby	5
#define SCECdFuncStop		6
#define SCECdFuncPause		7
#define SCECdFuncBreak          8


typedef void (*CdlCB)(u_char,u_char *);

/*
 *	CD and DVD Read Mode
 */
typedef struct {
	u_char trycount;	/* trycount   */
	u_char spindlctrl;	/* spindlctrl */
	u_char datapattern;	/* datapattern */
	u_char pad;		/* pad        */
} sceCdRMode;

/*
 *	Location
 */
typedef struct {
	u_char minute;		/* minute (BCD) */
	u_char second;		/* second (BCD) */
	u_char sector;		/* sector (BCD) */
	u_char track;		/* track (void) */
} sceCdlLOCCD;

/*
 * CdInit Mode
 */
#define SCECdINIT        0x00    /* disk chk	   */
#define SCECdINoD	 0x01    /* disk no chk    */
#define SCECdEXIT        0x05    /* cdvd exit	   */

/*
 *	Low Level File System for CdSearchFile() 
 */
#define CdlMAXFILE	64	/* max number of files in a directory */
#define CdlMAXDIR	128	/* max number of total directories */
#define CdlMAXLEVEL	8	/* max levels of directories */

typedef struct {
	u_int   lsn;		/* file location */
	u_int	size;		/* file size */
	char	name[16];	/* file name (body) */
	u_char  date[8];	/* date		    */
        u_int   flag;           /* file_flag        */
} sceCdlFILE;

/*
 *      RTC
 */
typedef struct {
        u_char stat;            /* status */
        u_char second;          /* second */
        u_char minute;          /* minute */
        u_char hour;            /* hour   */

        u_char pad;             /* pad    */
        u_char day;             /* day    */
        u_char month;           /* month  */
        u_char year;            /* year   */
} sceCdCLOCK;

/*
 *      Stream
 */
/*  Stream mode  */
#define STMNBLK         0
#define STMBLK          1
/*  Stream Init Devctl() */
typedef struct {
        u_int bufmax;
        u_int bankmax;
        u_int iop_bufaddr;
} sceCdStmInit;

/*
 *      Media mode
 */
#define SCECdCD         1
#define SCECdDVD        2

/*
 * Macros for sceCdTrayReq
 */
#define SCECdTrayOpen	0	/* Tray Open  */
#define SCECdTrayClose	1	/* Tray Close */
#define SCECdTrayCheck	2	/* Tray Check */

/*
 *      EE Read mode
 */
#define SCECdNoCheckReady       0x00000001
#define SCECdNoWriteBackDCache  0x00000002

/*
 *	Prototypes
 */
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

sceCdlLOCCD *sceCdIntToPos(int i, sceCdlLOCCD *p);
int sceCdPosToInt(sceCdlLOCCD *p);
int sceCdSearchFile(sceCdlFILE *fp,const char *name);
int sceCdLayerSearchFile(sceCdlFILE *fp,const char *name, int layer);
int sceCdRead(u_int lbn, u_int sectors,
                         void *buf , sceCdRMode *mode);
int sceCdReadIOPm(u_int lbn, u_int sectors,
                         void *buf , sceCdRMode *mode);
int sceCdReadChain(u_int *read_tag, sceCdRMode *mode);
int *sceCdCallback(void (*func)(int));
int *sceCdPOffCallback(void (*func)(void *),void *addr);
int sceCdTrayReq(int param,u_int *traychk);
u_int sceCdGetReadPos( void );
int sceCdSync(int mode);
int sceCdInit(int init_mode);
int sceCdInitEeCB(int cb_prio, void *stack_addr,int stack_size);
int sceCdDiskReady( int mode );
int sceCdGetError(void);
int sceCdGetDiskType( void );
int sceCdGetDiskType2(void);

int sceCdStatus( void );
int sceCdBreak( void );

int sceCdGetToc(u_char *toc) ;
int sceCdSeek( u_int lbn );
int sceCdStandby( void );
int sceCdStop( void );
int sceCdPause( void );

int sceCdStInit(u_int bufmax, u_int bankmax,  u_int iop_bufaddr);
int sceCdStStart(u_int lbn, sceCdRMode *mode);
int sceCdStSeek(u_int lbn);
int sceCdStSeekF(u_int lbn);
int sceCdStStop(void);
int sceCdStRead(u_int size, u_int *buf, u_int mode, u_int *err);
int sceCdStPause(void);
int sceCdStResume(void);
int sceCdStStat(void);
int sceCdReadClock( sceCdCLOCK *rtc );
int sceCdMmode( int media );
int sceCdChangeThreadPriority( int prio );
int sceCdPowerOff( int *stat );
u_int sceCdSetEEReadMode(u_int mode);
int sceCdReadDvdDualInfo(int *on_dual, u_int *layer1_start);

void *sceCdGetErxEntries(void);
int sceCdDriveCheckForErxLoading(int *rdy, u_int *dtype);

int sceCdReadCDDA(u_int lbn, u_int sectors, void *buf , sceCdRMode *mode);
int sceCdCtrlADout( int param, int *stat );
int sceCddaStInit(u_int bufmax, u_int bankmax,
                                 u_int iop_bufaddr, int datapattern);
int sceCddaStStart(u_int lbn, sceCdRMode *mode);
int sceCddaStSeek(u_int lbn);
int sceCddaStSeekF(u_int lbn);
int sceCddaStStop(void);
int sceCddaStRead(u_int size, u_int *buf, u_int mode, u_int *err);
int sceCddaStStat(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif /* _LIBCD_H_ */
#endif
