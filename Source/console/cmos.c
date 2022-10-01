/*
	File: Source/console/cmos.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

unsigned char cmos_read(unsigned char p)
{
	unsigned char data;
	io_out8(CMOS_INDEX, p);
	data = io_in8(CMOS_DATA);
	io_out8(CMOS_INDEX, 0x80);
	return data;
}

void cmos_write(unsigned char p, unsigned char data)
{
	io_out8(CMOS_INDEX, p);
	io_out8(CMOS_DATA, data);
	return;
}
