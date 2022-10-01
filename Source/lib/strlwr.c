/*
	File: Source/lib/strlwr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>
#include <string.h>

char *strlwr(char *str)
{
	if (str == NULL) {
		return NULL;
	}
	static char buf[1024];
	strcpy(buf, str);
	char *p = buf;
	while (*p != '\0') {
		if ('A' <= *p && *p <= 'Z') {
			*p = (*p) + 0x20;
		}
		p++;
	}
	return buf;
}
