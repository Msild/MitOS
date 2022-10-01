/*
	File: Source/include/string.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef STRING_H

#define STRING_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <stddef.h>		/* size_t */

char *strcpy(char *s, const char *ct);
char *strncpy(char *s, const char *ct, size_t n);
char *strcat(char *s, const char *ct);
char *strncat(char *s, const char *ct, size_t n);
int strcmp(const char *cs, const char *ct);
int strncmp(const char *cs, const char *ct, size_t n);
char *strrchr(const char *cs, int c);
char *strchr(const char *cs, int c);
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);
char *strpbrk(const char *s, const char *accept);
char *strstr(const char *cs, const char *ct);
size_t strlen(const char *cs);
char *strdup(const char *s);
char *strtok(char *s, const char *delim);
char *strlwr(char *str);
char *strupr(char *str);

void *memcpy(void *s, const void *ct, size_t n);
void *memmove(void *s, const void *ct, size_t n);
int memcmp(const void *cs, const void *ct, size_t n);
void *memchr(const void *cs, int c, size_t n);
void *memset(void *s, int c, size_t n);

#ifdef __cplusplus
	}
#endif

#endif
