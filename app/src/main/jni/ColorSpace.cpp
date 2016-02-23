#include "ColorSpace.h"

namespace KugouPlayer
{
	void ColorSpace::BRGA2BGR(unsigned char * des, unsigned char * src, int width, int height)
	{
		unsigned char * p0;
		unsigned char * p1;
		int lineBytesSrc =  WIDTHBYTES(32 * width);
		int lineBytesDst = WIDTHBYTES(24 * width);

		for (int y = 0; y < height; y++)
		{
			p0 = des + y * lineBytesDst;
			p1 = src + y * lineBytesSrc;

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 0);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 2);

				p0 += 3;
				p1 += 4;
			}
		}
	}

	void ColorSpace::ff_rgb24toyv12_c( unsigned char *src, unsigned char *ydst, unsigned char *udst, unsigned char *vdst, int width, int height, int lumStride, int chromStride, int srcStride, const int rgb2yuv[])
	{
	    int ry = rgb2yuv[RY_IDX], gy = rgb2yuv[GY_IDX], by = rgb2yuv[BY_IDX];
	    int ru = rgb2yuv[RU_IDX], gu = rgb2yuv[GU_IDX], bu = rgb2yuv[BU_IDX];
	    int rv = rgb2yuv[RV_IDX], gv = rgb2yuv[GV_IDX], bv = rgb2yuv[BV_IDX];
	    int y;
	    const int chromWidth = width >> 1;
	    int i;

	    for (y = 0; y < height; y += 2) {

	        for (i = 0; i < chromWidth; i++) {
	            unsigned int b = src[6 * i + 0];
	            unsigned int g = src[6 * i + 1];
	            unsigned int r = src[6 * i + 2];

	            unsigned int Y = ((ry * r + gy * g + by * b) >> RGB2YUV_SHIFT) +  16;
	            unsigned int V = ((rv * r + gv * g + bv * b) >> RGB2YUV_SHIFT) + 128;
	            unsigned int U = ((ru * r + gu * g + bu * b) >> RGB2YUV_SHIFT) + 128;

	            udst[i]     = U;
	            vdst[i]     = V;
	            ydst[2 * i] = Y;
	            b = src[6 * i + 3];
	            g = src[6 * i + 4];
	            r = src[6 * i + 5];

	            Y = ((ry * r + gy * g + by * b) >> RGB2YUV_SHIFT) + 16;
	            ydst[2 * i + 1] = Y;
	        }
	        ydst += lumStride;

	        src  += srcStride;

	        if (y+1 == height)
	            break;

	        for (i = 0; i < chromWidth; i++) {
	            unsigned int b = src[6 * i + 0];
	            unsigned int g = src[6 * i + 1];
	            unsigned int r = src[6 * i + 2];

	            unsigned int Y = ((ry * r + gy * g + by * b) >> RGB2YUV_SHIFT) + 16;

	            ydst[2 * i] = Y;
	            b = src[6 * i + 3];
	            g = src[6 * i + 4];
	            r = src[6 * i + 5];

	            Y = ((ry * r + gy * g + by * b) >> RGB2YUV_SHIFT) + 16;
	            ydst[2 * i + 1] = Y;
	        }
	        udst += chromStride;
	        vdst += chromStride;
	        ydst += lumStride;
	        src  += srcStride;
	    }
	}

	int ColorSpace::bgr24ToYUV420( unsigned char *src, unsigned char *yuvdst, int width, int height)
	{
		if(src == 0 || yuvdst == 0 || width < 2 || height < 2 || width%2 != 0 || height%2 != 0) return 0;
		ff_rgb24toyv12_c(src,yuvdst, yuvdst + width*height, yuvdst + width*height + ((width*height) >> 2), width, height, width, width >> 1, 3 * width, table);
		return 1;
	}

	inline unsigned char ColorSpace::clamp0255(unsigned short x)
	{
		if(x<0)
			return 0;
		else if (x>255)
			return 255;
		return x;
	}

	inline void ColorSpace::rgb_calc(yuv2rgb_rgb_t* rgb, int Y, int Cr, int Cb) {
		rgb->r = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);
		if (rgb->r < 0)
			rgb->r = 0;
		else if (rgb->r > 255)
			rgb->r = 255;

		rgb->g = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);
		if (rgb->g < 0)
			rgb->g = 0;
		else if (rgb->g > 255)
			rgb->g = 255;

		rgb->b = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);
		if (rgb->b < 0)
			rgb->b = 0;
		else if (rgb->b > 255)
			rgb->b = 255;
	}

	void ColorSpace::modify_yuv420splum(unsigned char* yuv420sp, int width, int height, int luminance)
	{
		int x = 0;
		int y = 0;

		unsigned char* pY = yuv420sp;

		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				if ((*pY) < 250)
				{
					*pY = clamp0255((*pY)+luminance);
				}
				pY++;
			}
		}
	}

	/*
	 * 实际是将YUV420SP的数据转换为BGRA的数据,低字节存放的是B G R A的顺序
	 */
	void ColorSpace::yuv420sp_to_bgra(unsigned char const* yuv420sp, int width, int height, unsigned char* rgba) {
	    const int width4 = width * 4;//一个像素点占4个字节，一行有4*width个字节，width4表示一行的字节数
	    unsigned char const* y0_ptr = yuv420sp;//yuv420sp数据的首地址
	    unsigned char const* y1_ptr = yuv420sp + width;//偏移首行后的Y亮度区域
	    unsigned char const* cb_ptr = yuv420sp + (width * height);//偏移整个Y亮度区域后到cb区域
	    unsigned char const* cr_ptr = cb_ptr + 1;//YUV420SP为YYYY UVUV这样的排列,此处偏移到cr区域的首地址
	    unsigned char* rgba0 = rgba;//实际是bgra的首地址
	    unsigned char* rgba1 = rgba + width4;//偏移到bgra数据区域的第二行
	    unsigned int Y00, Y01, Y10, Y11;
	    unsigned int Cr = 0;
	    unsigned int Cb = 0;
	    int r, c;
	    yuv2rgb_rgb_t rgb00, rgb01, rgb10, rgb11;

	    for (r = 0; r < height / 2; ++r) {
	        for (c = 0; c < width / 2; ++c, cr_ptr += 2, cb_ptr += 2) {
	            Cr = *cr_ptr;
	            Cb = *cb_ptr;
	            if (Cb < 0)
	                Cb += 127;
	            else
	                Cb -= 128;
	            if (Cr < 0)
	                Cr += 127;
	            else
	                Cr -= 128;

	            Y00 = *y0_ptr++;
	            //Y00 = clamp0255(Y00+60);
	            Y01 = *y0_ptr++;
	           // Y01 = clamp0255(Y01+60);
	            Y10 = *y1_ptr++;
	           // Y10 = clamp0255(Y10+60);
	            Y11 = *y1_ptr++;
	            //Y11 = clamp0255(Y11+60);

	            rgb_calc(&rgb00, Y00, Cr, Cb);
	            rgb_calc(&rgb01, Y01, Cr, Cb);
	            rgb_calc(&rgb10, Y10, Cr, Cb);
	            rgb_calc(&rgb11, Y11, Cr, Cb);
	            YUV2RGB_SET_RGB(rgba0, rgb00);
	            YUV2RGB_SET_RGB(rgba0, rgb01);
	            YUV2RGB_SET_RGB(rgba1, rgb10);
	            YUV2RGB_SET_RGB(rgba1, rgb11);
	        }
	        y0_ptr += width;
	        y1_ptr += width;
	        rgba0 += width4;
	        rgba1 += width4;
	    }
	}

