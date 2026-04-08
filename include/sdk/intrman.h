#ifndef _INTRMAN_H
#define _INTRMAN_H

#ifndef NULL
#define NULL ((void*)0)
#endif

extern int CpuSuspendIntr(int *oldstat);
extern int CpuResumeIntr(int oldstat);
extern int QueryIntrContext(void);

#endif /* _INTRMAN_H */
