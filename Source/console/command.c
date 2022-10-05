/*
	File: Source/console/command.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void command_ncrun(struct CONSOLE *cons, struct ARG arg, int memtotal);
int console_getchar(void);
char *console_getstring(struct CONSOLE *cons, char *src);
inline void syntax_error(struct CONSOLE *cons);
inline void switch_error(struct CONSOLE *cons, char *sw);
inline int no_switch(struct CONSOLE *cons, struct ARG arg);
inline void no_permissions(struct CONSOLE *cons);

static char help_text_attrib[] = {
	"Display or set the attributes of a file.\n"
	"Usage : ATTRIB <NAME> [/RHSA]\n"
	" NAME   File name.\n"
	" R      Read-only file.\n"
	" H      Hidden file.\n"
	" S      System file.\n"
	" A      Archive file.\n"
};

static char help_text_chtime[] = {
	"Modify the system time.\n"
	"Usage : CHTIME\n"
};

static char help_text_cls[] = {
	"Clear the screen.\n"
	"Usage : CLS\n"
};

static char help_text_color[] = {
	"Change the color of the console output text.\n"
	"Usage : COLOR [ATTR]\n"
	" ATTR   Color attribute.\n"
};

static char help_text_crc32[] = {
	"Calculates the CRC32 value of the specified file.\n"
	"Usage : CRC32 <NAME>\n"
	" NAME   File name.\n"
};

static char help_text_delete[] = {
	"Delete one or more files.\n"
	"Usage : DELETE <NAMES>\n"
	" NAMES  File name(s).\n"
};

static char help_text_disk[] = {
	"Display disk information.\n"
	"Usage : DISK\n"
};

static char help_text_dir[] = {
	"List all files.\n"
	"Usage : DIR [-N]\n"
	" -N     Use page breaks.\n"
};

static char help_text_echo[] = {
	"Show a message, display or set command echo.\n"
	"Usage : ECHO [MSG]\n"
	"        ECHO [ON|OFF]\n"
	" MSG    The message to be displayed.\n"
	" ON|OFF Command echo settings.\n"
};

static char help_text_exit[] = {
	"Exit the console.\n"
	"Usage : EXIT\n"
};

static char help_text_find[] = {
	"Check whether a file exists.\n"
	"Usage : FIND <NAME>\n"
	" NAME   File name.\n"
};

static char help_text_help[] = {
	"Provide MitOS command help.\n"
	"Usage : HELP [-N]\n"
	"        HELP [CMD]\n"
	" -N     Use page breaks.\n"
	" CMD    Command string.\n"
};

static char help_text_histoty[] = {
	"Display or clear the command history.\n"
	"Usage : HISTORY [-C|-N]\n"
	" -C     Clear the command history.\n"
	" -N     Use page breaks.\n"
};

static char help_text_mem[] = {
	"Displays the physical memory of the computer.\n"
	"Usage : MEM [-H]\n"
	" -H     Display in human-readable medium.\n"
};

static char help_text_more[] = {
	"Display output screen by screen.\n"
	"Usage : MORE <NAME> [-F]\n"
	" NAME   File name.\n"
	" -F     Force display of binary characters.\n"
};

static char help_text_pause[] = {
	"Pause the program and display \"Press Any Key to Continue...\".\n"
	"Usage : PAUSE\n"
};

static char help_text_prompt[] = {
	"Modify the current command prompt.\n"
	"Usage : PROMPT <TEXT>\n"
	" TEXT    The command prompt.\n"
};

static char help_text_rename[] = {
	"Rename a file.\n"
	"Usage : RENAME <NAME1> <NAME2>\n"
	" NAME1  Source file name.\n"
	" NAME2  Sestination file name.\n"
};

static char help_text_run[] = {
	"Start a new console window to run the program.\n"
	"Usage : RUN <NAME>\n"
	" NAME   Program name.\n"
};

static char help_text_shutdown[] = {
	"Shut down or restart the computer.\n"
	"Usage : SHUTDOWN [-S|-R] [-W]\n"
	" -S     Shut down the computer.\n"
	" -R     Restart the computer.\n"
	" -W     The time (in seconds) to wait.\n"
};

static char help_text_task[] = {
	"Display a list of running tasks.\n"
	"Usage : TASK [-N] [-SEL] [-L] [-P]\n"
	" -N     Use page breaks.\n"
	" -SEL   Task segment number.\n"
	" -L     The task level to be set.\n"
	" -P     The priority of the task to be set.\n"
};

static char help_text_time[] = {
	"Display the system date and time.\n"
	"Usage : TIME\n"
};

static char help_text_type[] = {
	"Display the contents of the text file.\n"
	"Usage : TYPE <NAME> [-F]\n"
	" NAME   File name.\n"
	" -F     Force display of binary characters.\n"
};

static char help_text_ver[] = {
	"Display MitOS version.\n"
	"Usage : VER\n"
};

void command_attrib(struct CONSOLE *cons, struct ARG arg)
{
	int i, prop = 0;
	char filename[12] = {0}, attr[5] = "    ";
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '/') {
			if (arg.argv[i][1] == 'R' || arg.argv[i][1] == 'r') {
				prop += 0x01;
			} else if (arg.argv[i][1] == 'H' || arg.argv[i][1] == 'h') {
				prop += 0x02;
			} else if (arg.argv[i][1] == 'S' || arg.argv[i][1] == 's') {
				prop += 0x04;
			} else if (arg.argv[i][1] == 'A' || arg.argv[i][1] == 'a') {
				prop += 0x20;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		} else {
			strncpy(filename, arg.argv[i], 12);
		}
	}
	if (*filename == 0) {
		syntax_error(cons);
		return;
	}
	struct FILEINFO *finfo = 
		file_search(filename, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		/* 找到文件 */
		if (prop == 0) {
			if (finfo->type & 0x20) {
				attr[3] = 'A';
			}
			if (finfo->type & 0x04) {
				attr[2] = 'S';
			}
			if (finfo->type & 0x02) {
				attr[1] = 'H';
			}
			if (finfo->type & 0x01) {
				attr[0] = 'R';
			}
			console_print(cons, "%s\n", attr);
		} else {
			finfo->type = prop;
		}
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_chtime(struct CONSOLE *cons, struct ARG arg)
{
	if (no_switch(cons, arg)) {
		return;
	}
	unsigned int year, month, mday, hour, minute, second, wday, status = 0, key = 0;
	static char *weeks[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	};
	year = get_year();
	month = get_month();
	mday = get_day_of_month();
	hour = get_hour();
	minute = get_minute();
	second = get_second();
	wday = get_day_of_week();

	for (;;) {
		cons->cur_x = 8;
		if (key == 0x0a) {
			break;
		}

		if (status == 0) {
			/* 设置年份 */
			console_print(cons, "\x06%04d\x06/%02d/%02d  %02d:%02d:%02d  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '6') {
				status = 1;
			} else if (key == '2') {
				if (year > 1990) {
					year--;
				}
			} else if (key == '8') {
				if (year < 9999) {
					year++;
				}
			}
		} else if (status == 1) {
			/* 设置月份 */
			console_print(cons, "%04d/\x06%02d\x06/%02d  %02d:%02d:%02d  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '4') {
				status = 0;
			} else if (key == '6') {
				status = 2;
			} else if (key == '2') {
				if (month > 1) {
					month--;
				}
			} else if (key == '8') {
				if (month < 12) {
					month++;
				}
			}
		} else if (status == 2) {
			/* 设置日期 */
			console_print(cons, "%04d/%02d/\x06%02d\x06  %02d:%02d:%02d  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			int mon_days = 0, rn = 0;
			if (ISLEAP(year)) {
				rn = 1;
			}
			switch (month) {
				case 12:
				case 10:
				case  8:
				case  7:
				case  5:
				case  3:
				case  1:
					mon_days = 31;
					break;
				case 11:
				case  9:
				case  6:
				case  4:
					mon_days = 30;
					break;
				case  2:
					mon_days = 28 + rn;
					break;
			}
			if (key == '4') {
				status = 1;
			} else if (key == '6') {
				status = 3;
			} else if (key == '2') {
				if (mday > 1) {
					mday--;
				}
			} else if (key == '8') {
				if (mday < mon_days) {
					mday++;
				}
			}
		} else if (status == 3) {
			/* 设置时 */
			console_print(cons, "%04d/%02d/%02d  \x06%02d\x06:%02d:%02d  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '4') {
				status = 2;
			} else if (key == '6') {
				status = 4;
			} else if (key == '2') {
				if (hour > 1) {
					hour--;
				}
			} else if (key == '8') {
				if (hour < 23) {
					hour++;
				}
			}
		} else if (status == 4) {
			/* 设置分 */
			console_print(cons, "%04d/%02d/%02d  %02d:\x06%02d\x06:%02d  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '4') {
				status = 3;
			} else if (key == '6') {
				status = 5;
			} else if (key == '2') {
				if (minute > 1) {
					minute--;
				}
			} else if (key == '8') {
				if (minute < 59) {
					minute++;
				}
			}
		} else if (status == 5) {
			/* 设置秒 */
			console_print(cons, "%04d/%02d/%02d  %02d:%02d:\x06%02d\x06  %s     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '4') {
				status = 4;
			} else if (key == '6') {
				status = 6;
			} else if (key == '2') {
				if (second > 1) {
					second--;
				}
			} else if (key == '8') {
				if (second < 59) {
					second++;
				}
			}
		} else if (status == 6) {
			/* 设置星期 */
			console_print(cons, "%04d/%02d/%02d  %02d:%02d:%02d  \x06%s\x06     ", 
			            year, month, mday, hour, minute, second, weeks[wday]);
			key = console_getchar();
			key -= 256;
			if (key == '4') {
				status = 5;
			} else if (key == '2') {
				if (wday > 0) {
					wday--;
				}
			} else if (key == '8') {
				if (wday < 6) {
					wday++;
				}
			}
		}
	}

	set_year(year);
	set_month(month);
	set_day_of_month(mday);
	set_hour(hour);
	set_minute(minute);
	set_second(second);
	set_day_of_week(wday);
	console_puts(cons, "\n\n");
	return;
}

void command_cls(struct CONSOLE *cons, struct ARG arg)
{
	if (no_switch(cons, arg)) {
		syntax_error(cons);
		return;
	}
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = cons->bc;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void command_color(struct CONSOLE *cons, struct ARG arg)
{
	char *q, s[2] = " ";
	int x, y, old_fc = cons->fc, tbc, tfc;
	if (arg.argc == 1) {
		cons->bc = 0;
		cons->fc = 7;
		goto draw;
	}
	q = arg.argv[1];
	if ((('0' <= q[0] && q[0] <= '9') || ('a' <= q[0] && q[0] <= 'f')) && 
	    (('0' <= q[1] && q[1] <= '9') || ('a' <= q[1] && q[1] <= 'f'))) {
		s[0] = q[0]; tbc = strtol(s, 0, 16);
		s[0] = q[1]; tfc = strtol(s, 0, 16);
		if (tbc == tfc) {
			console_newline(cons);
			return;
		} else {
			cons->bc = tbc;
			cons->fc = tfc;
		}
draw:
		for (y = 26; y < 159; y++)
			for (x = 6; x < 251; x++)
				if (cons->sht->buf[x + y * cons->sht->bxsize] == old_fc) {
					cons->sht->buf[x + y * cons->sht->bxsize] = cons->fc;
				} else {
					cons->sht->buf[x + y * cons->sht->bxsize] = cons->bc;
				}
		sheet_refresh(cons->sht, 6, 26, 251, 160);
		console_newline(cons);
	} else {
		syntax_error(cons);
		return;
	}
	return;
}

void command_crc32(struct CONSOLE *cons, int *fat, struct ARG arg)
{
	int i;
	char filename[12] = {0};
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			switch_error(cons, arg.argv[i]);
			return;
		} else {
			strncpy(filename, arg.argv[i], 12);
		}
	}
	if (*filename == 0) {
		syntax_error(cons);
		return;
	}
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(filename, 
		(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	unsigned char *p;
	if (finfo != 0) {
		/* 找到文件 */
		p = (unsigned char *) memman_alloc_4k(memman, finfo->size);
		file_load(finfo->clustno, finfo->size, p, fat, 
			(unsigned char *) (ADR_DISKIMG + 0x003e00));
		console_print(cons, "CRC32: %X\n", get_crc32(p, finfo->size));
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_delete(struct CONSOLE *cons, struct ARG arg)
{
	int i;
	struct FILEINFO *finfo;
	if (arg.argc < 2) {
		syntax_error(cons);
		return;
	}
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			switch_error(cons, arg.argv[i]);
			return;
		} else {
			finfo = file_search(arg.argv[i], (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (!fdimg_delete_file((unsigned char *) ADR_DISKIMG, arg.argv[i])) {
				console_print(cons, "Deleted File \'%s\'.\n", arg.argv[i]);
			} else {
				console_print(cons, "File \'%s\' Not Found.\n", arg.argv[i]);
			}
		}
	}
	console_newline(cons);
	return;
}

void command_dir(struct CONSOLE *cons, struct ARG arg)
{
	int i, j, k, line = 0, sw_n = 0;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-n", 2)) {
				sw_n = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	char s[40];

	for (i = 0; i < 224; i++) {
		if (line > CONSOLE_LINE - 2 && sw_n) {
			console_puts(cons, "\x06-- More --\x06");
			console_getchar();
			console_puts(cons, "\n");
			line = 0;
		}
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				line++;
				unsigned short year = finfo->date & 0xfe00;
				year = year >> 9;
				unsigned short month = finfo->date & 0x1e0;
				month = month>> 5;
				unsigned short day = finfo->date & 0x1f;
				unsigned short hour = finfo->time & 0xf800;
				hour = hour >> 11;
				unsigned short minute = finfo->time & 0x7e0;
				minute = minute >> 5;
				unsigned short second = finfo->time & 0x1f;
				sprintf(s, "             %7d %04d/%02d/%02d %02d:%02d:%02d", 
						finfo[i].size, 
						year + 1980, month, day, 
						hour, minute, second);
				for (k = 0; finfo[i].name[k] != ' ' && k < 8; k++) {}
				strncpy(s, finfo[i].name, k);
				for (j = 0; finfo[i].ext[j] != ' ' && j < 3; j++) {}
				if (j != 0) {
					s[k] = '.';
					strncpy(s + k + 1, finfo[i].ext, j);
				}
				console_puts(cons, s);
			}
		}
	}
	console_newline(cons);
	return;
}

void command_disk(struct CONSOLE *cons, struct ARG arg)
{
	if (no_switch(cons, arg)) {
		return;
	}
	char *BS_VolLab = (char *) (ADR_DISKIMG + 43);
	char *BS_FileSysType = (char *) (ADR_DISKIMG + 54);
	console_puts(cons, "Volume Label: ");
	console_putsl(cons, BS_VolLab, 11);
	console_newline(cons);
	console_puts(cons, "File System: ");
	console_putsl(cons, BS_FileSysType, 8);
	console_newline(cons);
	unsigned int TotalSpace = *((unsigned short *) (ADR_DISKIMG + 19)) * *((unsigned short *) (ADR_DISKIMG + 11));
	unsigned int FreeSpace = fat_get_rest_sectors((unsigned char *) ADR_DISKIMG) * *((unsigned short *) (ADR_DISKIMG + 11));
	console_print(cons, "Disk size: %d\nFree: %d, %d%%.\n\n", TotalSpace, FreeSpace, FreeSpace * 100 / TotalSpace);
	return;
}

void command_echo(struct CONSOLE *cons, struct ARG arg)
{
	if (arg.argc <= 1) {
		if (cons->echo) {
			console_puts(cons, "ECHO Is ON.\n\n");
		} else {
			console_puts(cons, "ECHO Is OFF.\n\n");
		}
		return;
	} else {
		if (strcmp(strlwr(arg.argv[1]), "on") == 0) {
			cons->echo = 1;
			cons->pmtlen = strlen(cons->prompt);
		} else if (strcmp(strlwr(arg.argv[1]), "off") == 0) {
			cons->echo = 0;
			cons->pmtlen = 0;
		} else {
			int i;
			for (i = 1; i < arg.argc; i++) {
				console_print(cons, "%s ", arg.argv[i]);
			}
		}
	}
	console_puts(cons, "\n\n");
	return;
}

void command_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_getnow();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO *fifo = (struct FIFO *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo_push(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768-1023 */
	} else {
		fifo_push(fifo, task - taskctl->tasks0 + 1024);	/* 1024-2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void command_find(struct CONSOLE *cons, struct ARG arg)
{
	int i;
	char filename[12] = {0};
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			switch_error(cons, arg.argv[i]);
			return;
		} else {
			strncpy(filename, arg.argv[i], 12);
		}
	}
	if (*filename == 0) {
		syntax_error(cons);
		return;
	}
	struct FILEINFO *finfo = 
		file_search(filename, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0 && finfo->name[0] != 0xe5) {
		/* 找到文件 */
		console_puts(cons, "File Found.\n");
		console_print(cons, "%d Bytes\n", finfo->size);
		unsigned short year = finfo->date & 0xfe00;
		year = year >> 9;
		unsigned short month = finfo->date & 0x1e0;
		month = month>> 5;
		unsigned short day = finfo->date & 0x1f;
		console_print(cons, "Date: %04d/%02d/%02d\n", year + 1980, month, day);
		unsigned short hour = finfo->time & 0xf800;
		hour = hour >> 11;
		unsigned short minute = finfo->time & 0x7e0;
		minute = minute >> 5;
		unsigned short second = finfo->time & 0x1f;
		console_print(cons, "Time: %02d:%02d:%02d\n", hour, minute, second);
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_help(struct CONSOLE *cons, struct ARG arg)
{
	int i, k = 0, sw_n = 0;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-n", 2)) {
				sw_n = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
	if (arg.argc == 1 || (arg.argc == 2 && arg.argv[1][0] == '-')) {
		static char *cmds[] = {
			"ATTRIB    Display Or Set File Attrib. \n",
			"CHTIME    Modify System Time.         \n",
			"CLS       Clear The Console.          \n",
			"COLOR     Change Color Of Text.       \n",
			"CRC32     Calculate File CRC32 Value. \n",
			"DELETE    Delete One Or More Files.   \n",
			"DISK      Display Disk Information.   \n",
			"DIR       Display File List.          \n",
			"ECHO      Display Messages.           \n",
			"EXIT      Close The Console.          \n",
			"FIND      Ckeck File Exist.           \n",
			"HELP      Display OS Help.            \n",
			"HISTORY   Display Command History.    \n",
			"MEM       Display Memory.             \n",
			"MORE      Display Screen By Screen.   \n",
			"PAUSE     Pause Batch Program.        \n",
			"PROMPT    Modify Command Prompt.      \n",
			"RENAME    Rename A File.              \n",
			"RUN       Run A Program.              \n",
			"SHUTDOWN  Shut Down Or Reboot.        \n",
			"TASK      Display Or Set The Tasks.   \n",
			"TIME      Display System Time.        \n",
			"TYPE      Display Text File.          \n",
			"VER       Display OS Version.         \n"
		};
		for (i = 0; i < 24; i++) {
			if (k > CONSOLE_LINE - 2 && sw_n) {
				console_puts(cons, "\x06-- More --\x06");
				console_getchar();
				console_puts(cons, "\n");
				k = 0;
			}
			console_puts(cons, cmds[i]);
			k++;
		}
	} else {
		if (strcmp(strlwr(arg.argv[1]), "attrib") == 0) {
			console_puts(cons, help_text_attrib);
		} else if (strcmp(strlwr(arg.argv[1]), "chtime") == 0) {
			console_puts(cons, help_text_chtime);
		} else if (strcmp(strlwr(arg.argv[1]), "cls") == 0) {
			console_puts(cons, help_text_cls);
		} else if (strcmp(strlwr(arg.argv[1]), "color") == 0) {
			console_puts(cons, help_text_color);
		} else if (strcmp(strlwr(arg.argv[1]), "crc32") == 0) {
			console_puts(cons, help_text_crc32);
		} else if (strcmp(strlwr(arg.argv[1]), "delete") == 0) {
			console_puts(cons, help_text_delete);
		} else if (strcmp(strlwr(arg.argv[1]), "disk") == 0) {
			console_puts(cons, help_text_disk);
		} else if (strcmp(strlwr(arg.argv[1]), "dir") == 0) {
			console_puts(cons, help_text_dir);
		} else if (strcmp(strlwr(arg.argv[1]), "echo") == 0) {
			console_puts(cons, help_text_echo);
		} else if (strcmp(strlwr(arg.argv[1]), "exit") == 0) {
			console_puts(cons, help_text_exit);
		} else if (strcmp(strlwr(arg.argv[1]), "find") == 0) {
			console_puts(cons, help_text_find);
		} else if (strcmp(strlwr(arg.argv[1]), "help") == 0) {
			console_puts(cons, help_text_help);
		} else if (strcmp(strlwr(arg.argv[1]), "history") == 0) {
			console_puts(cons, help_text_histoty);
		} else if (strcmp(strlwr(arg.argv[1]), "mem") == 0) {
			console_puts(cons, help_text_mem);
		} else if (strcmp(strlwr(arg.argv[1]), "more") == 0) {
			console_puts(cons, help_text_more);
		} else if (strcmp(strlwr(arg.argv[1]), "pause") == 0) {
			console_puts(cons, help_text_pause);
		} else if (strcmp(strlwr(arg.argv[1]), "prompt") == 0) {
			console_puts(cons, help_text_prompt);
		} else if (strcmp(strlwr(arg.argv[1]), "rename") == 0) {
			console_puts(cons, help_text_rename);
		} else if (strcmp(strlwr(arg.argv[1]), "run") == 0) {
			console_puts(cons, help_text_run);
		} else if (strcmp(strlwr(arg.argv[1]), "shutdown") == 0) {
			console_puts(cons, help_text_shutdown);
		} else if (strcmp(strlwr(arg.argv[1]), "task") == 0) {
			console_puts(cons, help_text_task);
		} else if (strcmp(strlwr(arg.argv[1]), "time") == 0) {
			console_puts(cons, help_text_time);
		} else if (strcmp(strlwr(arg.argv[1]), "type") == 0) {
			console_puts(cons, help_text_type);
		} else if (strcmp(strlwr(arg.argv[1]), "ver") == 0) {
			console_puts(cons, help_text_ver);
		} else {
			console_print(cons, "Command \'%s\' was not found.\n", arg.argv[1]);
		}
	}
	console_newline(cons);
	return;
}

void command_history(struct CONSOLE *cons, struct ARG arg)
{
	int i, k = 0, sw_c = 0, sw_n = 0;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-c", 2)) {
				sw_c = 1;
			} else if (check_switch(arg.argv[i], "-n", 2)) {
				sw_n = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
	if (sw_c) {
		cons->cmd_index = cons->cmd_num = 0;
	} else {
		for (i = 0; i < cons->cmd_num; i++) {
			if (k > CONSOLE_LINE - 2 && sw_n) {
				console_puts(cons, "\x06-- More --\x06");
				console_getchar();
				console_puts(cons, "\n");
				k = 0;
			}
			console_puts(cons, cons->cmd_buf[i]);
			console_newline(cons);
			k++;
		}
	}
	console_newline(cons);
	return;
}

void command_mem(struct CONSOLE *cons, struct ARG arg, int memtotal)
{
	int i, sw_h = 0;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-h", 2)) {
				sw_h = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	if (sw_h) {
		console_print(cons, "Total%8dMiB\nFree %8dMiB\n\n", 
		            memtotal / (1024 * 1024), memman_getfree(memman) / (1024 * 1024));
	} else {
		console_print(cons, "Total  %8X Bytes\nFree   %8X Bytes\n\n", 
		            memtotal, memman_getfree(memman));
	}
	return;
}

void command_more(struct CONSOLE *cons, int *fat, struct ARG arg)
{
	int i, x = 0, line = 0, key, sw_f = 0;
	char filename[12] = {0};
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-f", 2)) {
				sw_f = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		} else {
			strncpy(filename, arg.argv[i], 12);
		}
	}
	if (*filename == 0) {
		syntax_error(cons);
		return;
	}
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = 
		file_search(filename, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	unsigned char *p;
	if (finfo != 0) {
		/* 找到文件 */
		p = (unsigned char *) memman_alloc_4k(memman, finfo->size);
		file_load(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		if (!sw_f) {
			for (i = 0; i < finfo->size; i++) {
				if (!((0x20 <= p[i] && p[i] <= 0x7f) || 
					(p[i] == 0x09 || p[i] == 0x0a || p[i] == 0x0d))) {
					console_puts(cons, "Use -F Switch To Display A Binary File.\n\n");
					return;
				}
			}
		}
		for (i = 0; i < finfo->size; i++) {
			if (x == CONSOLE_COLUMN) {
				for (; i < finfo->size && p[i] != 0x0a; i++) {}
				i++;
				x = 0;
				line++;
			}
			if (line > CONSOLE_LINE - 2) {
				console_print(cons, "\x06-- More[%02d%%] --\x06", i * 100 / finfo->size);
				console_puts(cons, "");
				for (;;) {
					key = console_getchar();
					key -= 256;
					if (key == 'q' || key == 'Q') {
						console_puts(cons, "\n\n");
						return;
					} else if (key == ' ') {
						line = 0;
						break;
					} else if (key == 0x0a) {
						line = CONSOLE_LINE - 2;
						break;
					}
				}
				console_newline(cons);
			}
			if (p[i] == 0x0a) {
				console_newline(cons);
				x = 0;
				line++;
			} else {
				console_putchar(cons, p[i], 1);
				x++;
			}
		}
		console_newline(cons);
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_pause(struct CONSOLE *cons, struct ARG arg)
{
	if (no_switch(cons, arg)) {
		return;
	}
	console_puts(cons, "Press Any Key to Continue...");
	console_getchar();
	console_puts(cons, "\n\n");
	return;
}

void command_prompt(struct CONSOLE *cons, struct ARG arg)
{
	if (arg.argc < 2) {
		syntax_error(cons);
		return;
	}
	char pmtch = cons->prompt[cons->pmtlen - 1];
	strncpy(cons->prompt, arg.argv[1], 16);
	cons->prompt[strlen(arg.argv[1])] = pmtch;
	cons->prompt[strlen(arg.argv[1]) + 1] = 0;
	cons->pmtlen = strlen(cons->prompt);
	console_newline(cons);
	return;
}

void command_rename(struct CONSOLE *cons, struct ARG arg)
{
	if (arg.argc < 3) {
		syntax_error(cons);
		return;
	}
	struct FILEINFO *finfo = 
		file_search(arg.argv[1], (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		/* 找到文件 */
		char dirname[12];
		convert_to_dirName(arg.argv[2], dirname);
		memcpy(finfo->name, dirname, 11);
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_run(struct CONSOLE *cons, struct ARG arg, int memtotal)
{
	int i, j;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	keywin_off(shtctl->sheets[shtctl->top - 1]);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO *fifo = &sht->task->fifo;
	sheet_move(sht, 32, 4);
	sheet_setheight(sht, shtctl->top);
	/* 将命令复制到新控制台窗口 */
	for (i = 1; i < arg.argc; i++) {
		for (j = 0; arg.argv[i][j] != 0; j++) {
			fifo_push(fifo, arg.argv[i][j] + 256);
		}
		fifo_push(fifo, ' ' + 256);
	}
	fifo_push(fifo, 10 + 256);	/* Enter */
	key_win = sht;
	keywin_on(key_win);
	console_newline(cons);
	return;
}

void command_shutdown(struct CONSOLE *cons, struct ARG arg)
{
	int i, sw_r = 0, sw_s = 0, sw_w = 0;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-r", 2)) {
				sw_r = 1;
			} else if (check_switch(arg.argv[i], "-s", 2)) {
				sw_s = 1;
			} else if (check_switch(arg.argv[i], "-w", 2)) {
				sw_w = strtol(arg.argv[i] + 2, 0, 10);
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
run:
	if (sw_w == 0) {
		if (sw_r) {
			kernel_reboot();
		} else if (sw_s) {
			kernel_shutdown();
		}
	} else {
		struct TASK *task = task_getnow();
		int fifobuf[128];
		struct FIFO fifo;
		fifo_init(&fifo, 128, fifobuf, task);
		struct TIMER *timer;
		timer = timer_alloc();
		timer_init(timer, &fifo, 11);
		timer_settime(timer, sw_w * 100);
		for (;;) {
			io_cli();
			if (fifo_status(&fifo) == 0) {
				task_sleep(task);
				io_sti();
			} else {
				i = fifo_pop(&fifo);
				io_sti();
				if (i == 11) {
					break;
				}
			}
		}
		timer_free(timer);
		sw_w = 0;
		goto run;
	}
	console_newline(cons);
	return;
}

void command_task(struct CONSOLE *cons, struct ARG arg)
{
	int i, k = 0, sw_n = 0, sw_sel = 0, sw_l = 2, sw_p = 2;
	struct TASK *task;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-n", 2)) {
				sw_n = 1;
			} else if (check_switch(arg.argv[i], "-sel", 4)) {
				sw_sel = strtol(arg.argv[i] + 4, 0, 10);
			} else if (check_switch(arg.argv[i], "-l", 2)) {
				sw_l = strtol(arg.argv[i] + 2, 0, 10);
			} else if (check_switch(arg.argv[i], "-p", 2)) {
				sw_p = strtol(arg.argv[i] + 2, 0, 10);
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		}
	}
	if (sw_sel != 0) {
		for (i = 0; i < MAX_TASKS; i++) {
			task = &taskctl->tasks0[i];
			if (task->flags == 0) {
				break;
			}
			if (task->sel == sw_sel) {
				if (1 <= sw_l && sw_l <= 9) {
					if (1 <= sw_p && sw_p <= 10) {
						task_run(&taskctl->tasks0[i], sw_l, sw_p);
					} else {
						console_puts(cons, 
							"\'Priority\' should be between 1 and 10.\n");
					}
				} else {
					console_puts(cons, 
						"\'Level\' should be between 1 and 9.\n");
				}
				console_newline(cons);
				return;
			}
		}
		console_puts(cons, "No task was found.\n\n");
		return;
	}
	console_puts(cons, "Sel  Address   Level  Priority\n"
					   "------------------------------\n");
	for (i = 0; i < MAX_TASKS; i++) {
		task = &taskctl->tasks0[i];
		if (task->flags == 0) {
			break;
		}
		if (k > CONSOLE_LINE - 2 - 2 && sw_n) {
			console_puts(cons, "\x06-- More --\x06");
			console_getchar();
			console_puts(cons, "\nSel  Address   Level  Priority\n"
							     "------------------------------\n");
			k = 0;
		}
		console_print(cons, "%-4d %08X %6d %9d\n",
					task->sel, (int) task, task->level, task->priority);
		k++;
	}
	console_newline(cons);
	return;
}

void command_time(struct CONSOLE *cons, struct ARG arg)
{
	if (no_switch(cons, arg)) {
		return;
	}
	static char *weeks[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	};
	console_print(cons, "%04d/%02d/%02d  ", get_year(), 
				get_month(), get_day_of_month());
	console_puts(cons, weeks[get_day_of_week()]);
	console_newline(cons);
	console_print(cons, "%02d:%02d:%02d\n\n", get_hour(), 
				get_minute(), get_second());
	return;
}

void command_type(struct CONSOLE *cons, int *fat, struct ARG arg)
{
	int i, sw_f = 0;
	char filename[12] = {0};
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			if (check_switch(arg.argv[i], "-f", 2)) {
				sw_f = 1;
			} else {
				switch_error(cons, arg.argv[i]);
				return;
			}
		} else {
			strncpy(filename, arg.argv[i], 12);
		}
	}
	if (*filename == 0) {
		syntax_error(cons);
		return;
	}
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(filename, 
		(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	unsigned char *p;
	if (finfo != 0) {
		/* 找到文件 */
		p = (unsigned char *) memman_alloc_4k(memman, finfo->size);
		file_load(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		fat_find_file((unsigned char *) ADR_DISKIMG, filename);
		fdimg_add_file((unsigned char *) ADR_DISKIMG, p, finfo->size, "a.c", 0x20);
		if (!sw_f) {
			for (i = 0; i < finfo->size; i++) {
				if (!((0x20 <= p[i] && p[i] <= 0x7f) || 
					(p[i] == 0x09 || p[i] == 0x0a || p[i] == 0x0d))) {
					console_puts(cons, "Use -F Switch To See A Binary File.\n\n");
					return;
				}
			}
		}
		console_putsl(cons, p, finfo->size);
		console_newline(cons);
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* 未找到文件 */
		console_puts(cons, "File Not Found.\n");
	}
	console_newline(cons);
	return;
}

void command_ver(struct CONSOLE *cons, struct ARG arg, int memtotal)
{
	// if (no_switch(cons, arg)) {
	// 	return;
	// }
	// console_puts(cons, "MLworkshop MitOS\n");
	// console_print(cons, "Version %d.%d.%d.%d\nTotal RAM: %d MiB\n\n", 
	// 			(OS_VERSION & 0xf000) >> 12,
	// 			(OS_VERSION & 0x0f00) >>  8,
	// 			(OS_VERSION & 0x00f0) >>  4,
	// 			(OS_VERSION & 0x000f) >>  0,
	// 			memtotal / (1024 * 1024));
	// console_puts(cons, "Copyright (c) MLworkshop, All Rights Reserved.\n\n");
	unsigned char buf[256] = {0xdd};
	hd_read_sector(0, 0, 0, buf, 256);
	console_print(cons, "%02x\n", *buf);
	return;
}

int console_run_app(struct CONSOLE *cons, int *fat, char *cmdline, int memtotal, struct ARG arg)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	struct FILEINFO *finfo;
	char name[18], *p, *q;
	struct TASK *task = task_getnow();
	int i, segsiz, datsiz, esp, datmit, appsiz;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* 根据命令行生成文件名 */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0;

	/* 寻找文件 */
	finfo = file_search(name, (struct FILEINFO *) 
						(ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* 若找不到文件，则在文件名后加上".MIT"重新寻找 */
		name[i    ] = '.';
		name[i + 1] = 'M';
		name[i + 2] = 'I';
		name[i + 3] = 'T';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) 
							(ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		/* 找到文件 */
		appsiz = finfo->size;
		p = (unsigned char *) memman_alloc_4k(memman, appsiz);
		file_load(finfo->clustno, appsiz, p, fat, 
			(unsigned char *) (ADR_DISKIMG + 0x003e00));
		if (appsiz >= 36 && strncmp(p + 4, "MitC", 4) == 0 && *p == 0x00) {
			/*
				if (cons->sht != 0) {
					if (*(p + 7) == 'G') {
						command_ncrun(cons, arg, memtotal);
						goto fin;
					}
				}
			*/
			segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			datmit = *((int *) (p + 0x0014));
			q = (char *) memman_alloc_4k(memman, segsiz);
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, appsiz - 1, 
						 (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1, 
						 (int) q, AR_DATA32_RW + 0x60);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[datmit + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if (sht->flags == SHT_FLAG_APPWIN && sht->task == task) {
					/* 找到未关闭的图层 */
					tbitemctl_remove(sht);
					sheet_free(sht); /* 关闭 */
				}
			}
			for (i = 0; i < 8; i++) { /* 找到未关闭的文件 */
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, 
								   task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
			memman_free_4k(memman, (int) q, segsiz);
		} else {
			if (strncmp(finfo->ext, "MIT", 3) == 0) {
				console_puts(cons, ".MIT File Format Error.\n");
			} else {
				console_puts(cons, "Select A Program To Open The File.\n");
			}
		}
/* fin: */
		memman_free_4k(memman, (int) p, appsiz);
		console_newline(cons);
		return 1;
	}
	/* 未找到文件 */
	return 0;
}

void command_ncrun(struct CONSOLE *cons, struct ARG arg, int memtotal)
{
	int i, j;
	struct TASK *task = open_console_task(0, memtotal);
	struct FIFO *fifo = &task->fifo;
	/* 将命令复制到新控制台窗口 */
	for (i = 0; i < arg.argc; i++) {
		for (j = 0; arg.argv[i][j] != 0; j++) {
			fifo_push(fifo, arg.argv[i][j] + 256);
		}
		fifo_push(fifo, ' ' + 256);
	}
	fifo_push(fifo, 10 + 256);	/* Enter */
	console_newline(cons);
	return;
}

int console_getchar(void)
{
	struct TASK *task = task_getnow();
	int l;
	do {
		l = fifo_pop(&task->fifo);
	} while (!(256 <= l && l <= 511));
	fifo_push(&task->fifo, 1);
	return l;
}

char *console_getstring(struct CONSOLE *cons, char *src)
{
	int i;
	for (;;) {
		i = console_getchar();
		if (i == 8 + 256) {
			/* 退格键 */
			if (cons->cur_x > 8) {
				cons->cur_x -= 6;
				sheet_draw_mstring(cons->sht, cons->cur_x, cons->cur_y, 
					COL8_000000, COL8_000000, " ", 1);
			} else {
				/* console_beep(2000000, 150); */
			}
		} else if (i == 10 + 256) {
			/* 回车键 */
			src[(cons->cur_x - 8) / 6] = 0;
			console_newline(cons);
			break;
		} else {
			/* 一般字符 */
			if (cons->cur_x < 240) {
				/* 显示字符并后移光标 */
				src[(cons->cur_x - 8) / 6] = i - 256;
				console_putchar(cons, i - 256, 1);
			}
		}
	}
	return src;
}

inline void syntax_error(struct CONSOLE *cons)
{
	console_puts(cons, "Command Syntax Error.\n\n");
	return;
}

inline void switch_error(struct CONSOLE *cons, char *sw)
{
	console_print(cons, "Invalid Switch: \'%s\'.\n\n", sw);
	return;
}

inline int no_switch(struct CONSOLE *cons, struct ARG arg)
{
	int i;
	for (i = 1; i < arg.argc; i++) {
		if (arg.argv[i][0] == '-') {
			switch_error(cons, arg.argv[i]);
			return 1;
		}
	}
	return 0;
}

inline void no_permissions(struct CONSOLE *cons)
{
	console_puts(cons, "Insufficient Permissions.\n\n");
	return;
}
