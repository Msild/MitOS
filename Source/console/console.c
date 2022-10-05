/*
	File: Source/console/console.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void console_task(struct SHEET *sheet, int memtotal)
{
	struct TASK *task = task_getnow();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct HFILE fhandle[8];
	struct ARG arg;
	char cmdline[40], cmdline2[40];

	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	cons.bc = 0;
	cons.fc = 7;
	cons.echo = 1;
	cons.cmd_index = 0;
	cons.cmd_num = 0;
	task->cons = &cons;
	task->cmdline = cmdline;
	strcpy(cons.prompt, "FD>");
	cons.pmtlen = strlen(cons.prompt);

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_read_fat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* 未使用标志 */
	}
	task->fhandle = fhandle;
	task->fat = fat;

	/* 显示信息 */
	console_puts(&cons, "MLworkshop MitOS  Version 1.0.0.0\n\n");

	/* 显示提示符 */
	if (cons.echo) {
		console_puts(&cons, cons.prompt);
	}

	for (;;) {
		io_cli();
		if (fifo_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo_pop(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) { /* 光标用定时器 */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* 下次置0 */
					if (cons.cur_c >= 0) {
						cons.cur_c = cons.fc;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* 下次置1 */
					if (cons.cur_c >= 0) {
						cons.cur_c = cons.bc;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) { /* 显示光标 */
				cons.cur_c = cons.fc;
			}
			if (i == 3) { /* 隐藏光标 */
				if (cons.sht != 0) {
					varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.bc, 
						cons.cur_x, cons.cur_y, 
						cons.cur_x + 5, cons.cur_y + 7);
				}
				cons.cur_c = -1;
			}
			if (i == 4) { /* 点击控制台「x」按钮 */
				command_exit(&cons, fat);
			}
			if (i == 5) { /* ESC */
				cons.cur_x = 8 + cons.pmtlen * 6;
				varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.bc, 
					8 + cons.pmtlen * 6, cons.cur_y, 249, cons.cur_y + 7);
				sheet_refresh(cons.sht, 8 + cons.pmtlen * 6, 
					cons.cur_y, 250, cons.cur_y + 16);
			}
			if (i == 6) { /* Up */
				if (cons.cmd_index > 0) {
					cons.cur_x = 8 + cons.pmtlen * 6;
					varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.bc, 
						8 + cons.pmtlen * 6, cons.cur_y, 
						249, cons.cur_y + 7);
					sheet_refresh(cons.sht, 8 + cons.pmtlen * 6, 
						cons.cur_y, 250, cons.cur_y + 16);
					strcpy(cmdline, cons.cmd_buf[--cons.cmd_index]);
					console_puts(&cons, cmdline);
					cons.cur_x = 8 + cons.pmtlen * 6 + strlen(cmdline) * 6;
				}
			}
			if (i == 7) { /* Left */
				/* 暂时不做处理 */
			}
			if (i == 8) { /* Down */
				if (cons.cmd_index < cons.cmd_num - 1) {
					cons.cur_x = 8 + cons.pmtlen * 6;
					varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.bc, 
						8 + cons.pmtlen * 6, cons.cur_y, 
						249, cons.cur_y + 7);
					sheet_refresh(cons.sht, 8 + cons.pmtlen * 6, 
						cons.cur_y, 250, cons.cur_y + 16);
					strcpy(cmdline, cons.cmd_buf[++cons.cmd_index]);
					console_puts(&cons, cmdline);
					cons.cur_x = 8 + cons.pmtlen * 6 + strlen(cmdline) * 6;
				}
			}
			if (i == 9) { /* Right */
				/* 暂时不做处理 */
			}
			if (i == 10) { /* Insert */
				/* 暂时不做处理 */
			}
			if (i == 11) { /* Home */
				/* 暂时不做处理 */
			}
			if (i == 12) { /* PgUp */
				/* 暂时不做处理 */
			}
			if (i == 13) { /* Delete */
				/* 暂时不做处理 */
			}
			if (i == 14) { /* End */
				/* 暂时不做处理 */
			}
			if (i == 15) { /* PgDn */
				/* 暂时不做处理 */
			}
			if (i == 16) { /* 鼠标左键按下 */
				/* 暂时不做处理 */
			}
			if (i == 17) { /* 鼠标右键按下 */
				/* 暂时不做处理 */
			}
			if (i == 18) { /* 鼠标中键按下 */
				/* 暂时不做处理 */
			}
			if (256 <= i && i <= 511) { /* 键盘数据 */
				if (i == 8 + 256) {
					/* 退格键 */
					if (cons.cur_x > (8 + cons.pmtlen * 6)) {
						/* 擦除光标后前移 */
						console_putchar(&cons, ' ', 0);
						cons.cur_x -= 6;
					} else {
						/* console_beep(2000000, 150); */
					}
				} else if (i == 10 + 256) {
					/* 回车键 */
					/* 擦除光标后换行 */
					console_putchar(&cons, ' ', 0);
					cmdline[(cons.cur_x - 8) / 6 - cons.pmtlen] = 0;
					if (cmdline[0] != 0) { /* 非空行 */
						if (cons.cmd_num > 64) {
							cons.cmd_index = cons.cmd_num = 0;
						}
						strcpy(cons.cmd_buf[cons.cmd_num++], cmdline);
						cons.cmd_index = cons.cmd_num;
					}
					console_newline(&cons);
					strcpy(cmdline2, cmdline);
					cmdline_split(cmdline2, &arg);
					console_run(arg, cmdline, &cons, fat, memtotal);
					if (cons.sht == 0) {
						command_exit(&cons, fat);
					}
					/* 显示提示符 */
					if (cons.echo) {
						console_puts(&cons, cons.prompt);
					}
				} else {
					/* 一般字符 */
					if (cons.cur_x < 240) {
						/* 显示字符并后移光标 */
						cmdline[(cons.cur_x - 8) / 6 - cons.pmtlen] = i - 256;
						console_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* 重新显示光标 */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0) {
					varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.bc, 
						cons.cur_x + 1, cons.cur_y, 
						cons.cur_x + 5, cons.cur_y + 5);
					varm_fill_rectangle(cons.sht->buf, cons.sht->bxsize, cons.cur_c, 
						cons.cur_x + 1, cons.cur_y + 6, 
						cons.cur_x + 5, cons.cur_y + 7);
				}
				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, 
					cons.cur_x + 6, cons.cur_y + 8);
			}
		}
	}
}

