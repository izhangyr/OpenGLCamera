#ifndef __COMMON_H__
#define __COMMON_H__

//#define NEW_FILTER

#include <android/log.h>
#define LOG_TAG	"KugouPlayer"

#define LOGE( ... ) __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )

#ifndef	WIDTHBYTES
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)
#endif
#endif
