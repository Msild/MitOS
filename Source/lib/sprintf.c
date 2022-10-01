/*
	File: Source/lib/sprintf.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdarg.h>
#include <stdio.h>

int sprintf(char *s, const char *format, ...)
{
	int i;
	va_list ap;

	va_start(ap, format);
	i = vsprintf(s, format, ap);
	va_end(ap);
	return i;
}

