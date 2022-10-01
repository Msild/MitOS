/*
	File: Source/main/main.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>
#include <string.h>

#define KEYCMD_LED		0xed

struct SHEET *key_win;

void close_console(struct SHEET *sht);
void close_constask(struct TASK *task);

void Main(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, j, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char *buf_back, buf_mouse[256], *buf_menu;
	struct SHEET *sht_back, *sht_mouse, *sht_menu;
	struct TASK *task_main, *task;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7',  '8', '9', '0',  '-', '=', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P', '[', ']',  0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,  '\\', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,    ' ', 0,   0,    0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8',  '9', '-', '4',  '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,    0,   0,   0,    0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,    0,   0,   0,    0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', '@', '#', '$', '%', '^', '&',  '*', '(', ')', '_', '+', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P', '{', '}', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',   0,   '|', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,    ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8',  '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,    0,   0,   0,   0,   '|', 0,   0
	};
	int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int key_shift = 0, key_ctrl = 0, key_alt = 0, key_mit = 0, key_flag = 0;
	int x, y, mmx = -1, mmy = -1, mmx2 = 0, sbtns = 0;
	struct SHEET *sht = 0, *sht2;
	int *fat;
	unsigned char *mit32;
	struct FILEINFO *finfo;

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC的初始化已完成，开放CPU的中断 */
	fifo_init(&fifo, 128, fifobuf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* 开放键盘中断 */
	io_out8(PIC1_IMR, 0xef); /* 开放鼠标中断 */
	fifo_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memory_test(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	palette_init();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_main = task_init(memman);
	fifo.task = task_main;
	task_run(task_main, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;

	/* 载入mit32.res */
	resszie = (int *) memman_alloc_4k(memman, sizeof(resszie));
	fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_read_fat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	finfo = file_search("mit32.res", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		i = finfo->size;
		mit32 = file_load_tek(finfo->clustno, &i, fat);
		*resszie = i;
	} else {
		mit32 = NULL;
		*resszie = 0;
	}
	*((int *) 0x0fe8) = (int) mit32;
	memman_free_4k(memman, (int) fat, 4 * 2880);

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_init(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	varm_fill_rectangle(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, binfo->scrnx - 1, binfo->scrny - 1);
	static char *logo[] = {
		"______  ____________   _______________",
		"___   |/  /__(_)_  /_  __  __ \\_  ___/",
		"__  /|_/ /__  /_  __/  _  / / /____ \\ ",
		"_  /  / / _  / / /_    / /_/ /____/ / ",
		"/_/  /_/  /_/  \\__/    \\____/ /____/  "
	};
	/*
		______  ____________   _______________
		___   |/  /__(_)_  /_  __  __ \_  ___/
		__  /|_/ /__  /_  __/  _  / / /____ \ 
		_  /  / / _  / / /_    / /_/ /____/ / 
		/_/  /_/  /_/  \__/    \____/ /____/  
	*/
	for (i = (binfo->scrny - 5 * 16) / 2 - 64, j = 0; j < 5; i += 16, j++) {
		vram_draw_string(buf_back, binfo->scrnx, (binfo->scrnx - 38 * 8) / 2, i, COL8_AAAAAA, logo[j]);
		vram_draw_string(buf_back, binfo->scrnx, (binfo->scrnx - 38 * 8) / 2 + 1, i, COL8_555555, logo[j]);
	}
	varm_draw_mstring(buf_back, binfo->scrnx, (binfo->scrnx - 15 * 6) / 2 + 1, 
					   (binfo->scrny - 5 * 16) / 2 + 48, 8, "Version 1.0.0.0");
	varm_draw_mstring(buf_back, binfo->scrnx, (binfo->scrnx - 46 * 6) / 2, 
					   binfo->scrny - 32, 8, 
					   "Copyright (c) MLworkshop, All Rights Reserved.");
	sheet_refresh(sht_back, 0, 0, binfo->scrnx - 1, binfo->scrny - 1);
	sheet_setheight(sht_back, 0);
	console_beep(880000, 168);
	console_beep(1318510, 168);
	varm_fill_rectangle(buf_back, binfo->scrnx, COL8_00AAAA, 0, 0, binfo->scrnx - 1, binfo->scrny - 1);
	vram_init_srceen(buf_back, binfo->scrnx, binfo->scrny, EF_WALLPAPER);
	/* 初始化任务栏 */
	tbitemctl_init(memman, sht_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_init(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* 计算坐标使显示居中 */
	my = (binfo->scrny - 28 - 16) / 2;

	/* sht_menu */
	sht_menu = sheet_alloc(shtctl);
	buf_menu = (unsigned char *) memman_alloc_4k(memman, 128 * 200);
	sheet_init(sht_menu, buf_menu, 128, 200, -1);
	varm_fill_rectangle(buf_menu, 128, COL8_000000, 0,   0, 127, 199);
	varm_fill_rectangle(buf_menu, 128, COL8_FFFFFF, 0,   0, 126, 198);
	varm_fill_rectangle(buf_menu, 128, COL8_555555, 1,   1, 126, 198);
	varm_fill_rectangle(buf_menu, 128, COL8_AAAAAA, 1,   1, 125, 197);
	varm_fill_rectangle(buf_menu, 128, COL8_555555, 3, 166, 123, 166);
	vram_draw_menuitem(buf_menu, 128, 16,  12, 1, "Console");
	vram_draw_menuitem(buf_menu, 128, 16, 174, 9, "Shut Down");
	sht_menu->flags = SHT_FLAG_MENU;

	sheet_move(sht_back,   0,  0                );
	sheet_move(key_win,   16, 16                );
	sheet_move(sht_mouse, mx, my                );
	sheet_move(sht_menu,   2, binfo->scrny - 224);
	sheet_setheight(sht_back,  0);
	sheet_setheight(key_win,   1);
	sheet_setheight(sht_mouse, 2);
	sheet_setheight(sht_menu, -1);
	keywin_on(key_win);

	/* 设置LED状态以免发生冲突 */
	fifo_push(&keycmd, KEYCMD_LED);
	fifo_push(&keycmd, key_leds);

	sheet_refresh(sht_back, 0, 0, binfo->scrnx - 1, binfo->scrny - 1);

	for (;;) {
		if (memman_getfree(memman) < 1024 * 1024) {
			black_srceen(0, 0x20, "Out Of Memory.");
		}
		if (fifo_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 有键盘数据则发送 */
			keycmd_wait = fifo_pop(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo_status(&fifo) == 0) {
			/* FIFO为空则保留 */
			if (new_mx >= 0) {
				io_sti();
				sheet_move(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_move(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_main);
				io_sti();
			}
		} else {
			i = fifo_pop(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) {
				/* 窗口已关闭 */
				if (shtctl->top == 1) {
					/* 只有鼠标和背景 */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
					tbitemctl_set(key_win);
				}
			}
			if (256 <= i && i <= 511) {
				/* 键盘数据 */
				if (i == 256 + 0xe0) {
					/* 扩展键前缀 */
					key_flag = 1;
				}
				if (i < 0x80 + 256) { /* 将按键编码转为字符数据 */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					} else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') {
					/* 输入字母 */
					if (((key_leds & 4) == 0 && key_shift == 0) || 
						((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* 将大写字母转为小写字母 */
					}
				}
				if ('0' <= s[0] && s[0] <= '9' && i >= 256 + 0x47) {
					/* 输入小键盘数字 */
					if (!(key_leds & 2)) {
						/* Numlock关闭 */
						s[0] = 0;
					}
				}
				if (s[0] != 0 && key_win != 0 && shtctl->top > 1) {
					/* 一般字符、回车键、退格键 */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						if (key_ctrl == 0 && key_alt == 0) {
							fifo_push(&key_win->task->fifo, s[0] + 256);
						} else if (key_ctrl != 0 && key_alt == 0) {
							fifo_push(&key_win->task->fifo, s[0] + 1024);
						} else if (key_ctrl == 0 && key_alt != 0) {
							fifo_push(&key_win->task->fifo, s[0] + 1280);
						} else {
							fifo_push(&key_win->task->fifo, s[0] + 1536);
						}
					}
				}
				if (i == 256 + 0x01 && key_win != 0 && shtctl->top > 1) {
					/* ESC */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 5);
					}
				}
				if (i == 256 + 0x3b) {
					/* F1 */
					int oldx = 16, oldy = 16;
					if (key_win != 0) {
						keywin_off(key_win);
						oldx = key_win->vx0 + 16;
						oldy = key_win->vy0 + 16;
					}
					if (oldx >= binfo->scrnx - 256) {
						oldx = 16;
					}
					if (oldy >= binfo->scrny - 165 - 28) {
						oldy = 16;
					}
					key_win = open_console(shtctl, memtotal);
					sheet_move(key_win, oldx, oldy);
					sheet_setheight(key_win, shtctl->top);
					/* 自动将输入焦点切换到新的控制台窗口 */
					keywin_on(key_win);
				}
				if (i == 256 + 0x3e && key_win != 0) {
					/* F4 */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						/* cons_putstr0(task->cons, "\nBreak : Key \n"); */
						io_cli();	/* 强制结束处理时禁止切换任务 */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);	/* 唤醒正在休眠的任务 */
						key_win->mx = key_win->my = 0;
						key_win->mbtn = 0x00;
					}
				}
				if (i == 256 + 0x0f && key_win != 0) {
					/* Tab */
					/* 暂时不做处理 */
				}
				if (i == 256 + 0x34 && key_ctrl != 0 && key_alt != 0) {
					/* Ctrl+Alt+Delete*/
					/* 暂时不做处理 */
				}
				if (i == 256 + 0x2a) {
					/* 左Shift ON */
					key_shift |= 1;
				}
				if (i == 256 + 0xaa) {
					/* 左Shift OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0x36) {
					/* 右Shift ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xb6) {
					/* 右Shift OFF */
					key_shift &= ~2;
				}
				if (i == 256 + 0x1d) {
					/* 左Ctrl ON */
					key_ctrl |= 1;
				}
				if (i == 256 + 0x9d) {
					/* 左Ctrl OFF */
					key_ctrl &= ~1;
				}
				if (i == 256 + 0x1d && key_flag) {
					/* 右Ctrl ON */
					key_ctrl |= 2;
					key_flag = 0;
				}
				if (i == 256 + 0x9d && key_flag) {
					/* 右Ctrl OFF */
					key_ctrl &= ~2;
					key_flag = 0;
				}
				if (i == 256 + 0x38) {
					/* 左Alt ON */
					key_alt |= 1;
				}
				if (i == 256 + 0xb8) {
					/* 左Alt OFF */
					key_alt &= ~1;
				}
				if (i == 256 + 0x38 && key_flag) {
					/* 右Alt ON */
					key_alt |= 2;
					key_flag = 0;
				}
				if (i == 256 + 0xb8 && key_flag) {
					/* 右Alt OFF */
					key_alt &= ~2;
					key_flag = 0;
				}
				if (i == 256 + 0x5b && key_flag) {
					/* 左MitKey ON */
					key_mit |= 1;
					key_flag = 0;
					sbtns = !sbtns;
				}
				if (i == 256 + 0xdb && key_flag) {
					/* 左MitKey OFF */
					key_mit &= ~1;
					key_flag = 0;
					sbtns = !sbtns;
				}
				if (i == 256 + 0x5c && key_flag) {
					/* 右MitKey ON */
					key_mit |= 2;
					key_flag = 0;
					sbtns = !sbtns;
				}
				if (i == 256 + 0xdc && key_flag) {
					/* 右MitKey OFF */
					key_mit &= ~2;
					key_flag = 0;
					sbtns = !sbtns;
				}
				if (i == 256 + 0x5d && key_flag) {
					/* 控制键 */
					/* 暂时不做处理 */
				}
				if (i == 256 + 0x48 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Up */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 6);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x4b && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Left */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 7);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x50 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Down */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 8);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x4d && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Right */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 9);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x52 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Insert */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 10);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x47 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Home */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 11);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x49 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* PgUp */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 12);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x53 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* Delete */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 13);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x4f && key_flag && key_win != 0 && shtctl->top > 1) {
					/* End */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 14);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x51 && key_flag && key_win != 0 && shtctl->top > 1) {
					/* PgDn */
					if (!(key_win->flags >= SHT_FLAG_MENU)) {
						fifo_push(&key_win->task->fifo, 15);
					}
					key_flag = 0;
				}
				if (i == 256 + 0x1c && key_flag) {
					/* KP Enter */
					/* 暂时不做处理 */
					key_flag = 0;
				}
				if (i == 256 + 0x35 && key_flag) {
					/* KP / */
					/* 暂时不做处理 */
					key_flag = 0;
				}
				if (i == 256 + 0x3a) {
					/* CapsLock */
					key_leds ^= 4;
					fifo_push(&keycmd, KEYCMD_LED);
					fifo_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {
					/* NumLock */
					key_leds ^= 2;
					fifo_push(&keycmd, KEYCMD_LED);
					fifo_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {
					/* ScrollLock */
					key_leds ^= 1;
					fifo_push(&keycmd, KEYCMD_LED);
					fifo_push(&keycmd, key_leds);
				}
				if (i == 256 + 0xfa) {
					/* 键盘成功接收到数据 */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {
					/* 键盘未能接收到数据 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
				if (key_mit != 0) {
					vram_init_start_button(buf_back, binfo->scrnx, binfo->scrny, 1);
					sheet_refresh(sht_back, 2, binfo->scrny - 24, 
								  61, binfo->scrny - 2);
				} else {
					vram_init_start_button(buf_back, binfo->scrnx, binfo->scrny, 0);
					sheet_refresh(sht_back, 2, binfo->scrny - 24, 
								  61, binfo->scrny - 2);
				}
			} else if (512 <= i && i <= 767) {
				/* 鼠标数据 */
				if (decode_mouse(&mdec, i - 512) != 0) {
					/* 移动鼠标光标 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					new_mx = mx;
					new_my = my;
					if ((mdec.btn & 0x01) != 0) {
						/* 按下鼠标左键 */
						if (mmx < 0) {
							/* 通常模式 */
							/* 从顶部图层寻找鼠标 */
							for (j = shtctl->top - 1; j >= 0; j--) {
								sht = shtctl->sheets[j];
								tbitemctl_set(sht);
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (2 <= x && x <= 61 && sht->bysize - 24 <= y 
									&& y <= sht->bysize - 2 && j == 0) {
									/* 点击「开始」按钮 */
									sbtns = !sbtns;
								}
								if (!(2 <= mx && mx <= 129 && binfo->scrny - 224 <= my && my <= binfo->scrny - 25) && j != 0) {
									sbtns = 0;
								}
								if (sht->bxsize - 48 <= x && x <= sht->bxsize - 3 
									&& sht->bysize - 24 <= y && y <= sht->bysize - 2 
									&& j == 0) {
									/* 点击「时间」按钮 */
									/* 暂时不做处理 */
								}
								if (68 <= x && x <= tbitemctl->num * 24 - 4 + 69
									&& sht->bysize - 24 <= y && y <= sht->bysize - 3) {
										if ((x - 68) % 24 < 24) {
											/* 点击任务栏图标 */
											int id = (x - 68) / 24;
											tbitemctl_switch(id);
										}
								}
								if (0 <= x && x < sht->bxsize && 0 <= y 
									&& y < sht->bysize && j != 0) {
									if (sht->buf[y * sht->bxsize + x] != 
										sht->col_inv) {
										if (key_win != 0 && shtctl->top > 1 && 
											key_win->flags < SHT_FLAG_MENU) {
											fifo_push(&key_win->task->fifo, 16);
										}
										sht->mbtn = 0x01;
										sht->mx = x;
										sht->my = y;
										sheet_setheight(sht, shtctl->top - 1);
										if (sht != key_win && sht->flags < SHT_FLAG_MENU) {
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21 
											&& sht->flags < SHT_FLAG_MENU) {
											mmx = mx;	/* 进入窗口移动模式 */
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (sht->bxsize - 21 <= x 
											&& x < sht->bxsize - 5 
											&& 5 <= y && y < 19) {
											/* 点击「x」按钮 */
											if (sht->flags == SHT_FLAG_APPWIN) {
												/*应用程序窗口 */
												tbitemctl_remove(sht);
												task = sht->task;
												/* cons_putstr0(task->cons, "\nBreak : Mouse\n"); */
												/* 强制结束处理时禁止切换任务 */
												io_cli();
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											} else if (sht->flags == SHT_FLAG_CONSOLE) {
												/* 控制台窗口 */
												tbitemctl_remove(sht);
												task = sht->task;
												/* 暂时隐藏 */
												sheet_setheight(sht, -1);
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												fifo_push(&task->fifo, 4);
												io_sti();
											}
											sht->mx = sht->my = 0;
											sht->mbtn = 0x00;
										}
										break;
									}
								}
							}
						} else {
							/* 处于窗口移动模式 */
							x = mx - mmx;	/* 计算移动量 */
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							mmy = my;	/* 更新移动后坐标 */
						}
					} else if ((mdec.btn & 0x02) != 0) {
						/* 按下鼠标右键 */
						if (mmx < 0) {
							/* 通常模式 */
							/* 从顶部图层寻找鼠标 */
							for (j = shtctl->top - 1; j >= 0; j--) {
								sht = shtctl->sheets[j];
								tbitemctl_set(sht);
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y 
									&& y < sht->bysize && j != 0) {
									if (sht->buf[y * sht->bxsize + x] != 
										sht->col_inv) {
										if (key_win != 0 && shtctl->top > 1 && 
											key_win->flags < SHT_FLAG_MENU) {
											fifo_push(&key_win->task->fifo, 17);
										}
										sht->mbtn = 0x02;
										sht->mx = x;
										sht->my = y;
										sheet_setheight(sht, shtctl->top - 1);
										if (sht != key_win && sht->flags < SHT_FLAG_MENU) {
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										break;
									}
								}
							}
						}
					} else if ((mdec.btn & 0x04) != 0) {
						/* 按下鼠标中键 */
						if (mmx < 0) {
							/* 通常模式 */
							/* 从顶部图层寻找鼠标 */
							for (j = shtctl->top - 1; j >= 0; j--) {
								sht = shtctl->sheets[j];
								tbitemctl_set(sht);
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y 
									&& y < sht->bysize && j != 0) {
									if (sht->buf[y * sht->bxsize + x] != 
										sht->col_inv) {
										if (key_win != 0 && shtctl->top > 1 && 
											key_win->flags < SHT_FLAG_MENU) {
											fifo_push(&key_win->task->fifo, 18);
										}
										sht->mbtn = 0x04;
										sht->mx = x;
										sht->my = y;
										sheet_setheight(sht, shtctl->top - 1);
										if (sht != key_win && sht->flags < SHT_FLAG_MENU) {
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										break;
									}
								}
							}
						}
					} else {
						/* 松开鼠标 */
						mmx = -1;	/* 进入通常模式 */
						if (new_wx != 0x7fffffff) {
							sheet_move(sht, new_wx, new_wy);
							new_wx = 0x7fffffff;
						}
						vram_init_start_button(buf_back, binfo->scrnx, binfo->scrny, sbtns);
						sheet_refresh(sht_back, 2, binfo->scrny - 24, 61, binfo->scrny - 2);
						if (sbtns == 1) {
							keywin_off(key_win);
							sheet_setheight(sht_menu, shtctl->top - 1);
							key_win = sht_menu;
							keywin_on(key_win);
						} else {
							keywin_off(sht_menu);
							sheet_setheight(sht_menu, -1);
							key_win = shtctl->sheets[shtctl->top - 1];
							keywin_on(key_win);
						}
					}
				}
			} else if (768 <= i && i <= 1023) {
				/* 控制台关闭处理 */
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			} else if (2024 <= i && i <= 2279) {
				/* 只关闭控制台窗口 */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				tbitemctl_remove(sht);
				sheet_free(sht2);
			}
			sprintf(s, "%02d:%02d", get_hour(), get_minute());
			sheet_draw_mstring(sht_back, 
								   binfo->scrnx - 40, binfo->scrny - 21, 
								   COL8_000000, COL8_AAAAAA, s, 5);
			sprintf(s, "%02d/%02d", get_month(), get_day_of_month());
			sheet_draw_mstring(sht_back, 
								   binfo->scrnx - 40, binfo->scrny - 11, 
								   COL8_000000, COL8_AAAAAA, s, 5);
		}
	}
}

