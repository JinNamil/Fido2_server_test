#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#include <cstdio>

#ifdef _DEBUG
	#define __DEBUG__
#endif // _DEBUG

#ifdef __DEBUG__
    #define DBG_Log(x,...)    printf("[LOG %s:%d] " x "\n(in %s)\n\n", __FUNCTION__, __LINE__, ##__VA_ARGS__, __FILE__)
#else
    #define DBG_Log(x,...)
#endif

#endif // __DEBUG_H_INCLUDED__
