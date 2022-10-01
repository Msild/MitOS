/*
	File: Source/int/error.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stdio.h>

void black_srceen(int eip, char ecode, char *description)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[128];
	varm_fill_rectangle(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 
			 binfo->scrnx - 1, binfo->scrny - 1);
	varm_draw_mstring(binfo->vram, binfo->scrnx,  7,  7, COL8_AAAAAA, 
		"A problem has been detected and MitOS has been "
		"shut down to prevent damage to your computer.");
	varm_draw_mstring(binfo->vram, binfo->scrnx,  7, 23, COL8_AAAAAA, 
		"If this is the first time you've seen this "
		"stop error screen, restart your computer.");
	varm_draw_mstring(binfo->vram, binfo->scrnx,  7, 39, COL8_AAAAAA, 
		"Techical information:");
	varm_draw_mstring(binfo->vram, binfo->scrnx,  7, 55, COL8_AAAAAA, 
		"Description: ");
	varm_draw_mstring(binfo->vram, binfo->scrnx, 84, 55, COL8_AAAAAA, 
		description);
	sprintf(s, "ERRORCODE: %02X    EIP: %08X", ecode, eip);
	varm_draw_mstring(binfo->vram, binfo->scrnx,  7, 71, COL8_AAAAAA, s);
	io_cli();
	for (;;) {
		io_hlt();
	}
}

int *int_handler_00(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x00, "Division By Zero.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_01(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x01, "Debug INT.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_02(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x02, "Non Maskable Interrupt.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_03(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x03, "Break Point.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_04(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x04, "Stack Overflow.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_05(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x05, "Array Subscript Out Of Bounds.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_06(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x06, "Undefined Opcode.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_07(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x07, "Device Not Available.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_08(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x08, "Double Faults.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_09(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x09, "Different Segments.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0a(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0a, "Invalid TSS.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0b(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0b, "Segment Does Not Exist.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0c(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0c, "Stack Exception.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0d(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0d, "General Protected Exception.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0e(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0e, "Page Fault.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_0f(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x0f, "Inter Placeholder.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_10(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x10, "Floating Point Processing Fault.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_11(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x11, "Alignment Check.");
	return &(task->tss.esp0);	/* 异常结束 */
}

int *int_handler_12(int *esp)
{
	struct TASK *task = task_getnow();
	black_srceen(esp[11], 0x12, "Machine Check.");
	return &(task->tss.esp0);	/* 异常结束 */
}

/*
	错误代码
	0x00 - 0x1f		INT异常
	0x20			内存不足 
*/ 
