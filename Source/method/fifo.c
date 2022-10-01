/*
	File: Source/method/fifo.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

#define FLAGS_OVERRUN		0x0001

void fifo_init(struct FIFO *fifo, int size, int *buf, struct TASK *task)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 空 */
	fifo->flags = 0;
	fifo->p = 0; /* 写入位置 */
	fifo->q = 0; /* 读取位置 */
	fifo->task = task; /* 有数据写入时需要唤醒的任务 */
	return;
}

int fifo_push(struct FIFO *fifo, int data)
{
	if (fifo->free == 0) {
		/* 无空间则溢出 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* 任务处于休眠状态 */
			task_run(fifo->task, -1, 0); /* 唤醒任务 */
		}
	}
	return 0;
}

int fifo_pop(struct FIFO *fifo)
{
	int data;
	if (fifo->free == fifo->size) {
		/* 缓冲区为空则溢出 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo_status(struct FIFO *fifo)
{
	return fifo->size - fifo->free;
}
