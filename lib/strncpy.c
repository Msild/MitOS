/*
	File: Source/MitOS/lib/strncpy.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

char *strncpy(char *d, const char *s, size_t sz)
{
	char *tmp = d;

	while ('\0' != *s) {
		if (0 == sz)
			break;
		sz--;
		*d++ = *s++;
	}

	while (sz--)
		*d++ = '\0';

	return tmp;
}
