#include "Bitmap.h"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "Common.h"

namespace KugouPlayer
{

#ifndef BI_RGB
#	define BI_RGB 0
#endif

	typedef struct _RGBQUAD
	{
		byte red;
		byte green;
		byte blue;
		byte reserved;
	}RGBQUAD;

	inline bool read(void * buffer, size_t size, FILE * fp)
	{
		return ::fread(buffer, 1, size, fp) == size;
	}

	inline bool write(void * buffer, size_t size, FILE * fp)
	{
		return ::fwrite(buffer, 1, size, fp) == size;
	}

	inline bool skip(size_t size, FILE * fp)
	{
		return ::fseek(fp, size, SEEK_SET) == 0;
	}

	Bitmap::Bitmap(void)
	{
		Initialize();
	}

	Bitmap::Bitmap(const Bitmap & r)
	{
		//BITMAP FILE HEADER
		this->bfType = r.bfType;
		this->bfSize = r.bfSize;
		this->bfReserved1 = r.bfReserved1;
		this->bfReserved2 = r.bfReserved2;
		this->bfOffBits = r.bfOffBits;
		//BITMAP INFO HEADER
		this->biSize = r.biSize;
		this->biWidth = r.biWidth;
		this->biPlanes = r.biPlanes;
		this->biBitCount = r.biBitCount;
		this->biCompression = r.biCompression;
		this->biSizeImage = r.biSizeImage;
		this->biXPelsPerMeter = r.biXPelsPerMeter;
		this->biYPelsPerMeter = r.biYPelsPerMeter;
		this->biClrUsed = r.biClrUsed;
		this->biClrImportant = r.biClrImportant;
		//BITMAP DATA
		this->size = r.size;
		this->bits = new byte[size];
		assert(this->bits != 0);
		memcpy(this->bits, r.bits, this->size);
	}

	Bitmap::Bitmap(int width, int height, int bitCount, int initValue)
	{
		Initialize();

		Alloc(width, height, bitCount, initValue);
	}

	Bitmap::~Bitmap()
	{
		Cleanup();
	}

	byte & Bitmap::operator[](const uint index)
	{
		assert(index >= 0 && index < size);
		return bits[index];
	}

	const Bitmap & Bitmap::operator=(const Bitmap & r)
	{
		if (this != &r)
		{
			//BITMAP FILE HEADER
			this->bfType = r.bfType;
			this->bfSize = r.bfSize;
			this->bfReserved1 = r.bfReserved1;
			this->bfReserved2 = r.bfReserved2;
			this->bfOffBits = r.bfOffBits;
			//BITMAP INFO HEADER
			this->biSize = r.biSize;
			this->biWidth = r.biWidth;
			this->biHeight = r.biHeight;
			this->biPlanes = r.biPlanes;
			this->biBitCount = r.biBitCount;
			this->biCompression = r.biCompression;
			this->biSizeImage = r.biSizeImage;
			this->biXPelsPerMeter = r.biXPelsPerMeter;
			this->biYPelsPerMeter = r.biYPelsPerMeter;
			this->biClrUsed = r.biClrUsed;
			this->biClrImportant = r.biClrImportant;
			//BITMAP DATA
			if (this->bits != 0)
			{
				delete[] this->bits;
				this->bits = 0;
			}

			this->size = r.size;
			this->bits = new byte[size];
			assert(bits != 0);
			memcpy(bits, r.bits, size);
		}

		return *this;
	}

	void Bitmap::Initialize()
	{
		bits = 0;
		size = 0;

		bfType = 0;
		bfSize = 0;
		bfReserved1 = 0;
		bfReserved2 = 0;
		bfOffBits = 0;

		biSize = 0;
		biWidth = 0;
		biHeight = 0;
		biPlanes = 0;
		biBitCount = 0;
		biCompression = 0;
		biSizeImage = 0;
		biXPelsPerMeter = 0;
		biYPelsPerMeter = 0;
		biClrUsed = 0;
		biClrImportant = 0;
	}

