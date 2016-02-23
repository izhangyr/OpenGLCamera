#include <stdio.h>
#include <time.h>

#include "OpenGL.h"
#include "ColorSpace.h"

namespace KugouPlayer
{
//OpenGL类中的构造函数中初始化其成员变量
	OpenGL::OpenGL()
		: mOpenGLRender( NULL ),
		  mWriter( NULL ),
		  mRGBBuffer( NULL ),
		  mBGRBuf( NULL ),
		  YUV420Buf(NULL),
		  mImageWidth( 0 ),
		  mImageHeight( 0 )
		  //len(0),
		  //filePath(NULL),
		  //file(NULL)
	{
		pthread_mutex_init( &mutex, NULL );//初始化线程锁

		mOpenGLRender = new OpenGLRender();//为OpenGLRender类分配一块动态的内存

	//	avcodec_init();
	//	av_register_all();
#if 0
		bool flag = false;

		const char* matterPath = "/storage/emulated/0/matte33.bmp";

		flag  = Bitmap::Load(bitmapsource, matterPath);
		if (false == flag)
		{
			LOGE("Load bmp failed\r\n");
		}
		else
		{
			LOGE("Load bmp success33\r\n");
		}
#endif
		//bitmapdest.Alloc(width, height, 32);
		//BGR2RGBA(bitmapdest.bits, bitmapsource.bits, width, height);
	}

	OpenGL::~OpenGL()
	{
		if( mRGBBuffer != NULL )
		{
			delete []mRGBBuffer;
		}
	}
	/*
	 * 切换摄像界面
	 */
	void OpenGL::surfaceChange( int width, int height )
	{
		if( mOpenGLRender != NULL )
		{
			mOpenGLRender->setDisplayArea( 0, 0, width, height );
		}
	}

	/*
	 *改变滤镜类型,包括有是否使用高斯滤波
	 */
	void OpenGL::setFilterType( int type, bool enablegaussfilter )
	{
		mImageFilter.SetFilterType( type );//设置滤镜类型

		if( mOpenGLRender != NULL )
		{
			mOpenGLRender->enableGaussFilter( enablegaussfilter );
		}
	}

	void OpenGL::setRotation( int degrees, int flipHorizontal, int flipVertical )
	{
		if( mOpenGLRender != NULL )
		{
			mOpenGLRender->setRotation( degrees, flipHorizontal, flipVertical );
		}
	}

#if 0
	inline unsigned char OpenGL::clamp0255(unsigned short x)
	{
		if(x<0)
			return 0;
		else if (x>255)
			return 255;
		return x;
	}
#endif

#if 0
	void OpenGL::alpha_blend(unsigned char* pDst, int width, int height)
	{
		int lineBytes = WIDTHBYTES(32*width);

		unsigned char* p0 = NULL;
//		unsigned char* p1 = NULL;

#if 0
		for (int y=0; y<height; y++)
		{
			p0 = pDst + y * lineBytes;
			p1 = pSrc + y * lineBytes;

			for (int x =0; x<width; x++)
			{
				*(p0 + 0) = clamp0255(*(p0 + 0) * (1.0f - alpha) + *(p1 +0) * alpha);
				*(p0 + 1) = clamp0255(*(p0 + 1) * (1.0f - alpha) + *(p1 +1) * alpha);
				*(p0 + 2) = clamp0255(*(p0 + 2) * (1.0f - alpha) + *(p1 +2) * alpha);
				*(p0 + 3) = clamp0255(*(p0 + 3) * (1.0f - alpha) + *(p1 +3) * alpha);

				p0 += 4;
				p1 += 4;
			}
		}
#endif

#if 0
		for (int y=0; y<height; y++)
		{
			p0 = pDst + y * lineBytes;

			for (int x =0; x<width; x++)
			{
				*(p0 + 0) = clamp0255((*(p0 + 0)+100) * (1.0f - alpha));
				*(p0 + 1) = clamp0255((*(p0 + 1)+100) * (1.0f - alpha));
				*(p0 + 2) = clamp0255((*(p0 + 2)+100) * (1.0f - alpha));
				*(p0 + 3) = clamp0255((*(p0 + 3)+100) * (1.0f - alpha));

				p0 += 4;
			}
		}
#endif
		for (int y = 0; y < height; y++)
		{
			p0 = pDst + y * lineBytes;

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = clamp0255(((*(p0 + 0))+50));
				*(p0 + 1) = clamp0255(((*(p0 + 1))+50));
				*(p0 + 2) = clamp0255(((*(p0 + 2))+50));
				//*(p0 + 3) = clamp0255((*(p0 + 3)+100));

				p0 += 4;
			}
		}
	}
