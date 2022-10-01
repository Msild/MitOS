/*
	File: Source/console/api.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>

int *mitapi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_getnow();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO *sys_fifo = (struct FIFO *) *((int *) 0x0fec);
	int *reg = &eax + 1;	/* eax后面的地址 */
		/* 强行改写通过PUSHAD保存的值 */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
	int i;
	struct FILEINFO *finfo;
	struct HFILE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	if (edx == 1) {
		console_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		console_puts(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		console_putsl(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		task->status = eax;
		return &(task->tss.esp0);
	} else if (edx == 5) {
		keywin_off(shtctl->sheets[shtctl->top - 1]);
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags = SHT_FLAG_APPWIN;
		sht->mbtn = sht->mx = sht->my = 0;
		sheet_init(sht, (char *) ebx + ds_base, esi, edi, eax);
		vrma_make_window((char *) ebx + ds_base, esi, edi, (char *) ecx + ds_base, 0);
		sheet_move(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_setheight(sht, shtctl->top); /* 窗口置于鼠标光标下层 */
		key_win = sht;
		keywin_on(key_win);
		tbitemctl_add(NULL, sht);
		reg[7] = (int) sht;
	} else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		vram_draw_string(sht->buf, sht->bxsize, esi, edi, eax, 
					  (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		varm_fill_rectangle(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* 以16字节为单位 */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 以16字节为单位向上舍入 */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 以16字节为单位向上舍入 */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		vram_draw_line(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		sht = (struct SHEET *) ebx;
		tbitemctl_remove(sht);
		sheet_free(sht);
	} else if (edx == 15) {
		for (;;) {
			io_cli();
			if (fifo_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);	/* FIFO为空，休眠并等待 */
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo_pop(&task->fifo);
			io_sti();
			if (i <= 1 && cons->sht != 0) { /* 光标用定时器 */
				/* 应用程序运行时无需显示光标 */
				timer_init(cons->timer, &task->fifo, 1); /* 下次置1 */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* 光标ON */
				cons->cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* 光标OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {
				timer_cancel(cons->timer);
				io_cli();
				fifo_push(sys_fifo, cons->sht - shtctl->sheets0 + 2024);
				cons->sht = 0;
				io_sti();
			}
			if (5 <= i && i <= 18) { /* 特殊键值、鼠标数据 */
				reg[7] = i + 1792;
				return 0;
			}
			if (i >= 256) { /* 键盘数据 */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* 允许自动取消 */
	} else if (edx == 17) {
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base, (struct FILEINFO *) 
								(ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_load_tek(finfo->clustno, &fh->size, task->fat);
			}
		}
	} else if (edx == 22) {
		fh = (struct HFILE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		fh = (struct HFILE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		fh = (struct HFILE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		fh = (struct HFILE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		reg[7] = get_hour() * 10000 + 
				 get_minute() * 100 + 
				 get_second();
	} else if (edx == 28) {
		reg[7] = get_year() * 100000 + 
				 get_month() * 1000 + 
				 get_day_of_month() * 10 + 
				 get_day_of_week();
	} else if (edx == 29) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		vram_make_textbox(sht, eax, ecx, esi - eax, edi - ecx, ebp);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax - 3, ecx - 3, esi + 4, edi + 4);
		}
	} else if (edx == 30) {
		struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		switch (eax) {
			case 0:
				/* 系统版本 */ 
				reg[7] = OS_VERSION;
				break;
			case 1:
				/* 总共内存 */
				reg[7] = memory_test(0x00400000, 0xbfffffff);
				break;
			case 2:
				/* 可用内存 */
				reg[7] = memman_getfree(memman);
				break;
			case 3:
				/* X 分辨率 */
				reg[7] = binfo->scrnx;
				break;
			case 4:
				/* Y 分辨率 */
				reg[7] = binfo->scrny;
				break;
			default:
				reg[7] = 0; 
		}
	} else if (edx == 31) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		varm_draw_mstring(sht->buf, sht->bxsize, esi, edi, eax, 
						   (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 8);
		}
	} else if (edx == 32) {
		struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
		int *fat;
		struct ARG arg;
		fat = (int *) memman_alloc_4k(memman, 4 * 2880);
		file_read_fat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
		cmdline_split((char *) ebx + ds_base, &arg);
		console_run(arg, (char *) ebx + ds_base, cons, fat, 
					memory_test(0x00400000, 0xbfffffff));
	} else if (edx == 33) {
		finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
		for (i = 0; i < 224 && finfo[i].name[0] != 0; i++) { }
		reg[7] = i;
	} else if (edx == 34) {
		finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
		char *s = (char *) ebx + ds_base;
		sprintf(s, "filename.ext");
		for (i = 0; i < 8; i++) {
			s[i] = finfo[ecx].name[i];
		}
		s[ 8] = finfo[ecx].ext[0];
		s[ 9] = finfo[ecx].ext[1];
		s[10] = finfo[ecx].ext[2];
		s[11] = finfo[ecx].type;
		for (i = 0; i < 10; i++) s[i + 12] = finfo[ecx].reserve[i];
		reg[7] = 0;
	} else if (edx == 35) {
		finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
		switch (eax) {
			case 0:
				/* 文件时间 */
				reg[7] = (int) finfo[ebx].time;
				break;
			case 1:
				/* 文件日期 */
				reg[7] = (int) finfo[ebx].date;
				break;
			case 2:
				/* 文件位置 */
				reg[7] = (int) finfo[ebx].clustno;
				break;
			case 3:
				/* 文件大小 */
				reg[7] = finfo[ebx].size;
				break;
			default:
				reg[7] = 0; 
		}
	} else if (edx == 36) {
		reg[7] = kmktime(get_year(), get_month(), get_day_of_month(),
						get_hour(), get_minute(), get_second());
	} else if (edx == 37) {
		sht = (struct SHEET *) eax;
		sheet_move(sht, ebx, ecx);
	} else if (edx == 38) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		vram_draw_icon(sht->buf, sht->bxsize, esi, edi, eax);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 16, edi + 16);
		}
	} else if (edx == 39) {
		sht = (struct SHEET *) eax;
		switch (ecx) {
			case 0:
				reg[7] = sht->mbtn;
				sht->mbtn = -1;
				break;
			case 1:
				reg[7] = sht->mx;
				sht->mx = -1;
				break;
			case 2:
				reg[7] = sht->my;
				sht->my = -1;
				break;
			default:
				reg[7] = 0;
		}
	} else if (edx == 40) {
		sht = (struct SHEET *) eax;
		sheet_setheight(sht, -1);
	} else if (edx == 41) {
		sht = (struct SHEET *) eax;
		sheet_setheight(sht, shtctl->top - 1);
	} else if (edx == 42) {
		for (i = 0; i < MAX_TASKS && taskctl->tasks0[i].flags != 0; i++) { }
		reg[7] = i;
	} else if (edx == 43) {
		struct TASK *task = &taskctl->tasks0[ebx];
		switch (eax) {
			case 0:
				/* 任务编号 */
				reg[7] = task->sel;
				break;
				
			case 1:
				/* 任务地址 */
				reg[7] = (int) task;
				break;
			case 2:
				/* 任务Level */
				reg[7] = task->level;
				break;
			case 3:
				/* 任务Priority */
				reg[7] = task->priority;
				break;
			default:
				reg[7] = 0; 
		}
	} else if (edx == 44) {
		kernel_shutdown();
	} else if (edx == 45) {
		kernel_reboot();
	} else if (edx == 46) {
		unsigned char *icon = NULL;
		i = tbitemctl_getsheet((struct SHEET *) ecx);
		if (i != -1) {
			if (ebx != 0) {
				icon = (unsigned char *) ebx + ds_base;
			} else {
				int info[2], size;
				if (res_info(info, *resszie, (char *) *((int *) 0x0fe8))) {
					icon = res_decode(info, *resszie, (char *) *((int *) 0x0fe8), "ICO00006", &size);
				}
			}
			tbitemctl->item[i].icon = icon;
		}
	}
	return 0;
}
