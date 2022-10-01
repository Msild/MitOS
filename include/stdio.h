/*
	File: Source/MitOS/include/stdio.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef STDIO_H

#define STDIO_H

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef NULL
	#define NULL	((void *) 0)
#endif

#include <stdarg.h>

int sprintf(char *s, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);

#ifdef __cplusplus
	}
#endif

#endif
