/*
	File: Source/lib/strdup.c
	Copyright: MLworkshop
	Author: Msild
*/

#include <stdlib.h>
#include <string.h>

char *strdup(const char *cs)
{
	char *t = malloc(strlen(cs) + 1);
	if (t)
		strcpy(t, cs);
	return t;
}
