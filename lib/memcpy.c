/*
	File: Source/MitOS/lib/memcpy.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

void *memcpy(void *d, void *s, size_t sz)
{
	void *tmp = d;
	char *dp = (char*)d;
	char *sp = (char*)s;

	while (sz--)
		*dp++ = *sp++;

	return tmp;
}
