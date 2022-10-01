/*
	File: Source/MitOS/ui/taskbar.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stddef.h>

#define TBITEM_USE		1

struct TBITEMCTL *tbitemctl;

void vram_init_taskbar(char *vram, int x, int y)
{
	varm_fill_rectangle(vram, x, COL8_AAAAAA,  0, y - 28, 60    , y - 28);
	varm_fill_rectangle(vram, x, COL8_FFFFFF,  0, y - 27, x -  1, y - 27);
	varm_fill_rectangle(vram, x, COL8_AAAAAA,  0, y - 26, 60    , y -  1);
	vram_init_system_tray(vram, x, y, 1);
	vram_init_start_button(vram, x, y, 0);
	vram_draw_icon(vram, x, x - 64, y - 20, 0);
	return;
}

void vram_init_start_button(char *vram, int x, int y, int sw)
{
	varm_fill_rectangle(vram, x, COL8_AAAAAA, 2, y - 24, 60, y - 3);
	if (sw == 1) {
		varm_fill_rectangle(vram, x, COL8_000000,  3, y - 24, 59, y - 24);
		varm_fill_rectangle(vram, x, COL8_000000,  2, y - 24,  2, y -  4);
		varm_fill_rectangle(vram, x, COL8_555555,  3, y - 23, 59, y - 23);
		varm_fill_rectangle(vram, x, COL8_555555,  3, y - 23,  3, y -  5);
		varm_fill_rectangle(vram, x, COL8_FFFFFF,  2, y -  3, 59, y -  3);
		varm_fill_rectangle(vram, x, COL8_FFFFFF, 60, y - 24, 60, y -  3);
		vram_draw_string(vram, x, 13, y - 19, COL8_000000, "MitOS");
		vram_draw_string(vram, x, 12, y - 20, COL8_555555, "MitOS");
	} else {
		varm_fill_rectangle(vram, x, COL8_FFFFFF,  3, y - 24, 59, y - 24);
		varm_fill_rectangle(vram, x, COL8_FFFFFF,  2, y - 24,  2, y -  4);
		varm_fill_rectangle(vram, x, COL8_555555,  3, y -  4, 59, y -  4);
		varm_fill_rectangle(vram, x, COL8_555555, 59, y - 23, 59, y -  5);
		varm_fill_rectangle(vram, x, COL8_000000,  2, y -  3, 59, y -  3);
		varm_fill_rectangle(vram, x, COL8_000000, 60, y - 24, 60, y -  3);
		vram_draw_string(vram, x, 12, y - 20, COL8_000000, "MitOS");
		vram_draw_string(vram, x, 11, y - 21, COL8_555555, "MitOS");
	}
	return;
}

void vram_init_system_tray(char *vram, int x, int y, int icon_num)
{
	varm_fill_rectangle(vram, x, COL8_FFFFFF, x - 46 - icon_num * 22, y - 24, x - 3, y - 3);
	varm_fill_rectangle(vram, x, COL8_555555, x - 46 - icon_num * 22, y - 24, x - 4, y - 4);
	varm_fill_rectangle(vram, x, COL8_AAAAAA, x - 45 - icon_num * 22, y - 23, x - 4, y - 4);
	return;
}

void tbitemctl_init(struct MEMMAN *memman, struct SHEET *sht_back, int scrnx, int scrny)
{
	int i;
	tbitemctl = (struct TBITEMCTL *) 
		memman_alloc_4k(memman, sizeof(struct TBITEMCTL));
	for (i = 0; i < MAX_TBITEM; i++) {
		tbitemctl->item[i].flag = 0;
		tbitemctl->item[i].index = -1;
		tbitemctl->item[i].icon = NULL;
		tbitemctl->item[i].sht = NULL;
	}
	tbitemctl->num = 0;
	tbitemctl->active = -1;
	tbitemctl->sht_back = sht_back;
	tbitemctl->scrnx = scrnx;
	tbitemctl->scrny = scrny;
	return;
}

void tbitemctl_refresh(void)
{
	int i;
	int x = 68, y = tbitemctl->scrny - 24;
	vram_init_taskbar(tbitemctl->sht_back->buf, tbitemctl->sht_back->bxsize, tbitemctl->sht_back->bysize);
	for (i = 0; i < tbitemctl->num; i++) {
		if (i != tbitemctl->active) {
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_000000, x,     y,     x + 21, y + 21);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_FFFFFF, x,     y,     x + 20, y + 20);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_555555, x + 1, y + 1, x + 20, y + 20);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_AAAAAA, x + 1, y + 1, x + 19, y + 19);
		} else {
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_FFFFFF, x,     y,     x + 21, y + 21);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_000000, x,     y,     x + 20, y + 20);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_555555, x + 1, y + 1, x + 20, y + 20);
			varm_fill_rectangle(tbitemctl->sht_back->buf, tbitemctl->scrnx, COL8_AAAAAA, x + 2, y + 2, x + 20, y + 20);
		}
		if (tbitemctl->item[i].icon != NULL) {
			vram_draw_block(tbitemctl->sht_back->buf, tbitemctl->scrnx, 
				16, 16, x + 3, y + 3, tbitemctl->item[i].icon, 16);
		}
		x += 24;
	}
	sheet_refresh(tbitemctl->sht_back, 68, y, x + 1, y + 24);
	sheet_refresh(tbitemctl->sht_back, 68, tbitemctl->scrny - 24, tbitemctl->scrnx - 60, tbitemctl->scrny - 2);
}

int tbitemctl_add(unsigned char *icon, struct SHEET *sht)
{
	if (tbitemctl->num >= MAX_TBITEM) {
		return -1;
	}
	tbitemctl->item[tbitemctl->num].flag = TBITEM_USE;
	tbitemctl->item[tbitemctl->num].icon = icon;
	tbitemctl->item[tbitemctl->num].sht = sht;
	tbitemctl->item[tbitemctl->num].index = tbitemctl->num;
	tbitemctl->active = tbitemctl->num;
	tbitemctl->num++;
	tbitemctl_refresh();
	return 0;
}

int tbitemctl_remove(struct SHEET *sht)
{
	int i, id = tbitemctl_getsheet(sht);
	if (id == -1) {
		return -1;
	}
	tbitemctl->num--;
	for (i = id; i < tbitemctl->num; i++) {
		tbitemctl->item[i] = tbitemctl->item[i + 1];
	}
	tbitemctl->item[tbitemctl->num].flag = 0;
	vram_init_srceen(tbitemctl->sht_back->buf, tbitemctl->scrnx, tbitemctl->scrny, NULL);
	tbitemctl_refresh();
	return 0;
}

void tbitemctl_switch(int id)
{
	if (tbitemctl->item[id].sht != 0) {
		if (id == tbitemctl->active) {
			return;
		}
		struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
		keywin_off(shtctl->sheets[shtctl->top - 1]);
		tbitemctl->active = id;
		tbitemctl_refresh();
		sheet_setheight(tbitemctl->item[id].sht, shtctl->top - 1);
		key_win = tbitemctl->item[id].sht;
		keywin_on(key_win);
	}
	return;
}

void tbitemctl_set(struct SHEET *sht)
{
	int id = tbitemctl_getsheet(sht);
	if (id != -1) {
		tbitemctl_switch(id);
	}
	return;
}

int tbitemctl_getsheet(struct SHEET *sht)
{
	int i;
	for (i = 0; i < tbitemctl->num; i++) {
		if (tbitemctl->item[i].sht == sht) {
			return i;
		}
	}
	return -1;
}
