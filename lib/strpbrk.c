/*
	File: Source/MitOS/lib/strpbrk.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdio.h>
#include <stddef.h>

char *strpbrk(char *d, const char *s)
{
	const char *bgn = s;

	while ('\0' != *d) {
		while ('\0' != *s)
			if (*d == *s++)
				return d;
		d++;
		s = bgn;
	}

	return NULL;
}
