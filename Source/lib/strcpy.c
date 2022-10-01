/*
	File: Source/lib/strcpy.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

char *strcpy(char *d, const char *s)
{
	char *tmp = d;
	while ((*d++ = *s++) != '\0');
	return tmp;
}
