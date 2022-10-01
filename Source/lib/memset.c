/*
	File: Source/lib/memset.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

void *memset(void *d, int c, size_t sz)
{
	void *tmp = d;
	char *dp = (char*)d;

	while (sz--)
		*dp++ = c;

	return tmp;
}
