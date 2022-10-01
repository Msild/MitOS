/*
	File: Source/include/errno.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef ERRNO_H

#define ERRNO_H

#ifdef __cplusplus
	extern "C" {
#endif

extern int errno;

#define	ENOENT		2	/* No such file or directory */
#define	ERANGE		34	/* Result too large (or too small) */

#ifdef __cplusplus
	}
#endif

#endif
