/*
	File: Source/method/dsctbl.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
	int i;

	/* GDT初始化 */
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, ADR_GDT);

	/* IDT初始化 */
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);

	/* IDT设置 */
	set_gatedesc(idt + 0x00, (int) asm_int_handler_00, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_01, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_02, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_03, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_04, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_05, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_06, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_07, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_08, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_09, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_0a, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_0b, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x0c, (int) asm_int_handler_0c, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x0d, (int) asm_int_handler_0d, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_0e, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_0f, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_10, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_11, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x00, (int) asm_int_handler_12, 2 * 8, AR_INTGATE32);

	set_gatedesc(idt + 0x20, (int) asm_int_handler_20, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x21, (int) asm_int_handler_21, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int) asm_int_handler_2c, 2 * 8, AR_INTGATE32);

	set_gatedesc(idt + 0x40, (int) asm_mit_api,      2 * 8, AR_INTGATE32 + 0x60);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, 
				  unsigned int limit, int base, int ar)
{
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