void console_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2] = {chr, 0};
	int t;
	if (s[0] == 0x06) { /* 反显 */
		t = cons->bc;
		cons->bc = cons->fc;
		cons->fc = t;
	} else if (s[0] == 0x07) { /* 响铃 */
		console_beep(2000000, 150);
	} else if (s[0] == 0x08) { /* 退格 */
		if (cons->cur_x > (8 + cons->pmtlen * 6)) {
			cons->cur_x -= 6;
		}
	} else if (s[0] == 0x09) { /* 制表符 */
		for (;;) {
			if (cons->sht != 0) {
				sheet_draw_mstring(cons->sht, cons->cur_x, cons->cur_y, 
					cons->fc, cons->bc, " ", 1);
			}
			cons->cur_x += 6;
			if (cons->cur_x == 8 + 240) {
				console_newline(cons);
			}
			if ((cons->cur_x - 8) % 24 == 0) {
				break;	/* 被24整除则break */
			}
		}
	} else if (s[0] == 0x0a) { /* 换行 */
		console_newline(cons);
	} else if (s[0] == 0x0d) { /* 回车 */
		/* 暂时不做处理 */
	} else { /* 一般字符 */
		if (cons->sht != 0) {
			sheet_draw_mstring(cons->sht, cons->cur_x, cons->cur_y, 
				cons->fc, cons->bc, s, 1);
		}
		if (move != 0) {
			/* move为0时光标不后移 */
			cons->cur_x += 6;
			if (cons->cur_x == 8 + 240) {
				console_newline(cons);
			}
		}
	}
	return;
}

