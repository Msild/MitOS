/*
	File: Source/fs/fat12.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <string.h>

#define TSIZE		sizeof(struct DIRENTRY)

/*
	功能：
	  向软盘数据增加文件。
	参数：
	  fpImg - 软盘数据指针。
	  fp - 源文件指针。
	  fs - 源文件大小。
	  FileName = 文件名称。
	  attr  = 文件属性。
	返回：
	  0 - 成功。
	  1 - 可用空间不足。
	  2 - 无空表项。
*/
int fdimg_add_file(unsigned char *fpImg, unsigned char *fp, unsigned int fs, char *FileName, unsigned char attr)
{
	unsigned short iRootDirItem;
	struct DIRENTRY DirItem;
	char strDirName[12];

	/* 若已存在同名文件，取得其长度 */
	iRootDirItem = fat_find_file(fpImg, FileName);
	if (iRootDirItem < 0xff8) {
		memcpy(&DirItem, (unsigned char *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + iRootDirItem * TSIZE), TSIZE);
	} else {
		DirItem.size = 0;
	}

	/* 判断可用空间是否足够 */
	if ((DirItem.size + fat_get_rest_sectors(fpImg) * BYTESPERSECTOR) < fs) {
		/* 可用空间不足 */
		return 1;
	}

	/* 确认根目录区是否有空表项 */
	if (fat_find_dir_empty_entry(fpImg) > 0xff8) {
		/* 无空表项 */
		return 2;
	}

	/* 删除已存在的同名文件 */
	if (iRootDirItem < 0xff8) {
		fat_delete_file(fpImg, iRootDirItem);
	}

	convert_to_dirName(FileName, strDirName);
	fat_add_file(fpImg, fp, fs, strDirName, attr);
	return 0;
}

/*
	功能：
	  从软盘数据删除文件。
	参数：
	  fpImg - 软盘数据指针。
	  FileName = 文件名称。
	返回：
	  0 - 成功。
	  1 - 未找到文件。
*/
int fdimg_delete_file(unsigned char *fpImg, char *FileName)
{
	unsigned short iEntry;
	iEntry = fat_find_file(fpImg, FileName);
	if (iEntry < 0xff8) {
		fat_delete_file(fpImg, iEntry);
		return 0;
	}
	return 1;
}

/*
	功能：
	  读取 FAT 表项序号值。
	参数：
	  fpImg - 软盘数据指针。
	  entry - FAT表项序号。
	返回：
	  FAT表项的值。
*/
unsigned short fat_read_entry_value(unsigned char *fpImg, unsigned short entry)
{
	unsigned short *nTmp;
	nTmp = (unsigned short *) (fpImg + FIRSTOFFAT * BYTESPERSECTOR + entry * 3 / 2);
	return (entry % 2 == 0 ? *nTmp & 0xfff : *nTmp >> 4);
}

/*
	功能：
	  写入 FAT 表项序号值。
	参数：
	  fpImg - 软盘数据指针。
	  entry - FAT表项序号。
	  value - FAT表项的值。
*/
void fat_write_entry_value(unsigned char *fpImg, unsigned short entry, unsigned short value)
{
	unsigned short *nTmp;
	nTmp = (unsigned short *) (fpImg + FIRSTOFFAT * BYTESPERSECTOR + entry * 3 / 2);
	if (entry % 2 == 0) {
		value |= (value & 0xf000);
	} else {
		value = ((value << 4) | (value & 0xf));
	}
	*((unsigned short *) (fpImg + FIRSTOFFAT * BYTESPERSECTOR + entry * 3 / 2)) = *nTmp;
	*((unsigned short *) (fpImg + (FIRSTOFFAT + FIRSTOFROOTDIR) * BYTESPERSECTOR / 2 + entry * 3 / 2)) = *nTmp;
	return;
}

/*
	功能：
	  将8.3文件名转化为普通文件名。
	参数：
	  DirName - 待处理的文件名，char[8] + char[3] + 0，len=12。
	  FileName - 处理后的文件名，char[8] + '.' + char[3] + 0, len=13。
*/
void convert_to_filename(char *DirName, char *FileName)
{
	int i, j = 7, k = 10;

	while (DirName[j] == ' ') {
		/* 删除尾部空格 */
		j--;
	}
	for (i = 0; i <= j; i++) {
		FileName[i] = DirName[i];
	}

	while (DirName[k] == ' ') {
		/* 删除尾部空格 */
		k--;
	}
	if (k > 7) {
		/* 有扩展名 */
		FileName[++j] = '.';
		for (i = 8; i <= k; i++) {
			FileName[++j] = DirName[i];
		}
	}

	FileName[++j] = 0;
	for (i = 0; i < j; i++) {
		if ((unsigned int) (FileName[i] - 'A') < 26u) {
			FileName[i] += 'a' - 'A';
		}
	}
	return;
}

