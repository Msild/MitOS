/*
	File: Source/lib/strcat.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

char *strcat(char *d, const char *s)
{
	char *tmp = d;

	while ('\0' != *d)
		d++;

	while ('\0' != *s)
		*d++ = *s++;

	*d = '\0';

	return tmp;
}
