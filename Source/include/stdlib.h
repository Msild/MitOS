/*
	File: Source/include/stdlib.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef STDLIB_H

#define STDLIB_H

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef NULL
	#define NULL	((void *) 0)
#endif

#include <stdarg.h>
#include <stddef.h>

long strtol(const char *s, const char **endp, int base);
unsigned long int strtoul(const char *str, char **endptr, int base);
int rand(void);
void srand(unsigned int seed);

#ifdef __cplusplus
	}
#endif

#endif