#if 1
	void ColorSpace::YUV420SP2RGBA( unsigned char* rgb, unsigned char* yuv, int width, int height )
	{
		unsigned int* rgbData = ( unsigned int* )rgb;

		int sz = width * height;

		for( int j = 0; j < height; j++ )
		{
			int pixPtr = j * width;
			int jDiv2 = j >> 1;
			for( int i = 0; i < width; i++ )
			{
				int Y = yuv[ pixPtr ];
				int Cb = 0;
				int Cr = 0;

				if( Y < 0 )
				{
					Y += 255;
				}
				if( ( i & 0x1 ) != 1 )
				{
					int cOff = sz + jDiv2 * width + ( i >> 1 ) * 2;
					Cb = yuv[ cOff ];
					if( Cb < 0 )
					{
						Cb += 127;
					}
					else
					{
						Cb -= 128;
					}
					Cr = yuv[ cOff + 1 ];
					if( Cr < 0 )
					{
						Cr += 127;
					}
					else
					{
						Cr -= 128;
					}
				}
				int R = Y + Cr + ( Cr >> 2 ) + ( Cr >> 3 ) + ( Cr >> 5 );
				if( R < 0 )
				{
					R = 0;
				}
				else if( R > 255 )
				{
					R = 255;
				}
				int G = Y - ( Cb >> 2 ) + ( Cb >> 4 ) + ( Cb >> 5 ) - ( Cr >> 1 ) + ( Cr >> 3 ) + ( Cr >> 4 ) + ( Cr >> 5 );
				if( G < 0 )
				{
					G = 0;
				}
				else if( G > 255 )
				{
					G = 255;
				}
				int B = Y + Cb + ( Cb >> 1 ) + ( Cb >> 2 ) + ( Cb >> 6 );
				if( B < 0 )
				{
					B = 0;
				}
				else if( B > 255 )
				{
					B = 255;
				}
				rgbData[ pixPtr++ ] = 0xff000000 + ( R << 16 ) + ( G << 8 ) + B;
			}
		}
	}
#endif
}