void console_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 114) {
		cons->cur_y += 8; /* 换行 */
	} else {
		/* 滚动 */
		if (sheet != 0) {
			for (y = 28; y < 28 + 120; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = 
						sheet->buf[x + (y + 8) * sheet->bxsize];
				}
			}
			for (y = 28 + 120; y < 28 + 130; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = cons->bc;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 130);
		}
	}
	cons->cur_x = 8;
	return;
}

void console_puts(struct CONSOLE *cons, char *s)
{
	for (; *s != 0; s++) {
		console_putchar(cons, *s, 1);
	}
	return;
}

void console_putsl(struct CONSOLE *cons, char *s, int l)
{
	int i;
	for (i = 0; i < l; i++) {
		console_putchar(cons, s[i], 1);
	}
	return;
}

void console_beep(int tone, int ms)
{
	int i;
	struct TASK *task = task_getnow();
	int fifobuf[128];
	struct FIFO fifo;
	fifo_init(&fifo, 128, fifobuf, task);
	struct TIMER *timer;
	timer = timer_alloc();
	timer_init(timer, &fifo, 11);
	timer_settime(timer, ms / 10);
	i = 1193180000 / tone;
	io_out8(0x43, 0xb6);
	io_out8(0x42, i & 0xff);
	io_out8(0x42, i >> 8);
	i = io_in8(0x61);
	io_out8(0x61, (i | 0x03) & 0x0f);
	for (;;) {
		io_cli();
		if (fifo_status(&fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo_pop(&fifo);
			io_sti();
			if (i == 11) {
				i = io_in8(0x61);
				io_out8(0x61, i & 0x0d);
				break;
			}
		}
	}
	timer_free(timer);
}

int console_print(struct CONSOLE *cons, char *format, ...)
{
	va_list ap;
	char s[1024];
	int i;
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	console_puts(cons, s);
	va_end(ap);
	return i;
}

void console_run(struct ARG arg, char *cmdline, 
				 struct CONSOLE *cons, int *fat, int memtotal)
{
	if (check_command(arg, "attrib")) {
		command_attrib(cons, arg);
	} else if (check_command(arg, "chtime")) {
		command_chtime(cons, arg);
	} else if (check_command(arg, "cls")) {
		command_cls(cons, arg);
	} else if (check_command(arg, "color")) {
		command_color(cons, arg);
	} else if (check_command(arg, "crc32")) {
		command_crc32(cons, fat, arg);
	} else if (check_command(arg, "disk")) {
		command_disk(cons, arg);
	} else if (check_command(arg, "delete")) {
		command_delete(cons, arg);
	} else if (check_command(arg, "dir")) {
		command_dir(cons, arg);
	} else if (check_command(arg, "echo")) {
		command_echo(cons, arg);
	} else if (check_command(arg, "exit")) {
		command_exit(cons, fat);
	} else if (check_command(arg, "find")) {
		command_find(cons, arg);
	} else if (check_command(arg, "help")) {
		command_help(cons, arg);
	} else if (check_command(arg, "history")) {
		command_history(cons, arg);
	} else if (check_command(arg, "mem")) {
		command_mem(cons, arg, memtotal);
	} else if (check_command(arg, "more")) {
		command_more(cons, fat, arg);
	} else if (check_command(arg, "pause")) {
		command_pause(cons, arg);
	} else if (check_command(arg, "prompt")) {
		command_prompt(cons, arg);
	} else if (check_command(arg, "rename")) {
		command_rename(cons, arg);
	} else if (check_command(arg, "run")) {
		command_run(cons, arg, memtotal);
	} else if (check_command(arg, "shutdown")) {
		command_shutdown(cons, arg);
	} else if (check_command(arg, "task")) {
		command_task(cons, arg);
	} else if (check_command(arg, "time")) {
		command_time(cons, arg);
	} else if (check_command(arg, "type")) {
		command_type(cons, fat, arg);
	} else if (check_command(arg, "ver")) {
		command_ver(cons, arg, memtotal);
	}  else if (cmdline[0] != 0) {
		if (console_run_app(cons, fat, cmdline, memtotal, arg) == 0) {
			/* 既非命令又非空行 */
			console_print(cons, "Unknown Command \'%s\'.\n\n", arg.argv[0]);
		}
	}
	return;
}
