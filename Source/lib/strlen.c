/*
	File: Source/lib/strlen.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>

size_t strlen(char *d)
{
	char *tmp = d;

	while ('\0' != *d)
		d++;

	return d - tmp;
}
