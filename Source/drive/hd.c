/*
	File: Source/drive/fd.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"

#define MAX_ERRORS			7	/* 操作扇区时允许的最大出错次数 */
#define MAX_HD				2	/* 支持的最大硬盘数 */

/* 重新校正处理函数 */
static void recal_intr(void);

static int recalibrate = 1;		/* 重新校正标志 */
static int reset = 1;			/* 复位标志 */

/* 硬盘参数及类型定义 */
struct hd_i_struct {
	int head, sect, cyl, wpcom, lzone, ctl;
};
struct hd_i_struct hd_info[] = { {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0} };
static int NR_HD = 0;

/* 定义硬盘分区结构 */
static struct hd_struct {
	long start_sect;			/* 分区在硬盘中的物理扇区 */
	long nr_sects;				/* 分区中的扇区总数 */
} hd[5 * MAX_HD] = { {0, 0} };

static void port_read(unsigned int port, unsigned char *buf, unsigned int nr)
{
	// asm("cld\n\t"
	// 	"cmp %%ecx, 0\n\t"
	// 	"jle .exit"
	// 	"insw"
	// 	"dec %%ecx"
	// 	".exit:"
	// 	:
	// 	: "d"(port), "D"(buf), "c"(nr)
	// );
}

#define port_write(port,buf,nr) \
asm("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr):"cx","si")
