/*
	File: Source/MitOS/lib/memcmp.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

int memcmp(const void *d, const void *s, size_t sz)
{
	const char *dp = (const char*) d;
	const char *sp = (const char*) s;

	while (sz--) {
		if (*dp != *sp)
			return *dp - *sp;
		dp++;
		sp++;
	}

	return 0;
}
