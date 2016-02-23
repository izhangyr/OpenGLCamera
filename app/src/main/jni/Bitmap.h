#ifndef __CORE_BITMAP_H__
#define __CORE_BITMAP_H__

#include "Defines.h"
#include "Common.h"

namespace KugouPlayer
{
	class Bitmap
	{
	public:
		Bitmap(void);
		Bitmap(const Bitmap & r);
		Bitmap(int width, int height, int bitCount, int initValue = 0x00);
		virtual ~Bitmap(void);
		byte & operator[](const uint index);
		const Bitmap & operator=(const Bitmap & r);

		bool Alloc(int width, int height, int bitCount, int initValue = 0x00);
		void SwapChannel(int channel0, int channel1);
		void Clear(void);
		void ClearColor(uint8 color = 0xff);
		int  GetWidthByte(void);
		static bool Load(Bitmap & bitmap, const char* fileName);
		static bool Save(const char* fileName, const Bitmap & bitmap);
	private:
		void Initialize(void);
		void Cleanup(void);
		static bool Save(const char* fileName, byte * bits, int width, int height, int bitCount);
	public:
		byte*   bits;
		uint32  size;

		uint16  bfType;
		uint32  bfSize;
		uint16  bfReserved1;
		uint16  bfReserved2;
		uint32  bfOffBits;

		uint32  biSize;
		uint32  biWidth;
		uint32  biHeight;
		uint16  biPlanes;
		uint16  biBitCount;
		uint32  biCompression;
		uint32  biSizeImage;
		uint32  biXPelsPerMeter;
		uint32  biYPelsPerMeter;
		uint32  biClrUsed;
		uint32  biClrImportant;
	};
}

#endif
