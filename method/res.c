/*
	File: Source/MitOS/method/res.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <string.h>

unsigned int *resszie;

struct RES_HEADER {
	char hder[4];
	unsigned int ver;
	unsigned int idnum;
	unsigned char reserve[4];
};

struct RES_LIST {
	unsigned char id[8];
	unsigned int offset;
	unsigned int size;
};

int res_info(int *info, int size, unsigned char *fp)
{
	if (size < 16) {
		return 0;
	}
	struct RES_HEADER *header = (struct RES_HEADER *) fp;
	if (memcmp(header->hder, "MRES", 4) != 0) {
		return 0;
	}
	info[0] = header->ver;
	info[1] = header->idnum;
	return 1;
}

char *res_decode(int *info, int size, unsigned char *fp, char *id, int *rsize)
{
	struct RES_LIST *list;
	int i;
	for (i = 0; i < info[1]; i++) {
		list = (struct RES_LIST *) (fp + 16 + 16 * i);
		if (strncmp(id, list->id, 8) == 0) {
			*rsize = list->size;
			return (fp + list->offset);
		}
	}
	return 0;
}
