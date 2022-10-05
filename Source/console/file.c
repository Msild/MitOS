/*
	File: Source/console/file.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stddef.h>
#include <stdarg.h>

void file_read_fat(int *fat, unsigned char *img)
/* 将磁盘映像中的FAT解压缩 */
{
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void file_load(int clustno, int size, char *buf, int *fat, char *img)
{
	int i;
	for (;;) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
	return;
}

struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max)
{
	int i, j;
	char s[12];
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0 && name[i] != ' ' && name[i] != '-'; i++) {
		if (j >= 11) { return 0; /* 没有找到 */ }
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				/* 小写字母转为大写字母 */
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (finfo->name[0] == 0x00) {
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* 找到文件 */
		}
next:
		i++;
	}
	return 0; /* 没有找到 */
}

int kfgetc(struct HFILE *file)
{
	if (file->pos == file->size) {
		/* 文件尾部 */
		return EOF;
	}
	return file->buf[file->pos++];
}

char *kfgets(struct HFILE *file, char *str, int n)
{
	int c;
	char *s = str;
	while (--n > 0 && (c = kfgetc(file)) != EOF) {
		if ((*s++ = c) == '\n') {
			break;
		}
	}
	*s = '\0';
	return (c == EOF && s == str) ? NULL : s;
}

int kfseek(struct HFILE *file, int offset, int mode)
{
	if (SEEK_SET == mode) {
		file->pos = offset;
	} else if (SEEK_CUR == mode) {
		file->pos += offset;
	} else if (SEEK_END == mode) {
		file->pos = file->size + offset;
	}
	if (file->pos < 0) {
		file->pos = 0;
	}
	if (file->pos > file->size) {
		file->pos = file->size;
	}
	return 0;
}