	bool Bitmap::Alloc(int width, int height, int bitCount, int initValue /* = 0x00 */)
	{
		int lineBytes = WIDTHBYTES(width * bitCount);//保证图像的每一行的字节数是4的整数倍，目前是按每个像素点32位来处理的
		int imageSize = lineBytes * height;//得到的是32位一个像素点的整张图像的字节数大小
		int paletteSize = (bitCount << 1) * sizeof(RGBQUAD);//调色板大小

		this->bfType = 0x4D42;

		if (bitCount == 8)
		{
			this->bfOffBits = 54 + paletteSize;
			this->bfSize = 54 + paletteSize + imageSize;
		}
		else if (bitCount == 24 || bitCount == 32)
		{
			this->bfOffBits = 54;
			this->bfSize = 54 + imageSize;
		}

		bool flag = (biWidth != width || biHeight != height || biBitCount != bitCount);

		this->biSize = 40;//位图信息头固定是40字节
		this->biWidth = width;
		this->biHeight = height;
		this->biBitCount = bitCount;//像素点位数
		this->biSizeImage = imageSize;//图像总字节数
		this->biPlanes = 1;
		this->biCompression = BI_RGB;
		this->biClrUsed = 0;
		this->biClrImportant = 0;
		this->biXPelsPerMeter = 3780;
		this->biYPelsPerMeter = 3780;

		if (flag)//当宽与高发生改变时候才重新申请内存
		{
			byte * p = new byte[imageSize];//Bitmap类内部分配图像数据的空间是按bitCount为32位分配了空间
			if (p == 0) return false;

			memset(p, initValue, imageSize * sizeof(byte));

			if (bits != 0)
			{
				delete[] bits;
				bits = 0;
			}

			bits = p;
		}
		else
		{
			memset(bits, initValue, imageSize  * sizeof(byte));
		}

		size = imageSize;

		return true;
	}

	void Bitmap::SwapChannel(int channel0, int channel1)
	{
		int lineBytes = WIDTHBYTES(biBitCount * biWidth);

		for (uint32 y = 0; y < biHeight; y++)
		{
			byte * p0 = bits + y * lineBytes;

			for (uint32 x = 0; x < biWidth; x++)
			{
				byte temp = *(p0 + channel0);
				*(p0 + channel0) = *(p0 + channel1);
				*(p0 + channel1) = temp;

				p0 += biBitCount / 8;
			}
		}
	}

	void Bitmap::Clear(void)
	{
		Cleanup();
	}

	void Bitmap::ClearColor(uint8 color /* = 0xff */)
	{
		memset(bits, color, size);
	}

	int Bitmap::GetWidthByte(void)
	{
		return WIDTHBYTES(biBitCount * biWidth);
	}

