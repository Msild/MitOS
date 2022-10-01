/*
	File: Source/MitOS/lib/strstr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdio.h>
#include <stddef.h>

char *strstr(char *d, const char *s)
{
	char *tmp;
	const char *bgn = s;

	while ('\0' != *d) {
		tmp = d;
		while (*d++ == *s++)
			if ('\0' == *d || '\0' == *s)
				return tmp;
		d = tmp+1;
		s = bgn;
	}

	return NULL;
}
