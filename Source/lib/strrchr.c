/*
	File: Source/lib/strrchr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdio.h>
#include <stddef.h>

char *strrchr(char *d, int c)
{
	char *tmp = d;

	while ('\0' != *d)
		d++;

	while (tmp <= d) {
		if (c == *d)
			return d;
		d--;
	}

	return NULL;
}
