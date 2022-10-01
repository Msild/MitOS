/*
	File: MitOS/MitOS.h
	Copyright: MLworkshop
	Author: Msild
*/

/* boot.nas */
#define ADR_BOOTINFO	0x00000ff0	/* 启动信息地址 */
#define ADR_DISKIMG		0x00100000	/* 磁盘内容地址 */
struct BOOTINFO {	/* 0x0ff0-0x0fff */
	char cyls;	/* 读取扇区数 */
	char leds;	/* LED状态 */
	char vmode;	/* 显卡颜色位数 */
	char reserve;
	short scrnx, scrny;	/* 屏幕分辨率 */
	char *vram;
};

/* asmfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void asm_int_handler_00(void);
void asm_int_handler_01(void);
void asm_int_handler_02(void);
void asm_int_handler_03(void);
void asm_int_handler_04(void);
void asm_int_handler_05(void);
void asm_int_handler_06(void);
void asm_int_handler_07(void);
void asm_int_handler_08(void);
void asm_int_handler_09(void);
void asm_int_handler_0a(void);
void asm_int_handler_0b(void);
void asm_int_handler_0c(void);
void asm_int_handler_0d(void);
void asm_int_handler_0e(void);
void asm_int_handler_0f(void);
void asm_int_handler_10(void);
void asm_int_handler_11(void);
void asm_int_handler_12(void);
void asm_int_handler_20(void);
void asm_int_handler_21(void);
void asm_int_handler_2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_mit_api(void);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void kernel_reboot(void);
void kernel_shutdown(void);

/* fifo.c */
struct FIFO {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo_init(struct FIFO *fifo, int size, int *buf, struct TASK *task);
int fifo_push(struct FIFO *fifo, int data);
int fifo_pop(struct FIFO *fifo);
int fifo_status(struct FIFO *fifo);

/* graphic.c */
#define COL8_000000							0x00
#define COL8_0000AA							0x01
#define COL8_00AA00							0x02
#define COL8_00AAAA							0x03
#define COL8_AA0000							0x04
#define COL8_AA00AA							0x05
#define COL8_55AA00							0x06
#define COL8_AAAAAA							0x07
#define COL8_555555							0x08
#define COL8_5555FF							0x09
#define COL8_55FF55							0x0a
#define COL8_55FFFF							0x0b
#define COL8_FF5555							0x0c
#define COL8_FF55FF							0x0d
#define COL8_FFFF55							0x0e
#define COL8_FFFFFF							0x0f
void palette_init(void);
void palette_set(int start, int end, unsigned char *rgb);
void varm_fill_rectangle(unsigned char *vram, int xsize, unsigned char c, 
	int x0, int y0, int x1, int y1);
void vram_draw_line(unsigned char *vram, int xsize, unsigned char c,
	int x0, int y0, int x1, int y1);
void vram_init_srceen(char *vram, int x, int y,  char *bg_name);
void varm_draw_font(char *vram, int xsize, int x, int y, char c, char *font);
void vram_draw_string(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void vram_draw_mfont(char *vram, int xsize, int x, int y, char c, char *font);
void varm_draw_mstring(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor(char *mouse, char bc);
void vram_draw_block(char *vram, int vxsize, int pxsize, int pysize, 
				 int px0, int py0, char *buf, int bxsize);
int load_picture(char *filename, int *fat, char *vram, int bx, int x, int y, int mode);
void vram_draw_icon(char *vram, int bxsize, int x, int y, int id);
void vram_draw_menuitem(unsigned char *buf, int bxsize, int x, int y, int id, char *ds);

/* dsctbl.c */
#define ADR_IDT								0x0026f800
#define LIMIT_IDT							0x000007ff
#define ADR_GDT								0x00270000
#define LIMIT_GDT							0x0000ffff
#define ADR_BOTPAK							0x00280000
#define LIMIT_BOTPAK						0x0007ffff
#define AR_DATA32_RW						0x4092
#define AR_CODE32_ER						0x409a
#define AR_LDT								0x0082
#define AR_TSS32							0x0089
#define AR_INTGATE32						0x008e
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/* int.c */
#define PIC0_ICW1							0x0020
#define PIC0_OCW2							0x0020
#define PIC0_IMR							0x0021
#define PIC0_ICW2							0x0021
#define PIC0_ICW3							0x0021
#define PIC0_ICW4							0x0021
#define PIC1_ICW1							0x00a0
#define PIC1_OCW2							0x00a0
#define PIC1_IMR							0x00a1
#define PIC1_ICW2							0x00a1
#define PIC1_ICW3							0x00a1
#define PIC1_ICW4							0x00a1
void init_pic(void);

/* keyboard.c */
#define PORT_KEYDAT							0x0060
#define PORT_KEYCMD							0x0064
void int_handler_21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO *fifo, int data0);

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void int_handler_2c(int *esp);
void enable_mouse(struct FIFO *fifo, int data0, struct MOUSE_DEC *mdec);
int decode_mouse(struct MOUSE_DEC *mdec, unsigned char dat);

/* memory.c */
#define MEMMAN_FREES						4090	/* 约32KB */
#define MEMMAN_ADDR							0x003c0000
struct MEMFREE {	/* 剩余容量信息 */
	unsigned int addr, size;
};
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct MEMFREE free[MEMMAN_FREES];
};
unsigned int memory_test(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_getfree(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
void *kmalloc(struct MEMMAN *man, unsigned int size);
void kfree(struct MEMMAN *man, void *p);
void *krealloc(struct MEMMAN *man, void *p, unsigned int size);

/* sheet.c */
#define MAX_SHEETS							256
#define SHT_FLAG_CONSOLE					10
#define SHT_FLAG_APPWIN						20
#define SHT_FLAG_MENU						30
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	int mx, my, mbtn;
	struct SHTCTL *ctl;
	struct TASK *task;
};
struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_init(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_setheight(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_move(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* timer.c */
#define MAX_TIMER							512
struct TIMER {
	struct TIMER *next;
	unsigned int timeout;
	char flags, flags2;
	struct FIFO *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void int_handler_20(int *esp);
int timer_cancel(struct TIMER *timer);
void timer_cancelall(struct FIFO *fifo);

/* mtask.c */
#define MAX_TASKS							1000	/* 最大任务数量 */
#define TASK_GDT0							3		/* TSS从GDT的3号开始分配*/
#define MAX_TASKS_LV						100
#define MAX_TASKLEVELS						10
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
struct TASK {
	int sel, flags, status; /* sel代表GDT编号 */
	int level, priority;
	struct FIFO fifo;
	struct TSS32 tss;
	struct SEGMENT_DESCRIPTOR ldt[2];
	struct CONSOLE *cons;
	int ds_base, cons_stack;
	struct HFILE *fhandle;
	int *fat;
	char *cmdline;
};
struct TASKLEVEL {
	int running; /* 活动任务数量 */
	int now; /* 保存当前活动任务的变量 */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /* 当前活动层级 */
	char lv_change; /* 下次切换任务时是否需要改变层级 */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
extern struct TASKCTL *taskctl;
extern struct TIMER *tasktimer;
struct TASK *task_getnow(void);
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_sleep(struct TASK *task);
void task_switch(void);

/* window.c */
void vrma_make_window(unsigned char *buf, int xsize, int ysize, char *title, char act);
void sheet_draw_string(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void sheet_draw_mstring(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void vram_make_textbox(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void vram_make_wintitle(unsigned char *buf, int xsize, char *title, char actm);
void vram_change_wintitle(struct SHEET *sht, char act);

/* arg.c */
struct ARG {
	int argc;
	char *argv[16];
};
void string_split(char *src, const char *separator, char **dest, int *num);
void string_spacefilter(char *p);
int check_command(struct ARG arg, char *cmd);
int check_switch(char *ds, char *sw, unsigned int len);
int cmdline_split(char *cmdline, struct ARG *arg);

/* console.c */
#define CONSOLE_COLUMN						40
#define CONSOLE_LINE						16
#define CONSOLE_CHAR_WIDTH					6
#define CONSOLE_CHAR_HEIGHT					8
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
	struct TIMER *timer;
	int bc, fc, echo;
	char prompt[16];
	unsigned int pmtlen;
	char cmd_buf[64][40];
	int cmd_index, cmd_num;
};
struct HFILE {
	unsigned char *buf;
	int size;
	int pos;
};
void console_task(struct SHEET *sheet, int memtotal);
void console_putchar(struct CONSOLE *cons, int chr, char move);
void console_newline(struct CONSOLE *cons);
void console_puts(struct CONSOLE *cons, char *s);
void console_putsl(struct CONSOLE *cons, char *s, int l);
void console_beep(int tone, int ms);
int console_print(struct CONSOLE *cons, char *format, ...);
void console_run(struct ARG arg, char *cmdline, struct CONSOLE *cons, int *fat, int memtotal);

/* file.c */
#define EOF									-1
#define SEEK_SET							0
#define SEEK_CUR							1
#define SEEK_END							2
struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
void file_read_fat(int *fat, unsigned char *img);
void file_load(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
char *file_load_tek(int clustno, int *psize, int *fat);
int kfgetc(struct HFILE *file);
char *kfgets(struct HFILE *file, char *str, int n);
int kfseek(struct HFILE *file, int offset, int mode);

/* tek.c */
int tek_getsize(unsigned char *p);
int tek_decomp(unsigned char *p, char *q, int size);

/* main.c */
#define OS_VERSION							0x1000
#define EF_WALLPAPER						"BACK.JPG"
#define UNUSED(x)							(void) (x)
extern struct SHEET *key_win;
struct TASK *open_console_task(struct SHEET *sht, unsigned int memtotal);
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);
void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);

/* cmos.c */
#define CMOS_INDEX							0x70
#define CMOS_DATA							0x71
#define CMOS_CUR_SEC						0x00
#define CMOS_ALA_SEC						0x01
#define CMOS_CUR_MIN						0x02
#define CMOS_ALA_MIN						0x03
#define CMOS_CUR_HOUR						0x04
#define CMOS_ALA_HOUR						0x05
#define CMOS_WEEK_DAY						0x06
#define CMOS_MON_DAY						0x07
#define CMOS_CUR_MON						0x08
#define CMOS_CUR_YEAR						0x09
#define CMOS_DEV_TYPE						0x12
#define CMOS_CUR_CEN						0x32
#define BCD_HEX(n)							((n >> 4) * 10) + (n & 0x0f)
#define HEX_BCD(n)  						((n / 10) << 4) + (n % 10)
#define BCD_ASCII_FIRST(n)					(((n << 4) >> 4) + 0x30)
#define BCD_ASCII_SECOND(n)					((n << 4) + 0x30)
unsigned char cmos_read(unsigned char p);
void cmos_write(unsigned char p, unsigned char data);

/* jpeg.c */
struct DLL_STRPICENV {	/* 64KB */
	int work[64 * 1024 / 4];
};
struct RGB {
	unsigned char b, g, r, t;
};
int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, unsigned char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, 
	unsigned char *fp, int b_type, unsigned char *buf, int skip);

/* bmp.asm */
int bmp_info(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int bmp_decode(struct DLL_STRPICENV *env, int size, 
	unsigned char *fp, int b_type, unsigned char *buf, int skip);

/* error.c */
void black_srceen(int eip, char ecode, char *description);
int *int_handler_00(int *esp);
int *int_handler_01(int *esp);
int *int_handler_02(int *esp);
int *int_handler_03(int *esp);
int *int_handler_04(int *esp);
int *int_handler_05(int *esp);
int *int_handler_06(int *esp);
int *int_handler_07(int *esp);
int *int_handler_08(int *esp);
int *int_handler_09(int *esp);
int *int_handler_0a(int *esp);
int *int_handler_0b(int *esp);
int *int_handler_0c(int *esp);
int *int_handler_0d(int *esp);
int *int_handler_0e(int *esp);
int *int_handler_0f(int *esp);
int *int_handler_10(int *esp);
int *int_handler_11(int *esp);
int *int_handler_12(int *esp);

/* command.c */
#define	GET_BYTE0(n)						((n >> 0)  & 0xff)
#define	GET_BYTE1(n)						((n >> 8)  & 0xff)
#define	GET_BYTE2(n)						((n >> 16) & 0xff)
#define	GET_BYTE3(n)						((n >> 24) & 0xff)
void command_attrib(struct CONSOLE *cons, struct ARG arg);
void command_chtime(struct CONSOLE *cons, struct ARG arg);
void command_cls(struct CONSOLE *cons, struct ARG arg);
void command_color(struct CONSOLE *cons, struct ARG arg);
void command_crc32(struct CONSOLE *cons, int *fat, struct ARG arg);
void command_delete(struct CONSOLE *cons, struct ARG arg);
void command_disk(struct CONSOLE *cons, struct ARG arg);
void command_dir(struct CONSOLE *cons, struct ARG arg);
void command_echo(struct CONSOLE *cons, struct ARG arg);
void command_exit(struct CONSOLE *cons, int *fat);
void command_find(struct CONSOLE *cons, struct ARG arg);
void command_help(struct CONSOLE *cons, struct ARG arg);
void command_history(struct CONSOLE *cons, struct ARG arg);
void command_mem(struct CONSOLE *cons, struct ARG arg, int memtotal);
void command_more(struct CONSOLE *cons, int *fat, struct ARG arg);
void command_pause(struct CONSOLE *cons, struct ARG arg);
void command_prompt(struct CONSOLE *cons, struct ARG arg);
void command_rename(struct CONSOLE *cons, struct ARG arg);
void command_run(struct CONSOLE *cons, struct ARG arg, int memtotal);
void command_task(struct CONSOLE *cons, struct ARG arg);
void command_shutdown(struct CONSOLE *cons, struct ARG arg);
void command_tek(struct CONSOLE *cons, int *fat, struct ARG arg);
void command_time(struct CONSOLE *cons, struct ARG arg);
void command_type(struct CONSOLE *cons, int *fat, struct ARG arg);
void command_ver(struct CONSOLE *cons, struct ARG arg, int memtotal);
int console_run_app(struct CONSOLE *cons, int *fat, char *cmdline, int memtotal, struct ARG arg);

/* api.c */
int *mitapi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

/* taskbar.c */
#define MAX_TBITEM							20
struct TBITEM {
	int index;
	unsigned char *icon;
	struct SHEET *sht;
	int flag;
};
struct TBITEMCTL {
	struct SHEET *sht_back;
	int scrnx, scrny;
	struct TBITEM item[MAX_TBITEM];
	int num;
	int active;
};
extern struct TBITEMCTL *tbitemctl;
void vram_init_taskbar(char *vram, int x, int y);
void vram_init_start_button(char *vram, int x, int y, int sw);
void vram_init_system_tray(char *vram, int x, int y, int icon_num);
void tbitemctl_init(struct MEMMAN *memman, struct SHEET *sht_back, int scrnx, int scrny);
void tbitemctl_refresh(void);
int tbitemctl_add(unsigned char *icon, struct SHEET *sht);
int tbitemctl_remove(struct SHEET *sht);
void tbitemctl_switch(int id);
void tbitemctl_set(struct SHEET *sht);
int tbitemctl_getsheet(struct SHEET *sht);

/* time.c */
#define MINUTE								60
#define HOUR								(60 * MINUTE)
#define DAY									(24 * HOUR)
#define YEAR								(365 * DAY)
#define ISLEAP(year) ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 1000 == 0))
unsigned int get_hour(void);
unsigned int get_minute(void);
unsigned int get_second(void);
unsigned int get_day_of_month(void);
unsigned int get_day_of_week(void);
unsigned int get_month(void);
unsigned int get_year(void);
void set_hour(unsigned int hour_hex);
void set_minute(unsigned int min_hex);
void set_second(unsigned int sec_hex);
void set_day_of_month(unsigned int day_of_month);
void set_day_of_week(unsigned int day_of_week);
void set_month(unsigned int mon_hex);
void set_year(unsigned int year);
unsigned long kmktime(int year0, int mon0, int day, int hour, int min, int sec);

/* res.c */
extern unsigned int *resszie;
int res_info(int *info, int size, unsigned char *fp);
char *res_decode(int *info, int size, unsigned char *fp, char *id, int *rsize);

/* crc32.c */
unsigned int get_crc32(const unsigned char *buf, unsigned int size);

/* fat12.c */
#define TOTALSECTORS						2880
#define BYTESPERSECTOR						512
#define FIRSTOFFAT							1
#define FIRSTOFROOTDIR						19
#define FIRSTOFDATA							33
struct DIRENTRY
{
	char name[11];
	char attr;
	char reserve[10];
	unsigned short time;
	unsigned short date;
	unsigned short clustno;
	unsigned int size;
};
int fdimg_add_file(unsigned char *fpImg, unsigned char *fp, unsigned int fs, char *FileName, unsigned char attr);
int fdimg_delete_file(unsigned char *fpImg, char *FileName);
unsigned short fat_read_entry_value(unsigned char *fpImg, unsigned short entry);
void fat_write_entry_value(unsigned char *fpImg, unsigned short entry, unsigned short value);
void convert_to_filename(char *DirName, char *FileName);
void convert_to_dirName(char *FileName, char *DirName);
unsigned short fat_find_dir_empty_entry(unsigned char *fpImg);
unsigned short fat_find_fat_empty_entry(unsigned char *fpImg, unsigned short first);
unsigned short fat_get_rest_sectors(unsigned char *fpImg);
unsigned short fat_find_file(unsigned char *fpImg, char *FileName);
void fat_add_file(unsigned char *fpImg, unsigned char *fp, unsigned int fs, char *DirName, unsigned char attr);
void fat_delete_file(unsigned char *fpImg, unsigned short entry);

/* drive.c */
/* FD_COMMAND 值 */
#define FD_RECALIBRATE						0x07	/* 移动至磁道 0 */
#define FD_SEEK								0x0F	/* 寻道 */
#define FD_READ								0xE6	/* 读取 */
#define FD_WRITE							0xC5	/* 写入 */
#define FD_SENSEI							0x08	/* 检测中断状态 */
#define FD_SPECIFY							0x03	/* 指定 HUT 等 */
int fd_io(char command, unsigned int sect, unsigned char *buf);
