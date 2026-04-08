#ifndef	_STDLIB_H
#define	_STDLIB_H

#ifndef _TYPES_H
#include "types.h"
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

extern          long strtol(const char *,char**, int);
extern unsigned long strtoul(const char *, char **, int);

#define atoi(p) (int)strtol(p,NULL, 10)
#define atol(p) strtol(p,NULL, 10)

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	/* _STDLIB_H */

