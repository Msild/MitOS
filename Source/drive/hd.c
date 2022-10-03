/*
	File: Source/drive/hd.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

#define TIMEOUT 					10000

unsigned int hd_read_sector(unsigned long partOffset, unsigned long driveNo, unsigned long lba, unsigned char *buf, int size)
{
	lba += partOffset;

	unsigned int numberOfSector;
	if (size == 0) {
		return 0;
	} else if (size < 512) {
		numberOfSector = 1;
	} else {
		numberOfSector = (size % 512 == 0) ? size / 512 : (size / 512) + 1;
	}

	io_out8(0x01f2, numberOfSector);
	io_out8(0x01f3, lba);
	io_out8(0x01f4, lba >> 8);
	io_out8(0x01f5, (lba >> 16) & 0xff);
	io_out8(0x01f6, 0xe0 | (driveNo << 4));
	io_out8(0x01f7, 0x20);

	unsigned char status = 0;
	int timeout = TIMEOUT;
	do {
		status = io_in8(0x01f7);
		if (timeout-- == 0) {
			return 0;
		}
	} while ((status & 0x8) != 0x8);

	unsigned short i;
	for (i = 0; i < numberOfSector * 512; i += 2) {
		if (i >= size && size != -1) {
			break;
		} else {
			short inChar = io_in16(0x01f0);
			char first = inChar & 0xff;
			char second = inChar >> 8;
			if (i < size || size == -1) {
				buf[i] = first;
			}
			if (i + 1 < size || size == -1) {
				buf[i + 1] = second;
			}
		}
	}

	if (size != -1) {
		return size;
	} else {
		return numberOfSector * 512;
	}
}