#endif
	unsigned char* OpenGL::render( unsigned char* buffer, int widthTexture, int heightTexture )
	{
		//clock_t start;
		//clock_t end;
		int lineBytes = 0;
		int imageSize = 0;

		if( buffer != NULL )
		{
			// change yuv to rgb
			/*
			 * 若新渲染显示的图像尺寸比之前的要大,则需要重新分配一个图像尺寸大小的buffer.
			 * 否则直接使用之前的图像buffer.
			 */
			//if( ( widthTexture * heightTexture ) > ( mImageWidth * mImageHeight ) )
			if ((widthTexture > 0) && (heightTexture > 0) && (widthTexture != mImageWidth) && (heightTexture != mImageHeight))
			{
				mImageWidth = widthTexture;
				mImageHeight = heightTexture;

				if( mRGBBuffer != NULL )
				{
					delete []mRGBBuffer;
				}
				mRGBBuffer = new unsigned char[ ( mImageWidth * mImageHeight ) * 4 ];
#if 0
				if (mBGRBuf != NULL)
				{
					delete mBGRBuf;
				}
				lineBytes = WIDTHBYTES(mImageWidth * 24);
				imageSize = lineBytes * mImageHeight;
				mBGRBuf = new unsigned char[imageSize];
				if (NULL == mBGRBuf)
				{
					return NULL;
				}

				if (YUV420Buf != NULL)
				{
					delete YUV420Buf;
				}
				YUV420Buf = new unsigned char[mImageWidth*mImageHeight*3/2];
				if (NULL == YUV420Buf)
				{
					return NULL;
				}
#endif
			}

			memset(mRGBBuffer, 0, (( mImageWidth * mImageHeight ) * 4));

			//memset(mBGRBuf, 0, imageSize);
			//memset(YUV420Buf, 0, mImageWidth*mImageHeight*3/2);
			//将外部传进来的YUV数据转换为RGB,并存放在mRGBBuffer中
			if( mRGBBuffer != NULL )
			{
				//ColorSpace::modify_yuv420splum(buffer, mImageWidth, mImageHeight, 100);

				//ColorSpace::YUV420SP2RGBA( mRGBBuffer, buffer, widthTexture, heightTexture );
				ColorSpace::yuv420sp_to_bgra(buffer, mImageWidth, mImageHeight, mRGBBuffer);

				//ColorSpace::BRGA2BGR(mBGRBuf, mRGBBuffer, mImageWidth, mImageHeight);
				//ColorSpace::bgr24ToYUV420(mBGRBuf, YUV420Buf, mImageWidth, mImageHeight);

				//start = clock();
				//alpha_blend(mRGBBuffer,bitmapsource.bits,widthTexture,heightTexture, 0.3f);
				//alpha_blend(mRGBBuffer, mImageWidth, mImageHeight);
				//end = clock();
				//LOGE("the time differ:%d\r\n", (end-start));

				// color process  将之前选择的滤镜类型并经过像素数值计算后的滤镜数据放在mRGBBuffer中
				mImageFilter.Process( mRGBBuffer, widthTexture, heightTexture );
			}

			pthread_mutex_lock( &mutex );
			if( mWriter != NULL )
			{
				len = mWriter->writeVideo( buffer );//对传进来的YUV数据开始进行h264编码并写文件
				//lenArray.push(len);
			}
			pthread_mutex_unlock( &mutex );
		}

		// shader process 正式进行渲染操作
		/*
		 *若外部没有新的图像YUV数据传进来，而之前转换好的RGBbuffer还存在,则也是需要渲染显示的。
		 当然有新的YUV图像数据传进来后,首先上面是将其转换为BGRA数据后,然后才在下面进行渲染显示
		 */
		if( ( mRGBBuffer != NULL ) && ( mOpenGLRender != NULL ) )
		{
			mOpenGLRender->render( mRGBBuffer, mImageWidth, mImageHeight );
		}
	}

	void OpenGL::startRecord( const char* path, int width, int height )
	{
		H264Writer* writer = new H264Writer( path, width, height );

		//filePath = path;

		pthread_mutex_lock( &mutex );
		mWriter = writer;
		pthread_mutex_unlock( &mutex );
	}

	void OpenGL::stopRecord()
	{
		pthread_mutex_lock( &mutex );
		if( mWriter != NULL )
		{
			delete mWriter;
			mWriter = NULL;
		}
#if 0
		if (bitmapsource.bits != NULL)
		{
			delete []bitmapsource.bits;
			bitmapsource.bits = 0;
		}
#endif
		pthread_mutex_unlock( &mutex );

#if 0
		pthread_mutex_lock( &mutex );
		file = fopen( filePath, "rb" );
		if (NULL == file)
		{
			LOGE("open filePath:%s failed\r\n", filePath);
		}

		while (!lenArray.empty())
		{
			int v = lenArray.front();
			lenArray.pop();


		}


		fclose(file);
		pthread_mutex_unlock( &mutex );
#endif
	}
}
