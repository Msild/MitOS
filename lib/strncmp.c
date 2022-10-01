/*
	File: Source/MitOS/lib/strncmp.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

int	strncmp(char *d, const char *s, size_t sz)
{
	while (sz--) {
		if (*d == '\0')
			goto ret;
		if (*d != *s) {
ret:
			return *d - *s;
		}
		d++;
		s++;
	}
	return 0;
}
