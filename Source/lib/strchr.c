/*
	File: Source/lib/strchr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

char *strchr(char *d, int c)
{
	while (c != *d) {
		if ('\0' == *d++)
			return NULL;
	}

	return d;
}
