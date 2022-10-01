/*
	File: Source/MitOS/lib/strcmp.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

int strcmp(const char *d, const char *s)
{
	while ('\0' != *d) {
		if (*d != *s)
			return *d - *s;
		d++;
		s++;
	}

	return *d - *s;
}