	void Bitmap::Cleanup(void)
	{
		if (bits != 0)
		{
			delete[] bits;
		}

		Initialize();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Bitmap::Load(Bitmap & bitmap, const char* fileName)
	{
		bitmap.Clear();//复位位图的每个信息参数的值

		if (fileName == 0) return false;
		FILE * fp = NULL;
		//fopen_s(&fp, fileName, "rb");
		fp = fopen(fileName, "rb");
		if (fp == 0) return false;

		bool result = false;

		do
		{
			//read bitmap file header  位图的文件头  占14字节
			if (!read(&bitmap.bfType, sizeof(bitmap.bfType), fp)) break; //bfType    说明文件类型，在windows系统中为BM
			if (bitmap.bfType != 0x4D42) break;//固定其值为0x4D42,表示是bmp图像

			if (!read(&bitmap.bfSize, sizeof(bitmap.bfSize), fp)) break;//整个文件大小(字节数)
			if (!read(&bitmap.bfReserved1, sizeof(bitmap.bfReserved1), fp)) break;//保留值,设置为0
			if (!read(&bitmap.bfReserved2, sizeof(bitmap.bfReserved2), fp)) break;//保留值,设置为0
			if (!read(&bitmap.bfOffBits, sizeof(bitmap.bfOffBits), fp)) break;//说明实际图形数据的偏移量,这个值固定是54(0x36),因为位图的文件头占14个字节，位图的文件信息头占40字节

			//read bitmap info header  位图的文件信息头
			if (!read(&bitmap.biSize, sizeof(bitmap.biSize), fp)) break;//位图文件信息头的字节数:40
			if (!read(&bitmap.biWidth, sizeof(bitmap.biWidth), fp)) break;//位图的宽
			if (!read(&bitmap.biHeight, sizeof(bitmap.biHeight), fp)) break;//位图的高
			if (!read(&bitmap.biPlanes, sizeof(bitmap.biPlanes), fp)) break;//为目标设备说明位面数，其值设为１
			if (!read(&bitmap.biBitCount, sizeof(bitmap.biBitCount), fp)) break;//每个像素的位数，单色位图为１，２５６色为８，２４bit为２４,32bit为32
			if (!read(&bitmap.biCompression, sizeof(bitmap.biCompression), fp))	break;//压缩说明，BI_RGB：无压缩，BI_RLE8：８位RLE压缩，BI_RLE4：４位RLE压缩 压缩方式，可以是0，1，2，其中0表示不压缩
			if (!read(&bitmap.biSizeImage, sizeof(bitmap.biSizeImage), fp)) break;//实际位图数据占用的字节数  是按每一行的字节数必须是4的倍数对齐的标准组成的实际图像的字节数
			
			/*
			比如一幅图片640*480，就是说水平640个像素,垂直480个像素，但是并没有指定该附图的精度，这幅图片在不同的设备上高宽显示会不相同。
			biXPelsPerMeter和biYpelsPerMeter则表示图片像素的精度，做个极端的假设，假如每米一个像素，那么640*480就需要水平640m,垂直480m，这么大的尺寸，估计坐在飞机上你才能看到这幅图片的全貌
			*/
			if (!read(&bitmap.biXPelsPerMeter, sizeof(bitmap.biXPelsPerMeter), fp)) break;//X方向分辨率  每米像素数 
			if (!read(&bitmap.biYPelsPerMeter, sizeof(bitmap.biYPelsPerMeter), fp))	break;//Y方向的分辨率 每米像素数 
			if (!read(&bitmap.biClrUsed, sizeof(bitmap.biClrUsed), fp)) break;//使用的颜色数，如果为0，则表示默认值(2^颜色位数)  表示位图实际使用的颜色表中的颜色变址
			if (!read(&bitmap.biClrImportant, sizeof(bitmap.biClrImportant), fp)) break;//重要颜色数，如果为0，则表示所有颜色都是重要的 表示位图显示过程中被认为重要颜色变址数
		
			if (bitmap.biBitCount != 8 && bitmap.biBitCount != 24 && bitmap.biBitCount != 32)
			{
				break;
			}

			LOGE("bitmap.biBitCount:%d\r\n", bitmap.biBitCount);
			// 每一行的字节数必须是4的整倍数，如果不是，则需要补齐 #define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)
			int lineBytes = WIDTHBYTES(bitmap.biWidth * bitmap.biBitCount);
			//assert(bitmap.biHeader.biSizeImage == lineBytes * bitmap.biHeader.biHeight);

			bitmap.size = lineBytes * bitmap.biHeight;
			bitmap.bits = new uint8[bitmap.size];//为实际数据分配了空间
			if (bitmap.bits == 0) break;

			if (!skip(bitmap.bfOffBits, fp))break;//跳过位图文件头和文件信息头，跳到实际数据处

			/*
			第三部分为调色板Palette，当然，这里是对那些需要调色板的位图文件而言的。24位和32位是不需要调色板的。
			第四部分就是实际的图象数据了。对于用到调色板的位图，图象数据就是该象素颜在调色板中的索引值。对于真彩色图，图象数据就是实际的R、G、B值。对于2色位图，
			用1位就可以表示该象素的颜色(一般0表示黑，1表示白)，所以一个字节可以表示8个象素。对于16色位图，用4位可以表示一个象素的颜色，所以一个字节可以表示2个象素。
			对于256色位图，一个字节刚好可以表示1个象素。对于真彩色图，三个字节才能表示1个象素。要注意两点： (1) 每一行的字节数必须是4的整倍数，如果不是，则需要补齐。
			(2) 一般来说，.bMP文件的数据从下到上，从左到右的。也就是说，从文件中最先读到的是图象最下面一行的左边第一个象素，然后是左边第二个象素……接下来是倒数第二行左边第一个象素，左边第二个象素……依次类推 ，
			最后得到的是最上面一行的最右一个象素。
			其实每幅图像都是由一个一个的点组成的。在位图阵列里就存放着每一个点的颜色信息。每个像素点是由 3 个字节的数据组成的。也就是我们常用到的 RGB 色彩空间每种颜色占一个字节。
			每个点的颜色都是由蓝色、绿色、红色这个顺序存储的。
			*/
			if (!read(bitmap.bits, bitmap.size, fp))//读取了位图的图像部分的数据
			{
				delete bitmap.bits;
				bitmap.bits = 0;
				bitmap.size = 0;
				return false;
			}
			LOGE("bitmap.size:%d\r\n", bitmap.size);

			result = true;
		} while (0);

		fclose(fp);

		return result;
	}

	bool Bitmap::Save(const char* fileName, const Bitmap & bitmap)
	{
		return Save(fileName, bitmap.bits, bitmap.biWidth, bitmap.biHeight, bitmap.biBitCount);
	}

	bool Bitmap::Save(const char* fileName, byte * bits, int width, int height, int bitCount)
	{
		if (fileName == 0) return false;

		FILE * fp = NULL;
		//fopen_s(&fp, fileName, "wb");
		fp = fopen(fileName, "wb");

		if (fp == 0) return false;

		bool result = false;

		do
		{
			int lineBytes = WIDTHBYTES(width * bitCount);
			int paletteSize = (1 << bitCount) * sizeof(RGBQUAD);
			int imageSize = lineBytes * height;

			uint16  bfType = 0x4D42;
			uint32  bfSize = 0;
			uint16  bfReserved1 = 0;
			uint16  bfReserved2 = 0;
			uint32  bfOffBits = 0;

			if (bitCount == 8)
			{
				bfOffBits = 54 + paletteSize;
				bfSize = 54 + paletteSize + imageSize;
			}
			else if (bitCount == 24 || bitCount == 32)
			{
				bfOffBits = 54;
				bfSize = 54 + imageSize;
			}

			uint32 biSize = 40;
			uint32 biWidth = width;
			uint32 biHeight = height;
			uint16 biPlanes = 1;
			uint16 biBitCount = bitCount;
			uint32 biCompression = BI_RGB;
			uint32 biSizeImage = imageSize;
			uint32 biXPelsPerMeter = 3780;
			uint32 biYPelsPerMeter = 3780;
			uint32 biClrUsed = 0;
			uint32 biClrImportant = 0;

			if (!write(&bfType, sizeof(bfType), fp)) break;
			if (!write(&bfSize, sizeof(bfSize), fp)) break;
			if (!write(&bfReserved1, sizeof(bfReserved1), fp)) break;
			if (!write(&bfReserved2, sizeof(bfReserved2), fp)) break;
			if (!write(&bfOffBits, sizeof(bfOffBits), fp)) break;

			if (!write(&biSize, sizeof(biSize), fp)) break;
			if (!write(&biWidth, sizeof(biWidth), fp)) break;
			if (!write(&biHeight, sizeof(biHeight), fp)) break;
			if (!write(&biPlanes, sizeof(biPlanes), fp)) break;
			if (!write(&biBitCount, sizeof(biBitCount), fp)) break;
			if (!write(&biCompression, sizeof(biCompression), fp)) break;
			if (!write(&biSizeImage, sizeof(biSizeImage), fp)) break;
			if (!write(&biXPelsPerMeter, sizeof(biXPelsPerMeter), fp)) break;
			if (!write(&biYPelsPerMeter, sizeof(biYPelsPerMeter), fp)) break;
			if (!write(&biClrUsed, sizeof(biClrUsed), fp)) break;
			if (!write(&biClrImportant, sizeof(biClrImportant), fp)) break;

			if (bitCount == 8)
			{
				RGBQUAD * rgbQuad = new RGBQUAD[1 << bitCount];

				for (int i = 0; i < (1 << bitCount); i++)
				{
					rgbQuad[i].red = i;
					rgbQuad[i].green = i;
					rgbQuad[i].blue = i;
				}

				if (!write(rgbQuad, sizeof(RGBQUAD) * (1 << bitCount), fp))
				{
					break;
				}

				delete rgbQuad;
			}

			if (!write(bits, imageSize, fp)) break;
			
			result = true;

		} while (0);

		fclose(fp);

		return result;
	}
}


