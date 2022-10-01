/*
	File: Source/lib/strspn.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

size_t strspn(char *d, const char *s)
{
	char *tmp = d;
	const char *bgn = s;

	while ('\0' != *d) {
		while ('\0' != *s) {
			if (*d == *s)
				break;
			s++;
		}
		if ('\0' == *s)
			return d - tmp;
		d++;
		s = bgn;
	}

	return d - tmp;
}
