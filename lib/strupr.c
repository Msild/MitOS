/*
	File: Source/MitOS/lib/strupr.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stddef.h>
#include <string.h>

char *strupr(char *str)
{
	if (str == NULL) {
		return NULL;
	}
	static char buf[1024];
	strcpy(buf, str);
	char *p = buf;
	while (*p != '\0') {
		if ('a' <= *p && *p <= 'z') {
			*p = (*p) - 0x20;
		}
		p++;
	}
	return buf;
}
