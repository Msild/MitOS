/*
	File: Source/MitOS/int/mouse.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

struct FIFO *mousefifo;
int mousedata0;

void int_handler_2c(int *esp)
{
	int data;
	io_out8(PIC1_OCW2, 0x64);	/* 通知PIC1 IRQ-12的受理已完成 */
	io_out8(PIC0_OCW2, 0x62);	/* 通知PIC0 IRQ-02的受理已完成 */
	data = io_in8(PORT_KEYDAT);
	fifo_push(mousefifo, data + mousedata0);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct FIFO *fifo, int data0, struct MOUSE_DEC *mdec)
{
	/* 将FIFO缓冲区的信息保存至全局变量 */
	mousefifo = fifo;
	mousedata0 = data0;
	/* 鼠标有效 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* 若顺利，ACK(0xfa)会被发送 */
	mdec->phase = 0; /* 等待鼠标0xfa阶段 */
	return;
}

int decode_mouse(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* 等待鼠标0xfa阶段 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 等待鼠标bit1阶段 */
		if ((dat & 0xc8) == 0x08) {
			/* 若bit1正确 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 等待鼠标bit2阶段 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 等待鼠标bit3阶段 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 鼠标y方向与画面符号相反 */
		return 1;
	}
	return -1; /* 通常不会到此 */
}
