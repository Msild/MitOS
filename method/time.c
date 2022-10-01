/*
	File: Source/MitOS/method/time.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

unsigned int get_hour(void)
{
	return BCD_HEX(cmos_read(CMOS_CUR_HOUR));
}

void set_hour(unsigned int hour_hex)
{
	cmos_write(CMOS_CUR_HOUR, HEX_BCD(hour_hex));
	return;
}

unsigned int get_minute(void)
{
	return BCD_HEX(cmos_read(CMOS_CUR_MIN));
}

void set_minute(unsigned int min_hex)
{
	cmos_write(CMOS_CUR_MIN, HEX_BCD(min_hex));
	return;
}

unsigned int get_second(void)
{
	return BCD_HEX(cmos_read(CMOS_CUR_SEC));
}

void set_second(unsigned int sec_hex)
{
	cmos_write(CMOS_CUR_SEC, HEX_BCD(sec_hex));
	return;
}

unsigned int get_day_of_month(void)
{
	return BCD_HEX(cmos_read(CMOS_MON_DAY));
}

void set_day_of_month(unsigned int day_of_month)
{
	cmos_write(CMOS_MON_DAY, HEX_BCD(day_of_month));
	return;
}

unsigned int get_day_of_week(void)
{
	return BCD_HEX(cmos_read(CMOS_WEEK_DAY));
}

void set_day_of_week(unsigned int day_of_week)
{
	cmos_write(CMOS_WEEK_DAY, HEX_BCD(day_of_week));
	return;
}

unsigned int get_month(void)
{
	return BCD_HEX(cmos_read(CMOS_CUR_MON));
}

void set_month(unsigned int mon_hex)
{
	cmos_write(CMOS_CUR_MON, HEX_BCD(mon_hex));
	return;
}

unsigned int get_year(void)
{
	return (BCD_HEX(cmos_read(CMOS_CUR_CEN)) * 100) + 
			BCD_HEX(cmos_read(CMOS_CUR_YEAR)) + 1980;
}

void set_year(unsigned int year)
{
	cmos_write(CMOS_CUR_CEN, HEX_BCD(year / 100));
	cmos_write(CMOS_CUR_YEAR, HEX_BCD((year - 2000)));
	return;
}

unsigned long kmktime(int year0, int mon0, int day, int hour, int min, int sec)
{
	unsigned int mon = mon0, year = year0;
	if (0 >= (int) (mon -= 2)) {
		mon += 12;
		year -= 1;
	}
	return ((((unsigned long)(year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) +
			  year * 365 - 719499) * 24 + hour ) * 60 + min ) * 60 + sec;
}
