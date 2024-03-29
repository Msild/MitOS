/*
	File: Source/include/stdarg.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef STDARG_H

#define STDARG_H

#ifdef __cplusplus
	extern "C" {
#endif

#define va_start(v,l)	__builtin_stdarg_start((v),l)
#define va_end			__builtin_va_end
#define va_arg			__builtin_va_arg
#define va_copy(d,s)	__builtin_va_copy((d),(s))
#define	va_list			__builtin_va_list

#ifdef __cplusplus
	}
#endif

#endif
