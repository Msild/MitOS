/*
	File: Source/include/stdbool.h
	Copyright: MLworkshop
	Author: Msild
*/

#ifndef STDBOOL_H

#define STDBOOL_H

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef NULL
	#define NULL	((void *) 0)
#endif

#define __bool_true_false_are_defined	1

#define bool		_Bool
#define false		0
#define true 		1

#ifdef __cplusplus
	}
#endif

#endif
