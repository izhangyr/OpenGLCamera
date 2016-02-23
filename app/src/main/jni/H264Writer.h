#ifndef __H264WRITER_H__
#define __H264WRITER_H__

#include <stdio.h>

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#include "./x264/build-android/master/arm/include/x264.h"
#ifdef __cplusplus
}
#endif

namespace KugouPlayer
{
	class H264Writer
	{
	public:
		H264Writer( const char* path, int width, int height );

		~H264Writer();

		int writeVideo( unsigned char* buffer );

	private:
		FILE* pFile;
		x264_t* handle;
		x264_param_t* param;  
		x264_picture_t* picture;  
		x264_nal_t* nal;
	};
}

#endif
