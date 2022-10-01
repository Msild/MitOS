/*
	File: Source/include/setjmp.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef SETJMP_H

#define SETJMP_H

#ifdef __cplusplus
	extern "C" {
#endif

typedef int jmp_buf[3]; /* EBP, EIP, ESP */

#define setjmp(env)			__builtin_setjmp(env)
#define longjmp(env, val)	__builtin_longjmp(env, val)

#ifdef __cplusplus
	}
#endif

#endif
