#ifndef __COLORSPACE_H__
#define __COLORSPACE_H__

#include "Defines.h"
#include "Common.h"

//#define YUV2RGB_SET_RGB(p, rgb) *p++ = (unsigned char)rgb.r; *p++ = (unsigned char)rgb.g; *p++ = (unsigned char)rgb.b; *p++ = 0x00
#define RY_IDX 0
#define GY_IDX 1
#define BY_IDX 2
#define RU_IDX 3
#define GU_IDX 4
#define BU_IDX 5
#define RV_IDX 6
#define GV_IDX 7
#define BV_IDX 8
#define RGB2YUV_SHIFT 15

#define YUV2RGB_SET_RGB(p, rgb) *p++ = (unsigned char)rgb.b; *p++ = (unsigned char)rgb.g; *p++ = (unsigned char)rgb.r; *p++ = 0xff
namespace KugouPlayer
{
	const static int table[] = {
			8414,16519, 3208, -4865, -9528, 14392, 14392, -12061, -2332,/* 0,
			0, 0, 0, 0, 0, 0, 1082592392, 210239488, 1082592392, 210239488,
			8414,551436423, 8414, 551436423, 1082597598, 551419904, 1082597598, 551419904, 3208,210256007,
			3208, 210256007, -624412616, 943194112, -624412616, 943194112, 60671, -318776632, 60671, -318776632,
			-624366337,	-318832640, -624366337, -318832640, 14392, 943250120, 14392, 943250120, -790366492, -152829952,
			-790366492, -152829952, 14392, 943247587, 14392, 943247587, -790415304, 943194112, -790415304, 943194112,
			63204, -152776477, 63204, -152776477, 210247902, 210247902, 210247902, 210247902, 551423112, 551423112,
			551423112, 551423112, 16519, 16519, 16519, 16519, 1082589184, 1082589184, 1082589184, 1082589184,
			943254783, 943254783, 943254783, 943254783, -318818248, -318818248, -318818248, -318818248, 56008, 56008,
			56008,	56008, -624427008, -624427008, -624427008, -624427008, -152815560, -152815560, -152815560, -152815560,
			943257316,	943257316, 943257316, 943257316, 53475, 53475, 53475, 53475, -790429696, -790429696,
			-790429696, -790429696 */
	};

	class ColorSpace
	{
	public:
		typedef struct yuv2rgb_rgb_t
		{
			int r, g, b;
		} yuv2rgb_rgb_t;


	public:
		static void BRGA2BGR(unsigned char * des, unsigned char * src, int width, int height);
		static void ff_rgb24toyv12_c( unsigned char *src, unsigned char *ydst, unsigned char *udst, unsigned char *vdst, int width, int height, int lumStride, int chromStride, int srcStride, const int rgb2yuv[]);
		static int bgr24ToYUV420( unsigned char *src, unsigned char *yuvdst, int width, int height);

		static inline void rgb_calc(yuv2rgb_rgb_t* rgb, int Y, int Cr, int Cb);
		static inline unsigned char clamp0255(unsigned short x);
		static void modify_yuv420splum(unsigned char* yuv420sp, int width, int height, int luminance);
		static void yuv420sp_to_bgra(unsigned char const* yuv420sp, int width, int height, unsigned char* rgba);
		static void YUV420SP2RGBA( unsigned char* rgb, unsigned char* yuv, int width, int height );
	};
}

#endif
