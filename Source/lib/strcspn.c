/*
	File: Source/lib/strcspn.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

size_t strcspn(char *d, const char *s)
{
	char *tmp = d;
	const char *bgn = s;

	while ('\0' != *d) {
		while ('\0' != *s)
			if (*d == *s++)
				return d - tmp;
		d++;
		s = bgn;
	}

	return d - tmp;
}
