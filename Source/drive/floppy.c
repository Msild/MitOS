/*
	File: Source/drive/floppy.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stddef.h>

#define FD_STATUS			0x3f4
#define FD_DATA				0x3f5
#define FD_DOR				0x3f2	/* 数字输出寄存器 */
#define FD_DIR				0x3f7	/* 数字输入寄存器（只读） */
#define FD_DCR				0x3f7	/* 磁盘控制寄存器（只写） */

#define STATUS_BUSYMASK		0x0F	/* 设备忙掩码 */
#define STATUS_BUSY			0x10	/* FDC 忙 */
#define STATUS_DMA			0x20	/* 0- DMA 模式 */
#define STATUS_DIR			0x40	/* 0- cpu->fdc */
#define STATUS_READY		0x80	/* 数据注册就绪 */

#define DMA_READ 0x46
#define DMA_WRITE 0x4A

struct floppy_struct {
	unsigned int size, sect, head, track, stretch;
	unsigned char gap, rate, spec1;
} floppy_struct;

static struct floppy_struct floppy_type = {
	2880, 18, 2, 80, 0, 0x1B, 0x00, 0xCF
}; /* 1.44MB 软盘 */

static unsigned int current_dev = 0;

/*
	@brief 向软驱控制器输出一字节的命令或参数。
	@param byte 命令或参数。
	@return 执行结果 [0 - 成功] [1 - 失败] 。
*/
static int output_byte(char byte)
{
	int counter;
	unsigned char status;
	for (counter = 0; counter < 10000; counter++) {
		status = io_in8(FD_STATUS) & (STATUS_READY | STATUS_DIR);
		if (status == STATUS_READY) {
			io_out8(FD_DATA, byte);
			return 0;
		}
	}
	/* 发送错误 */
	return 1;
}

/*
	@brief 复位软盘控制器。
*/
void fd_reset()
{
	int i;
	io_cli();
	io_out8(FD_DOR, FD_SENSEI); /* 重启 */
	for (i = 0; i < 100; i++) {
		/* asm("nop"); */ /* 延时以保证重启完成 */
	}
	io_out8(FD_DOR, 0xc); /* 选择 DMA 模式软驱A */
	io_sti();
}

/*
	@brief 设定驱动器参数。
*/
void fd_setarg()
{
	output_byte(FD_SPECIFY);
	output_byte(0xcf); /* 马达步进速度、磁头卸载时间 32ms */
	output_byte(6);
}

/*
	@brief 设置 DMA。
	@param buf 缓冲区。
	@param command 命令。
*/
static void set_dma(unsigned char *buf, unsigned char cmd)
{
	long addr = (long) buf;
	io_cli();
	/* DMA 2 掩码 */
	io_out8(10, 4 | 2);
	/* 输出命令字节 */
	io_out8(12, (char) ((cmd == FD_READ) ? DMA_READ : DMA_WRITE));
	io_out8(11, (char) ((cmd == FD_READ) ? DMA_READ : DMA_WRITE));
	/* addr 低 8 位 */
	io_out8(4, addr);
	addr >>= 8;
	/* addr 的 8-15 位 */
	io_out8(4, addr);
	addr >>= 8;
	/* addr 的第 16-19 位 */
	io_out8(0x81, addr);
	/* count-1 的低 8 位 (1024-1=0x3ff) */
	io_out8(5, 0xff);
	/* count-1 的高 8 位 */
	io_out8(5, 3);
	/* 激活 DMA 2 */
	io_out8(10, 0 | 2);
	io_sti();
}

/*
	@brief 软盘读写。
	@param command 命令 [FD_READ - 读取] [FD_WRITE - 写入] 。
	@param sect 扇区号。
	@param buf 字节流缓冲区。
	@return 执行结果 [0 - 成功] [1 - 失败] 。
*/
int fd_io(char command, unsigned int sect, unsigned char *buf)
{
	unsigned int head, track, block, sector, seek_track;
	if (NULL == buf) {
		return 1;
	}

	if (sect >= (floppy_type.head * floppy_type.track * floppy_type.sect)) {
		return 2;
	}

	/* 计算参数 */
	sector = sect % floppy_type.sect + 1;
	block = sect / floppy_type.sect;
	track = block / floppy_type.head;
	head = block % floppy_type.head;
	seek_track = track << floppy_type.stretch;

	/* 软盘重新校正 */
	output_byte(FD_RECALIBRATE);
	output_byte(current_dev);

	/* 寻找磁道 */
	output_byte(FD_SEEK);
	output_byte(current_dev);
	output_byte(seek_track);

	/* 设置 DMA，准备传送数据 */
	set_dma(buf, command);

	/* 发送扇区命令 */
	output_byte(command);			/* 命令 */
	output_byte(current_dev);		/* 驱动器号 */
	output_byte(track);				/* 磁道 */
	output_byte(head);				/* 磁头 */
	output_byte(sector);			/* 开始扇区 */
	output_byte(2);					/* 扇区大小位 512 */
	output_byte(floppy_type.sect);	/* 最大扇区 */
	output_byte(floppy_type.gap);	/* 扇区差 */
	output_byte(0xff);				/* 扇区大小 */
	return 0;
}