void keywin_off(struct SHEET *key_win)
{
	if (!(key_win->flags >= SHT_FLAG_MENU)) {
		vram_change_wintitle(key_win, 0);
	}
	if (key_win->flags == SHT_FLAG_CONSOLE) {
		fifo_push(&key_win->task->fifo, 3); /* 隐藏控制台光标 */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	if (!(key_win->flags >= SHT_FLAG_MENU)) {
		vram_change_wintitle(key_win, 1);
	}
	if (key_win->flags == SHT_FLAG_CONSOLE) {
		fifo_push(&key_win->task->fifo, 2); /* 显示控制台光标 */
	}
	return;
}

struct TASK *open_console_task(struct SHEET *sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	sheet_init(sht, buf, 256, 165, -1); /* 无透明色 */
	vrma_make_window(buf, 256, 165, "Console", 0);
	vram_draw_icon(buf, 256, 4, 4, 1);
	vram_make_textbox(sht, 8, 28, 240, 128, COL8_000000);
	sht->task = open_console_task(sht, memtotal);
	sht->flags = SHT_FLAG_CONSOLE;	/* 显示光标 */
	int info[2], size;
	unsigned char *icon = NULL;
	if (res_info(info, *resszie, (char *) *((int *) 0x0fe8))) {
		icon = res_decode(info, *resszie, (char *) *((int *) 0x0fe8), "ICO00001", &size);
	}
	tbitemctl_add(icon, sht);
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0; /* task_free(task); 的代替 */
	return;
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	tbitemctl_remove(sht);
	sheet_free(sht);
	close_constask(task);
	return;
}
