/*
	File: Source/MitOS/ui/graphic.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stddef.h>
#include <stdio.h>

unsigned char rgb2pal(int r, int g, int b, int x, int y);

void palette_init(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/* 0.黑色 */
		0x00, 0x00, 0xaa,	/* 1.深蓝 */
		0x00, 0xaa, 0x00,	/* 2.深绿 */
		0x00, 0xaa, 0xaa,	/* 3.湖蓝 */
		0xaa, 0x00, 0x00,	/* 4.深红 */
		0xaa, 0x00, 0xaa,	/* 5.紫色 */
		0xaa, 0x55, 0x00,	/* 6.棕色 */
		0xaa, 0xaa, 0xaa,	/* 7.灰色 */
		0x55, 0x55, 0x55,	/* 8.深灰 */
		0x55, 0x55, 0xff,	/* 9.蓝色 */
		0x55, 0xff, 0x55,	/* A.绿色 */
		0x55, 0xff, 0xff,	/* B.天蓝 */
		0xff, 0x55, 0x55,	/* C.红色 */
		0xff, 0x55, 0xff,	/* D.粉红 */
		0xff, 0xff, 0x55,	/* E.黄色 */
		0xff, 0xff, 0xff	/* F.白色 */
	};
	palette_set(0, 15, table_rgb);

	unsigned char table2[216 * 3];
	int r, g, b;
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	palette_set(16, 231, table2);
	return;
}

void palette_set(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志 */
	io_cli(); 					/* 禁止中断 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}

void varm_fill_rectangle(unsigned char *vram, int xsize, 
	unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void vram_draw_line(unsigned char *vram, int xsize, unsigned char c,
	int x0, int y0, int x1, int y1)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}
	for (i = 0; i < len; i++) {
		vram[(y >> 10) * xsize + (x >> 10)] = c;
		x += dx;
		y += dy;
	}
	return;
}

void vram_init_srceen(char *vram, int x, int y, char *bg_name)
{
	if (bg_name != NULL) {
		int *fat;
		struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		fat = (int *) memman_alloc_4k(memman, 4 * 2880);
		file_read_fat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
		load_picture(bg_name, fat, vram, x, x, y, 0);
		memman_free_4k(memman, (int) fat, 4 * 2880);
	}

	varm_fill_rectangle(vram, x, COL8_AAAAAA,  0,     y - 28, x -  1, y - 28);
	varm_fill_rectangle(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	varm_fill_rectangle(vram, x, COL8_AAAAAA,  0,     y - 26, x -  1, y -  1);

	vram_init_taskbar(vram, x, y);

	vram_draw_string(vram, x, 12, y - 20, COL8_000000, "MitOS");
	vram_draw_string(vram, x, 11, y - 21, COL8_555555, "MitOS");
	return;
}

void varm_draw_font(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void vram_draw_string(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	extern char ascii[4096];
	for (; *s != 0x00; s++) {
		varm_draw_font(vram, xsize, x, y, c, ascii + *s * 16);
		x += 8;
	}
	return;
}

void vram_draw_mfont(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 8; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void varm_draw_mstring(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	extern char mchar[2048];
	for (; *s != 0x00; s++) {
		vram_draw_mfont(vram, xsize, x, y, c, mchar + *s * 8);
		x += 6;
	}
	return;
}

void init_mouse_cursor(char *mouse, char bc)
{
	/* 准备鼠标指针（16*16） */
	static char cursor[16][16] = {
		"*...............",
		"**..............",
		"*O*.............",
		"*OO*............",
		"*OOO*...........",
		"*OOOO*..........",
		"*OOOOO*.........",
		"*OOOOOO*........",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOOOOOOO*.....",
		"*OOOO******.....",
		"*OOO*...........",
		"*OO*............",
		"*O*.............",
		"**.............."
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void vram_draw_block(char *vram, int vxsize, int pxsize, int pysize, 
				 int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

int load_picture(char *filename, int *fat, char *vram, int bx, int x, int y, int mode)
{
	int i, j, x0 = 0, y0 = 0, fsize, info[8];
	unsigned char *filebuf, r, g, b;
	struct RGB *picbuf;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct DLL_STRPICENV *env;
	finfo = file_search(filename, 
						(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0) {
		return 1;
	}
	fsize = finfo->size;
	filebuf = (unsigned char *) memman_alloc_4k(memman, fsize);
	filebuf = file_load_tek(finfo->clustno, &fsize, fat);
	env = (struct DLL_STRPICENV *) 
		memman_alloc_4k(memman, sizeof(struct DLL_STRPICENV));
	if (info_JPEG(env, info, fsize, filebuf) == 0) {
		return 2;
	}
	picbuf = (struct RGB *) 
		memman_alloc_4k(memman, info[2] * info[3] * sizeof(struct RGB));
	i = decode0_JPEG(env, fsize, filebuf, 4, (unsigned char *) picbuf, 0);
	if (i != 0) {
		return 2;
	}
	if (mode == 0) {
		x0 = (int) ((x - info[2]) / 2);
		y0 = (int) ((y - info[3]) / 2);
	} else if (mode == 1) {
		x0 = x;
		y0 = y;
	}

	for (i = 0; i < info[3]; i++) {
		for (j = 0; j < info[2]; j++) {
			r = picbuf[i * info[2] + j].r;
			g = picbuf[i * info[2] + j].g;
			b = picbuf[i * info[2] + j].b;
			vram[(y0 + i) * bx + (x0 + j)] = rgb2pal(r, g, b, j, i);
		}
	}
	memman_free_4k(memman, (int) filebuf, fsize);
	memman_free_4k(memman, (int) picbuf , info[2] * info[3] * sizeof(struct RGB));
	memman_free_4k(memman, (int) env , sizeof(struct DLL_STRPICENV));
	return 0;
}

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	int i;
	x &= 1; /* 判断奇偶 */
	y &= 1;
	i = table[x + y * 2];	/* 生成中间色 */
	r = (r * 21) / 256;	/* r = 0~20 */
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;	/* r = 0~5 */
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}

void vram_draw_icon(char *vram, int bxsize, int x, int y, int id)
{
	if (*resszie != 0) {
		int info[2];
		if (res_info(info, *resszie, (char *) *((int *) 0x0fe8)) == 0) {
			return;
		}
		char s[16], *icon = NULL;
		int ix, iy, size;
		sprintf(s, "ICO%05X", id);
		icon = res_decode(info, *resszie, (char *) *((int *) 0x0fe8), s, &size);
		char ixy;
		for (ix = 0; ix < 16; ix++) {
			for (iy = 0; iy < 16; iy++) {
				ixy = icon[ix + 16 * iy];
				if (ixy >= 0x20) ixy = -1;
				if (ixy != -1) vram[bxsize * (y + iy) + (x + ix)] = ixy;
			}
		}
	}
	return;
}

void vram_draw_menuitem(unsigned char *buf, int bxsize, int x, int y, int id, char *ds)
{
	vram_draw_icon(buf, bxsize, x, y, id);
	varm_draw_mstring(buf, bxsize, x + 20, y + 4, COL8_000000, ds);
	return;
}