/*
	功能：
	  将普通文件名转化为8.3文件名。
	参数：
	  FileName - 处理后的文件名，char[8] + '.' + char[3] + 0, len=13。
	  DirName - 待处理的文件名，char[8] + char[3] + 0，len=12。
*/
void convert_to_dirName(char *FileName, char *DirName)
{
	int i, j = -1;
	char *pExtension;

	pExtension = strrchr(FileName, '.');

	if (NULL == pExtension) {
		/* 没有扩展名 */
		/* 文件名最长8字节 */
		for (i = 0; i < 8; i++) {
			if (FileName[i] == 0) {
				break;
			}
			DirName[i] = FileName[i];
		}
		while (i < 11) {
			DirName[i++] = ' ';
		}
	} else {
		/* 有扩展名 */
		/* '.'之前为文件名，最长8字节 */
		j = pExtension - FileName;
		for (i = 0; i < j && i < 8; i++) {
			DirName[i] = FileName[i];
		}
		while (i < 8) {
			DirName[i++] = ' ';
		}
		j++;
		/* '.'之后为扩展名，最长3字节 */
		for (i = 8; i < 11; i++, j++) {
			if (FileName[j] == 0) {
				break;
			}
			DirName[i] = FileName[j];
		}
		while (i < 11) {
			DirName[i++] = ' ';
		}
	}

	for (i = 0; i < 11; i++) {
		if ((unsigned int) (DirName[i] - 'a') < 26u) {
			DirName[i] += 'A' - 'a';
		}
	}
	DirName[11] = 0;
	return;
}

/*
	功能：
	  在目录表中寻找空值表项。
	参数：
	  fpImg - 软盘数据指针。
	返回：
	  第一个空值表项序号。
	  0xfff 未找到。
*/
unsigned short fat_find_dir_empty_entry(unsigned char *fpImg)
{
	struct DIRENTRY *Diritem;
    unsigned short i, max;
    unsigned char ch;
	max = (FIRSTOFDATA - FIRSTOFROOTDIR) * BYTESPERSECTOR / TSIZE;
	for (i = 0; i < max; i++) {
		Diritem = (struct DIRENTRY *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + i * TSIZE);
		ch = Diritem->name[0];
		if ((ch == 0) /* 空 */ || (ch == 0xe5) /* 已删除 */) {
			return i;
		}
	}
    return 0xfff;
}

/*
	功能：
	  在FAT表中寻找空值表项。
	参数：
	  fpImg - 软盘数据指针。
	  first - 起始表项序号。
	返回：
	  first及其之后的第一个空值表项序号。
	  0xfff 未找到。
*/
unsigned short fat_find_fat_empty_entry(unsigned char *fpImg, unsigned short first)
{
	unsigned short i, max;
	max = (FIRSTOFROOTDIR - FIRSTOFFAT) * BYTESPERSECTOR / 3;
	for (i = first; i < max; i++) {
		if (fat_read_entry_value(fpImg, i) == 0) {
			return i;
		}
	}
	return 0xfff;
}

/*
	功能：
	  统计数据区空白扇区数。
	参数：
	  fpImg - 软盘数据指针。
	返回：
	  数据区的空白扇区数。
*/
unsigned short fat_get_rest_sectors(unsigned char *fpImg)
{
	unsigned short iFirstEntry = 2, iNext, nNum = 0;
	while ((iNext = fat_find_fat_empty_entry(fpImg, iFirstEntry)) > 0 && (iNext < 0xfff)) {
		iFirstEntry = iNext + 1;
		nNum++;
	}
	return nNum;
}

/*
	功能：
	  在根目录区中寻找文件。
    参数：
	  fpImg - 软盘数据指针。
	  FileName - 文件名。
    返回：
	  该文件的表项序号。
	  0xfff - 未找到。
*/
unsigned short fat_find_file(unsigned char *fpImg, char *FileName)
{
	struct DIRENTRY *DirItem;
	char FileItemName[12];
	unsigned short i, max;
	convert_to_dirName(FileName, FileItemName);
	max = (FIRSTOFDATA - FIRSTOFROOTDIR) * BYTESPERSECTOR / TSIZE;
	for (i = 0; i < max; i++) {
		DirItem = (struct DIRENTRY *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + i * TSIZE);
		if (strncmp(DirItem->name, FileItemName, 11) == 0) {
			return i;
		}
	}
	return 0xfff;
}

