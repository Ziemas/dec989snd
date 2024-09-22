#ifndef	_STRING_H
#define	_STRING_H

#ifndef _TYPES_H
#include "types.h"
#endif
#include "memory.h"

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
extern char  *strcat (char *dest,     const char *src);
extern char  *strncat(char *dest,     const char *src, size_t n);
extern int    strcmp (const char *s1, const char *s2);
extern int    strncmp(const char *s1, const char *s2,  size_t n);
extern char  *strcpy (char *dest,     const char *src);
extern char  *strncpy(char *dest,     const char *src, size_t n);
extern size_t strlen (const char *s);
extern char  *index  (const char *s,  int c);
extern char  *rindex (const char *s,  int c);

extern char  *strchr (const char *s,  int c);
extern char  *strrchr(const char *s,  int c);
extern char  *strpbrk(const char *s1, const char *s2);
extern int    strspn (const char *s1, const char *s2);
extern int    strcspn(const char *s1, const char *s2);
extern char  *strtok (char *s1,       const char *s2);
extern char  *strtok_r (char *s1, const char *s2, char **lasts);
extern char  *strstr (const char *s1, const char *s2);
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	/* _STRING_H */

