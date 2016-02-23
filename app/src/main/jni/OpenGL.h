#ifndef __OPENGL_H__
#define __OPENGL_H__

#include <pthread.h>
#include <queue>
#include "OpenGLRender.h"
#include "ImageFilter.h"
#include "H264Writer.h"

#if 0
#ifdef __cplusplus
extern "C"
{
#endif
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libavutil\avutil.h"
#ifdef __cplusplus
}
#endif
#endif

//using namespace std;
namespace KugouPlayer
{
	class OpenGL
	{
	public:
		OpenGL();
		~OpenGL();
		void surfaceChange( int width, int height );

		void setFilterType( int type, bool enablegaussfilter );

		void setRotation( int degrees, int flipHorizontal, int flipVertical );

		unsigned char* render( unsigned char* buffer, int widthTexture, int heightTexture );

		void startRecord( const char* path, int width, int height );

		void stopRecord();

	private:
		//inline unsigned char clamp0255(unsigned short x);
		//void alpha_blend(unsigned char* pDst, unsigned char* pSrc, int width, int height, float alpha);
		//void alpha_blend(unsigned char* pDst, int width, int height);
		ImageFilter mImageFilter;//图像滤镜处理器
		pthread_mutex_t mutex;//线程锁
		OpenGLRender* mOpenGLRender;//用于将滤镜处理后的图像渲染显示出来
		H264Writer* mWriter;//H264编码器的结构体体指针
		unsigned char* mRGBBuffer;//存放的RGB数据
		unsigned char* mBGRBuf;
		unsigned char* YUV420Buf;
		int mImageWidth;//图像宽
		int mImageHeight;//图像高
		//queue<int> lenArray;
		int len;
		//const char* filePath;
		//FILE* file;
	};
}

#endif
