/*
	File: Source/lib/memmove.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

void *memmove(void *d, void *s, size_t sz)
{
	void *tmp = d;
	char *dp = (char*)d;
	char *sp = (char*)s;

	if (dp > sp) {
		dp += sz;
		sp += sz;
		while (sz--)
			*--dp = *--sp;
	} else {
		while (sz--)
			*dp++ = *sp++;
	}

	return tmp;
}
