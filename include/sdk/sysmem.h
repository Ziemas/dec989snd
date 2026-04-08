#ifndef _SYSMEM_H
#define _SYSMEM_H

#ifndef NULL
#define NULL (void*)0
#endif

#define SMEM_Low	(0)
#define SMEM_High	(1)
#define SMEM_Addr	(2)

extern void *AllocSysMemory(int type, unsigned long size, void *addr);
extern int FreeSysMemory(void *area);
extern unsigned long QueryMemSize(void);
extern unsigned long QueryMaxFreeMemSize(void);
extern unsigned long QueryTotalFreeMemSize(void);
extern void *QueryBlockTopAddress(void *addr);
extern unsigned long QueryBlockSize(void *addr);

extern void *Kprintf(const char *format, ...);

#endif /* _SYSMEM_H */
