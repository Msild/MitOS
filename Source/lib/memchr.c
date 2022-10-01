/*
	File: Source/lib/memchr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdio.h>
#include <stddef.h>

void *memchr(void *d, int c, size_t sz)
{
	char *dp = (char*)d;

	while (sz--) {
		if (c == *dp)
			return dp;
		dp++;
	}

	return NULL;
}
