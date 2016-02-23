#ifndef __CORE_DEFINE_H__
#define __CORE_DEFINE_H__

// 不定长整形
typedef char int8;
typedef short int16;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned char byte;
// 定长整型
#if defined(_WIN64) || defined(__x86_64__)
	typedef int int32;
	typedef unsigned int uint32;
	typedef long long int64;
	typedef unsigned long long uint64;
	typedef long long intx;
	typedef unsigned long long uintx;
#else
	typedef int int32;
	typedef unsigned int uint32;
	typedef long long int64;
	typedef unsigned long long uint64;
	typedef int intx;
	typedef unsigned int uintx;
#endif

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)<(b)?(b):(a))
#endif

#ifndef	WIDTHBYTES
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)
#endif

#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH 401
#endif

#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT 440
#endif

#endif