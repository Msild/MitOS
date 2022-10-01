/*
	File: Source/console/arg.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define isalpha(ch) (unsigned int) ((ch | 0x20) - 'a') < 26u

void string_split(char *src, const char *separator, char **dest, int *num)
{
	char *pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0) {
		return;
	}
	if (separator == NULL || strlen(separator) == 0) 	{
		return;
	}
	pNext = strtok(src, separator);
	while (pNext != NULL) {
		*dest++ = pNext;
		++count;
		pNext = strtok(NULL, separator);
	}
	*num = count;
}

int check_command(struct ARG arg, char *cmd)
{
	if (strcmp(strlwr(arg.argv[0]), cmd) == 0) {
		return 1;
	} else {
		return 0;
	}
}

int check_switch(char *ds, char *sw, unsigned int len)
{
	if (strncmp(strlwr(ds), sw, len) == 0 && !isalpha(ds[len])) {
		return 1;
	}
	return 0;
}

void string_spacefilter(char *p)
{
	int i = 0;
	while (*(p + i)) {
		if (!(*(p + i) == 32 && *(p + i + 1) == 32)) {
			i++;
		} else {
			strcpy(p + i, p + i + 1);
		}
	}
}

int cmdline_split(char *cmdline, struct ARG *arg)
{
	string_spacefilter(cmdline);
	string_split(cmdline, " ", arg->argv, &arg->argc);
	return 0;
}
