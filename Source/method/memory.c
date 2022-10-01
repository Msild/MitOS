/*
	File: Source/method/memory.c
	Copyright: MLworkshop
	Author: Msild
*/

#include "../MitOS.h"
#include <stddef.h>

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memory_test(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 确认CPU是386或以上 */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 如果CPU为286，AC自动由1变为0 */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* 禁止缓存 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* 允许缓存 */
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* 可用信息数目 */
	man->maxfrees = 0;		/* 用于观察可用状况：frees的最大值 */
	man->lostsize = 0;		/* 释放失败的内存总和 */
	man->losts = 0;			/* 释放失败的次数 */
	return;
}

unsigned int memman_getfree(struct MEMMAN *man)
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* 找到足够大的内存 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* 如果free[i]变成0，则删掉一条可用信息 */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* 结构体赋值 */
				}
			}
			return a;
		}
	}
	return 0; /* 没有可用空间 */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	/* 为便于归纳内存，将free[]按addr的顺序排列 */
	/* 先决定应该放在哪里 */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* 前面有可用内存 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 可以与前面的可用内存归纳到一起 */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 后面有可用内存 */
				if (addr + size == man->free[i].addr) {
					/* 可以与后面的可用内存归纳到一起  */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]删除*/
					/* free[i]变成0后归纳到前面去 */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 结构体赋值 */
					}
				}
			}
			return 0; /* 成功完成 */
		}
	}
	/* 不能与前面的可用内存归纳到一起  */
	if (i < man->frees) {
		/* 后面有可用内存 */
		if (addr + size == man->free[i].addr) {
			/* 可以与前面的可用内存归纳到一起  */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 成功完成 */
		}
	}
	/* 前后都不可与之归纳 */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]之后的往后移动，腾出可用内存 */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 更新最大值 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 成功完成 */
	}
	/* 不能往后移动*/
	man->losts++;
	man->lostsize += size;
	return -1; /* 失败 */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000; /* 向上舍入 */
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000; /* 向上舍入 */
	i = memman_free(man, addr, size);
	return i;
}

void *kmalloc(struct MEMMAN *man, unsigned int size)
{
	unsigned char *p = (unsigned char *) memman_alloc_4k(man, size + 16);
	if (NULL != p) {
		*((unsigned int *) p) = size;
		p += 16;
		return p;
	}
	return NULL;
}

void kfree(struct MEMMAN *man, void *p)
{
	unsigned char *q = p;
	unsigned int size;
	if (q != 0) {
		q -= 16;
		size = *((unsigned int *) q);
		memman_free_4k(man, (unsigned int) q, size + 16);
	}
	return;
}

void *krealloc(struct MEMMAN *man, void *p, unsigned int size)
{
	if (NULL == p) {
		return kmalloc(man, size);
	}
	kfree(man, p);
	return kmalloc(man, size);
}
