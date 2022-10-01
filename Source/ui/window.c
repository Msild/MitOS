/*
	File: Source/ui/window.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

void vrma_make_window(unsigned char *buf, 
				  int xsize, int ysize, char *title, char act)
{
	varm_fill_rectangle(buf, xsize, COL8_555555, 0, 0, xsize - 1, ysize - 1);
	varm_fill_rectangle(buf, xsize, COL8_AAAAAA, 1, 1, xsize - 2, ysize - 2);
	vram_make_wintitle(buf, xsize, title, act);
	return;
}

void vram_make_wintitle(unsigned char *buf, int xsize, char *title, char act)
{
	static char closebtn[14][16] = {
		"                ",
		"                ",
		"                ",
		"    @@    @@    ",
		"     @@  @@     ",
		"      @@@@      ",
		"       @@       ",
		"      @@@@      ",
		"     @@  @@     ",
		"    @@    @@    ",
		"                ",
		"                ",
		"                ",
		"                "
	};
	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = COL8_FFFFFF;
		tbc = COL8_0000AA;
	} else {
		tc = COL8_AAAAAA;
		tbc = COL8_555555;
	}
	varm_fill_rectangle(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	vram_draw_string(buf, xsize, 24, 4, tc, title);
	/* drawicon8_sys(buf, xsize, 4, 4, 5); */
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_AAAAAA;
			} else {
				c = tbc;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void sheet_draw_string(struct SHEET *sht, int x, int y, 
					   int c, int b, char *s, int l)
{
	varm_fill_rectangle(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	vram_draw_string(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void sheet_draw_mstring(struct SHEET *sht, int x, int y, 
							int c, int b, char *s, int l)
{
	varm_fill_rectangle(sht->buf, sht->bxsize, b, x, y, x + l * 6 - 1, y + 7);
	varm_draw_mstring(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x + l * 6, y + 8);
}

void vram_make_textbox(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
	int x1 = x0 + sx, y1 = y0 + sy;
	varm_fill_rectangle(sht->buf, sht->bxsize, COL8_555555, x0 - 3, y0 - 3, x1 + 3, y1 + 3);
	varm_fill_rectangle(sht->buf, sht->bxsize, c          , x0 - 2, y0 - 2, x1 + 2, y1 + 2);
	return;
}

void vram_change_wintitle(struct SHEET *sht, char act)
{
	int x, y, xsize = sht->bxsize;
	char c, tc_new, tbc_new, tc_old, tbc_old, *buf = sht->buf;
	if (act != 0) {
		tc_new  = COL8_FFFFFF;
		tbc_new = COL8_0000AA;
		tc_old  = COL8_AAAAAA;
		tbc_old = COL8_555555;
	} else {
		tc_new  = COL8_AAAAAA;
		tbc_new = COL8_555555;
		tc_old  = COL8_FFFFFF;
		tbc_old = COL8_0000AA;
	}
	for (y = 3; y <= 20; y++) {
		for (x = 3; x <= xsize - 4; x++) {
			c = buf[y * xsize + x];
			if (c == tc_old && x <= xsize - 22) {
				c = tc_new;
			} else if (c == tbc_old) {
				c = tbc_new;
			}
			buf[y * xsize + x] = c;
		}
	}
	sheet_refresh(sht, 3, 3, xsize, 21);
	return;
}