unsigned char *FatLoadFile(unsigned char *fpImg, unsigned short entry)
{
	unsigned short nFatValue, iFatNext;
	struct DIRENTRY *Diritem;
	Diritem = (struct DIRENTRY *) (fpImg +  FIRSTOFROOTDIR * BYTESPERSECTOR + entry * TSIZE);
	iFatNext = Diritem->clustno;
	do {
		nFatValue = fat_read_entry_value(fpImg, iFatNext);
		
		iFatNext = nFatValue;
	} while(iFatNext < 0xff8);
	*((unsigned char *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + entry * TSIZE)) = 0xe5;
	return;
}

/*
	功能：
	  向软盘数据添加文件.
	参数：
	  fpImg - 软盘数据指针。
	  fp - 源文件指针。
	  fs - 源文件大小。
	  DirName - 8.3文件名。
	  attr - 文件属性。
*/
void fat_add_file(unsigned char *fpImg, unsigned char *fp, unsigned int fs, char *DirName, unsigned char attr)
{
	unsigned long nFileLen = fs;
	unsigned char ucData[BYTESPERSECTOR];
	unsigned short arrySectorList[TOTALSECTORS], i;
	unsigned short nNeedSectors, iLast, nRemainderBytes, iRootDirItem;
	struct DIRENTRY DirItem;

	/* 计算所需扇区数 */
	nRemainderBytes = nFileLen % BYTESPERSECTOR;
	nNeedSectors = nFileLen / BYTESPERSECTOR + (nRemainderBytes > 0 ? 1 : 0);
	iLast = nNeedSectors - 1;

	/* 寻找FAT空表项并建立FAT表链 */
	arrySectorList[0] = fat_find_fat_empty_entry(fpImg, 2);
	for (i = 1; i < nNeedSectors; i++) {
		arrySectorList[i] = fat_find_fat_empty_entry(fpImg, arrySectorList[i - 1] + 1);
		fat_write_entry_value(fpImg, arrySectorList[i - 1], arrySectorList[i]);
	}
	fat_write_entry_value(fpImg, arrySectorList[iLast], 0xfff);

	/* 根目录区表项记录 */
	iRootDirItem = fat_find_dir_empty_entry(fpImg);
	strncpy(DirItem.name, DirName, 11);
	DirItem.attr = attr;
	/* DirItem.time = DirItem.date = 0; */
	DirItem.clustno = arrySectorList[0];
	DirItem.size = nFileLen;
	memcpy((unsigned char *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + iRootDirItem * TSIZE), &DirItem, TSIZE);


	memcpy(fpImg + (arrySectorList[0] + FIRSTOFDATA - 2) * BYTESPERSECTOR, fp, 1278);
	// /* 复制数据至相应扇区 */
	// for (i = 0; i < iLast; i++) {
	// 	/* 最后一个扇区单独处理 */
	// 	memcpy((unsigned char *) (fpImg + (arrySectorList[i] + FIRSTOFDATA - 2) * BYTESPERSECTOR), fp, BYTESPERSECTOR);
	// 	fpImg += BYTESPERSECTOR;
	// 	fp += BYTESPERSECTOR;
	// }
	// if (nRemainderBytes > 0) {
	// 	/* 最后一个扇区不满512字节 */
	// 	memcpy(ucData, fp, nRemainderBytes);
	// 	for (i = nRemainderBytes; i < BYTESPERSECTOR; i++) {
	// 		ucData[i] = 0x00;
	// 	}
	// } else {
	// 	/* 最后一个扇区满512字节 */
	// 	memcpy(ucData, fp, BYTESPERSECTOR);
	// }
	// memcpy(fpImg + (arrySectorList[iLast] + FIRSTOFDATA - 2) * BYTESPERSECTOR, ucData, BYTESPERSECTOR);
	return;
}

/*
	功能：
	  从软盘数据中删除文件。
	参数：
	  fpImg - 软盘数据指针。
	  entry - 目标文件FAT表项号。
*/
void fat_delete_file(unsigned char *fpImg, unsigned short entry)
{
	unsigned short nFatValue, iFatNext;
	struct DIRENTRY *Diritem;
	Diritem = (struct DIRENTRY *) (fpImg +  FIRSTOFROOTDIR * BYTESPERSECTOR + entry * TSIZE);
	iFatNext = Diritem->clustno;
	do {
		nFatValue = fat_read_entry_value(fpImg, iFatNext);
		fat_write_entry_value(fpImg, iFatNext, 0);
		iFatNext = nFatValue;
	} while(iFatNext < 0xff8);
	*((unsigned char *) (fpImg + FIRSTOFROOTDIR * BYTESPERSECTOR + entry * TSIZE)) = 0xe5;
	return;
}
