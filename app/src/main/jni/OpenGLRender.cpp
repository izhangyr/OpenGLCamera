#include <GLES2/gl2.h>
#include <stdlib.h>
#include <stdio.h>

#include "OpenGLRender.h"
#include "ImageFilter.h"

namespace KugouPlayer
{
	void RGBA2BGR(uint8 * des, uint8 * src, int width, int height)
	{
		for (int y = 0; y < height; y++)
		{
			uint8 * p0 = des + y * WIDTHBYTES(24 * width);
			uint8 * p1 = src + y * WIDTHBYTES(32 * width);

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 2);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 0);

				p0 += 3;
				p1 += 4;
			}
		}
	}

	void BGRA2RGBA(uint8 * des, uint8 * src, int width, int height)//24位bmp数据是按BGR的顺序存放,B为低字节,R为高字节
	{
		for (int y = 0; y < height; y++)
		{
			uint8 * p0 = des + y * WIDTHBYTES(32 * width);
			uint8 * p1 = src + y * WIDTHBYTES(32 * width);

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 2);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 0);
				*(p0 + 3) = 0xff;

				p0 += 4;
				p1 += 4;
			}
		}
	}

	void RGBA2BGRA(uint8 * des, uint8 * src, int width, int height)
	{
		for (int y = 0; y < height; y++)
		{
			uint8 * p0 = des + y * WIDTHBYTES(32 * width);
			uint8 * p1 = src + y * WIDTHBYTES(32 * width);

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 2);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 0);
				*(p0 + 3) = 0xff;

				p0 += 4;
				p1 += 4;
			}
		}
	}

	/*
	 *着色器与着色语言
	着色语言是一种类C语言，但不像C语言一样支持双精度浮点型(double)、字节型(byte)、短整型(short)、长整型(long)，并且取消了C中的联合体(union)、枚举类型(enum)、无符号数(unsigned)以及位运算等特性。
	着色语言中有许多内建的原生数据类型以及构建数据类型，如：浮点型(float)、布尔型(bool)、整型(int)、矩阵型(matrix)以及向量型(vec2、vec3等)等。总体来说，这些数据类型可以分为标量、向量、矩阵、采样器、结构体以及数组等。
	shader支持下面数据类型：
	Float  bool  int
	vec2           包含了2个浮点数的向量     vec3          包含了3个浮点数的向量     vec4         包含了4个浮点数的向量
	ivec2          包含了2个整数的向量         ivec3       包含了3个整数的向量      ivec4       包含了4个整数的向量
	bvec2          包含了2个布尔数的向量    bvec3        包含了3个布尔数的向量   bvec4       包含了4个布尔数的向量
	mat2    2*2维矩阵                           mat3   3*3维矩阵                          mat4    4*4维矩阵
    sampler1D   1D纹理着色器          sampler2D    2D纹理着色器
	sampler3D   3D纹理着色器

	OpenGL ES2.0的shader里面声明的变量一般有三种类型uniform, attribute, varying。
	1.uniform变量
	uniform变量是外部application程序传递给（vertex和fragment）shader的变量。因此它是 application通过函数glUniform**（）函数赋值的。在（vertex和fragment）shader程序内部，uniform变量 就像是C语言里面的常量（const ），它不能被shader程序修改。（shader只能用，不能改）
	如果uniform变量在vertex和fragment两者之间声明方式完全一样，则它可以在vertex和fragment共享使用。（相当于一个被vertex和fragment shader共享的全局变量）
	uniform变量一般用来表示：变换矩阵，材质，光照参数和颜色等信息。
	2.attribute变量
	attribute变量是只能在vertex shader中使用的变量。（它不能在fragment shader中声明attribute变量，也不能被fragment shader中使用）
	一般用attribute变量来表示一些顶点的数据，如：顶点坐标，法线，纹理坐标，顶点颜色等。
	在application中，一般用函数glBindAttribLocation（）来绑定每个attribute变量的位置，然后用函数glVertexAttribPointer（）为每个attribute变量赋值。
	3.varying变量
	varying变量是vertex和fragment shader之间做数据传递用的。一般vertex shader修改varying变量的值，然后fragment shader使用该varying变量的值。因此varying变量在vertex和fragment shader二者之间的声明必须是一致的。
	application不能使用此变量。
	 */
	const char* OpenGLRender::VERTEX_SHADER_STRING =
		"attribute vec4 position;"
		"attribute vec2 texcoord;"
		"varying vec2 v_texcoord;"
		"void main() {"
		"   gl_Position = position;"
		"	v_texcoord = texcoord;"
		"}";

#if 0
	const char* OpenGLRender::YUV_FRAGMENT_SHADER_STRING =
		"varying lowp vec2 v_texcoord;"
		"uniform sampler2D s_texture;"
		"void main(void) {"
		"	gl_FragColor = texture2D(s_texture, v_texcoord);"
		"}";
#else
	const char* OpenGLRender::YUV_FRAGMENT_SHADER_STRING =
		"varying lowp vec2 v_texcoord;\n"
		"uniform sampler2D s_texture;\n"
		"uniform lowp vec2 TexSize;\n"
		"uniform lowp int enablegaussfilter;\n"
		"mediump float kernal[21];\n"
		"mediump float softlight(mediump float a, mediump float b)\n"
		"{\n"
		"  if(b <= 0.5)\n"
		"  {\n"
		"    return a * b / 0.5 + (a / 1.0) * (a / 1.0) * (1.0 - 2.0 * b);\n"
		"  }\n"
		"  else\n"
		"  {\n"
		"    return a * (1.0 - b) / 0.5 + sqrt(a / 1.0) * (2.0 * b - 1.0);\n"
		"  }\n"
		"}\n"
		"mediump vec4 softlight(mediump vec4 a, mediump vec4 b)\n"
		"{\n"
		"  return vec4(softlight(a.x, b.x), softlight(a.y, b.y), softlight(a.z, b.z), 1.0);\n"
		"}\n"
		"mediump vec4 gauss_filter(mediump float _kernal[21], sampler2D _image, lowp vec2 _uv, lowp vec2 _texSize)\n"
		"{\n"
		"  mediump vec4 oc = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"  mediump float dx = 1.0 / _texSize.x;\n"
		"  mediump float dy = 1.0 / _texSize.y;\n"
		"  for (int n = 0, i = -10; i <= 10; i++, n++)\n"
		"  {\n"
		"    lowp vec2 _uv_new = vec2(_uv.x + float( i ) * dx, _uv.y);\n"
		"    oc += texture2D(_image, _uv_new) * _kernal[n];\n"
		"  }\n"
		"  mediump vec4 nc = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"  for (int n = 0, i = -10; i <= 10; i++, n++)\n"
		"  {\n"
		"    lowp vec2 _uv_new = vec2(_uv.x, _uv.y + float( i ) * dy);\n"
		"    nc += texture2D(_image, _uv_new) * _kernal[n];\n"
		"  }\n"
		"  return (oc + nc) / vec4(2.0);\n"
		"}\n"
		"mediump vec4 alpha_blend(mediump vec4 a, mediump vec4 b)\n"
		"{\n"
		"  return a * vec4(1.0 - b.a) + b * vec4(b.a);\n"
		"}\n"
		"void main(void)\n"
		"{\n"
		"  kernal[0] = float(0.001332);kernal[1] = float(0.003131);kernal[2] = float(0.006729);\n"
		"  kernal[3] = float(0.013216);kernal[4] = float(0.023722);kernal[5] = float(0.038916);\n"
		"  kernal[6] = float(0.058347);kernal[7] = float(0.079951);kernal[8] = float(0.100124);\n"
		"  kernal[9] = float(0.114596);kernal[10] = float(0.119871);kernal[11] = float(0.114596);\n"
		"  kernal[12] = float(0.100124);kernal[13] = float(0.079951);kernal[14] = float(0.058347);\n"
		"  kernal[15] = float(0.038916);kernal[16] = float(0.023722);kernal[17] = float(0.013216);\n"
		"  kernal[18] = float(0.006729);kernal[19] = float(0.003131);kernal[20] = float(0.001332);\n"
		"  mediump vec4 a = texture2D(s_texture, v_texcoord);\n"
		"  if( enablegaussfilter == 0 )\n"
		"  {\n"
		"    gl_FragColor = a;\n"
		"  }\n"
		"  else\n"
		"  {\n"
		"    mediump vec4 b = gauss_filter(kernal, s_texture, v_texcoord, TexSize);\n"
		"    mediump vec4 c = softlight(a, b);\n"
		"    gl_FragColor = c;\n"
		"  }"
		"}\n";
#endif

	const float OpenGLRender::squardVertices[ 8 ] =
		{
			-1.0f, -1.0f,
			1.0f, -1.0f,
			-1.0f,  1.0f,
			1.0f,  1.0f
		};

	const float OpenGLRender::TEXTURE_ROTATED[ 4 ][ 8 ] =
		{
			// normal
			{
				0.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
			},
			// rotate 90
			{
				1.0f, 1.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,
			},
			// rotate 180
			{
				1.0f, 0.0f,
				0.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
			},
			// rotate 270
			{
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
			}
		};

	/*
	 * OpenGLRender类的构造函数中初始化其成员变量
	 */
	OpenGLRender::OpenGLRender()
		: mGaussFilterFlag( false ),
		  mGaussFilterDrawFlag( true ),
		  program( 0 ),
		  textureId( 0 ),
		  texture( 0 ),
		  texturesize( 0 ),
		  enablegaussfilter( 0 ),
		  vertexShader( 0 ),
		  fragmentShader( 0 ),
		  x( 0 ),
		  y( 0 ),
		  width( 0 ),
		  height( 0 )
	{
		//将正常的没有经过角度旋转的顶点坐标值拷贝到coordVertices中
		memcpy( coordVertices, TEXTURE_ROTATED[ 0 ], sizeof( coordVertices ) );

		int err = _LoadShader();//加载着色器
		if( err < 0 )
		{
			LOGE( "openGL load shaders failed! err:%d\n", err );
		}
		else
		{
			LOGE( "openGL load shaders success!!!\n" );
		}

		/*
		 * 使用glUseProgram()将OpenGL渲染管道切换到着色器模式，并使用刚才做好的（着色）程序对象。然后，才可以提交顶点。
		 */
		glUseProgram( program );

		/*
		 * glGenTextures函数根据纹理参数返回n个纹理索引，纹理名称集合不必是一个连续的整数集合
		 * glGenTextures就是用来产生你要操作的纹理对象的索引的，比如你告诉OpenGL，我需要5个纹理对象，它会从没有用到的整数里返回5个给你。
		 * 下面的调用是需要1个纹理对象,然后将这个纹理索引值放在textureId中
		 */
		glGenTextures( 1, &textureId );
	}

	OpenGLRender::~OpenGLRender()
	{
		Cleanup();
	}

	void OpenGLRender::setDisplayArea( int x, int y, int width, int height )
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	void OpenGLRender::setRotation( int degrees, int flipHorizontal, int flipVertical )
	{
		/*
		 * 根据旋转度数设置不同的旋转浮点数值
		 */
		float rotatedTex[ 8 ];
		if( degrees == 90 )
		{
			memcpy( rotatedTex, TEXTURE_ROTATED[ 1 ], sizeof( rotatedTex ) );
		}
		else if( degrees == 180 )
		{
			memcpy( rotatedTex, TEXTURE_ROTATED[ 2 ], sizeof( rotatedTex ) );
		}
		else if( degrees == 270 )
		{
			memcpy( rotatedTex, TEXTURE_ROTATED[ 3 ], sizeof( rotatedTex ) );
		}
		else
		{
			memcpy( rotatedTex, TEXTURE_ROTATED[ 0 ], sizeof( rotatedTex ) );
		}

		if ( flipHorizontal )
		{
			rotatedTex[ 0 ] = flip( rotatedTex[ 0 ] );
			rotatedTex[ 2 ] = flip( rotatedTex[ 2 ] );
			rotatedTex[ 4 ] = flip( rotatedTex[ 4 ] );
			rotatedTex[ 6 ] = flip( rotatedTex[ 6 ] );
		}
		if ( flipVertical )
		{
			rotatedTex[ 1 ] = flip( rotatedTex[ 1 ] );
			rotatedTex[ 3 ] = flip( rotatedTex[ 3 ] );
			rotatedTex[ 5 ] = flip( rotatedTex[ 5 ] );
			rotatedTex[ 7 ] = flip( rotatedTex[ 7 ] );
		}

		//对应的旋转角度浮点数值在渲染函数中render中会被实际使用到
		memcpy( coordVertices, rotatedTex, sizeof( coordVertices ) );

		LOGE( "setRotation degrees:%d, flip:%d %d\n", degrees, flipHorizontal, flipVertical );
	}

	float OpenGLRender::flip( float i )
	{
		return 1.0f - i;
	}

	void OpenGLRender::render( unsigned char* buffer, int widthTexture, int heightTexture )
	{
		//unsigned char* rgbbuffer = NULL;
		//int lineBytes = 0;
		//int imageSize = 0;

		//lineBytes =  WIDTHBYTES(widthTexture * 32);
		//imageSize =  lineBytes * heightTexture;

		_RegenTextures();

		/*
		 * glViewport主要完成这样的功能。它负责把视景体截取的图像按照怎样的高和宽显示到屏幕上。
		 */
		glViewport( this->x, this->y, this->width, this->height );
		//指定颜色缓冲区的清理值
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		//传入缓冲标志位，表明需要清除的缓冲
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//LOGE("x=%d y=%d width=%d height=%d widthTexture=%d heightTexture=%d\r\n", x, y, this->width, this->height, widthTexture, heightTexture);

		if( buffer != NULL )
		{
			//rgbbuffer = new unsigned char[imageSize];

			//绑定之前生成的纹理对象索引值
			glBindTexture( GL_TEXTURE_2D, textureId );


			//BGRA2RGBA(rgbbuffer,buffer, widthTexture, heightTexture);


			/*
			        纹理过滤函数glTexParameteri()
			         图象从纹理图象空间映射到帧缓冲图象空间(映射需要重新构造纹理图像,这样就会造成应用到多边形上的图像失真),这时就可用glTexParmeteri()函数来确定如何把纹理象素映射成像素.
			   GL_TEXTURE_WRAP_S: S方向上的贴图模式  GL_TEXTURE_WRAP_T:T方向上的贴图模式.
			   GL_TEXTURE_MAG_FILTER: 放大过滤   GL_TEXTURE_MIN_FILTER: 缩小过滤
			   GL_LINEAR: 线性过滤, 使用距离当前渲染像素中心最近的4个纹素加权平均值.
			   GL_NEAREST:对最接近当前多边形的解析度的两个层级贴图进行采样,然后用这两个值进行线性插值.
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			/*
			 *使用glVertexAttribPointer表示在应用程序中为顶点着色器语言中的attribute类型的变量赋值
			 *第一个参数index:指定要修改的顶点属性的索引值  第二个参数：指定每个顶点属性的组件数量(一个顶点由多少个值组成)。必须为1、2、3或者4。初始值为4。（梦维：如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a））
			 *第三个参数type:指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
			 *第四个参数normalized:指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）
			    第五个参数:stride 指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0。
			   第六个参数:pointer 指定一个指针，指向数组中第一个顶点属性的第一个组件。初始值为0。
			 */
			glVertexAttribPointer( ATTRIBUTE_VERTEX, 2, GL_FLOAT, 0, 0, squardVertices );
			/*
			 * 要启用或者禁用顶点属性数组，调用glEnableVertexAttribArray和glDisableVertexAttribArray传入参数index。如果启用，那么当glDrawArrays或者glDrawElements被调用时，顶点属性数组会被使用。
			 */
			glEnableVertexAttribArray( ATTRIBUTE_VERTEX );

			glVertexAttribPointer( ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, 0, 0, coordVertices );
			glEnableVertexAttribArray( ATTRIBUTE_TEXCOORD );

			glActiveTexture( GL_TEXTURE0 );//激活纹理
			glBindTexture( GL_TEXTURE_2D, textureId );

			//为之前绑定的片段着色器语言的uniform类型的变量进行赋值
			glUniform1i( texture, 0 );//只能传0,其它值则是黑屏

			glUniform2f( texturesize, ( GLfloat )widthTexture, ( GLfloat )heightTexture );

			if( mGaussFilterDrawFlag != mGaussFilterFlag )
			{
				mGaussFilterDrawFlag = mGaussFilterFlag;
				glUniform1i( enablegaussfilter, mGaussFilterDrawFlag ? 1 : 0 );
			}

			/*
			 根据指定的参数，生成一个2D纹理(Texture)
			 第一个参数指定目标纹理，这个值必须是GL_TEXTURE_2D,第二个参数level执行细节级别，0是最基本的图像级别,n表示第N级贴图细化级别
			 第三个参数internalformat 指定纹理中的颜色组件，这个取值和后面的format取值必须相同。可选的值有GL_ALPHA,GL_RGB,GL_RGBA,GL_LUMINANCE, GL_LUMINANCE_ALPHA 等几种。
			第四个参数width 指定纹理图像的宽度，第五个参数height 指定纹理图像的高度，第六个参数border 指定边框的宽度，必须为0.第七个参数format 像素数据的颜色格式，必须和internalformatt取值必须相同。
			第八个参数type 指定像素数据的数据类型。可以使用的值有GL_UNSIGNED_BYTE,GL_UNSIGNED_SHORT_5_6_5,GL_UNSIGNED_SHORT_4_4_4_4,GL_UNSIGNED_SHORT_5_5_5_1。
			第九个参数pixels 指定内存中指向图像数据的指针
			在调用该函数之前，必须调用glBindTexture(GL_TEXTURE_2D, mTextureID );以指定要操作的纹理ID
			*/
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, widthTexture, heightTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );

			/*
			 * 提供绘制功能。当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
			       调用该函数之前需要，调用glEnableVertexAttribArray、glVertexAttribPointer等设置顶点属性和数据。
			  mode，绘制方式，OpenGL2.0以后提供以下参数:GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN。
		      first，从数组缓存中的哪一位开始绘制，一般为0。      count，数组中顶点的数量。
			 */
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);//只能传4，4个顶点

			glDisableVertexAttribArray( ATTRIBUTE_VERTEX );
			glDisableVertexAttribArray( ATTRIBUTE_TEXCOORD );
			glBindTexture( GL_TEXTURE_2D, 0 );//这句可以去掉

			//delete []rgbbuffer;
		}
	}

	unsigned int OpenGLRender::_CompileShader( unsigned int type, const char *shaderSource )
	{
		int status = 0;

		unsigned int shader = glCreateShader( type );//创建着色器shader，目前一般是两个着色器:顶点着色器和片段着色器,返回着色器标识符
		if( ( shader == 0 ) || ( shader == GL_INVALID_ENUM ) )
		{
			return 0;
		}

		/*将着色器语言源代码字符数组绑定到着色器对象，第二个参数表示指定字符指针数组中元素的个数，即多少个着色器源代码.
		 * 第三个参数为字符指针数组地址，第四个参数是个整数指针数组，和shader字符指针数组对应，它指定每个shader源代码的字符串长度。
		 * 若置为NULL表示源代码字符串长度数组的个数为0个
		 *为了使程序简单，在本例中，字符指针数组元素只有一个，即只有一个shaderSource来放源代码。
		 */
		glShaderSource( shader, 1, &shaderSource, NULL );
		glCompileShader( shader );//编译着色器语言

		//用于获得着色器语言编译的状态
		glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
		if( status == GL_FALSE )
		{
			glDeleteShader( shader );//删除着色器
			return 0;
		}

		return shader;
	}

	int OpenGLRender::_RegenTextures()
	{
		if( textureId )
		{
			glDeleteTextures( 1, &textureId );
		}
		glGenTextures( 1, &textureId );
		return 0;
	}

	/*
	 * 加载着色器
	 */
	int OpenGLRender::_LoadShader()
	{
		int err = 0;

		do
		{
			program = glCreateProgram();//申请一个着色器程序,返回该着色器程序的标识符

			/*加载对应的着色器
			 * 需要编译对应的着色器语言
			 */
			vertexShader = _CompileShader( GL_VERTEX_SHADER, VERTEX_SHADER_STRING );
			if( !vertexShader )
			{
				err = -1;
				break;
			}

			fragmentShader = _CompileShader( GL_FRAGMENT_SHADER, YUV_FRAGMENT_SHADER_STRING );
			if( !fragmentShader )
			{
				err = -2;
				break;
			}

			//将编译好的shader附加到程序中
			glAttachShader( program, vertexShader );
			glAttachShader( program, fragmentShader );
			/*
			 *在应用程序中 使用glBindAttribLocation绑定顶点着色器语言中的attribute变量,后面在渲染时会使用函数glVertexAttribPointer（）
			 *在应用程序中 为每个attribute变量赋值
			 */
			glBindAttribLocation( program, ATTRIBUTE_VERTEX, "position" );
			glBindAttribLocation( program, ATTRIBUTE_TEXCOORD, "texcoord" );

			//链接程序
			glLinkProgram( program );

			int status;
			glGetProgramiv( program, GL_LINK_STATUS, &status );//获取程序的链接状态信息
			if( status == GL_FALSE )
			{
				err = -3;
				break;
			}

			glValidateProgram( program );//对（着色）程序对象进行正确性验证
			glGetProgramiv( program, GL_VALIDATE_STATUS, &status );//获取程序的正确性验证状态信息
			if( status == GL_FALSE )
			{
				err = -4;
				break;
			}

			/*
			 * 在应用程序中获得片段着色器中的Uniform类型变量的位置,相当于是在应用程序中使用变量绑定了片段着色器中的Uniform类型变量，
			 * 后面在实际渲染时会在应用程序中对其赋值
			 */
			texture = glGetUniformLocation( program, "s_texture" );
			texturesize = glGetUniformLocation( program, "TexSize" );
			enablegaussfilter = glGetUniformLocation( program, "enablegaussfilter" );
		}
		while( 0 );

		if( err < 0 )
		{
			if( vertexShader )
			{
				glDeleteShader( vertexShader );//删除顶点着色器
				vertexShader = 0;
			}

			if( fragmentShader )
			{
				glDeleteShader( fragmentShader );//删除片段着色器
				fragmentShader = 0;
			}

			if( program )
			{
				glDeleteProgram(program);//删除程序
				program = 0;
			}
		}

		return err;
	}

	void OpenGLRender::Cleanup()
	{
		if( vertexShader )
		{
			glDeleteShader( vertexShader );//删除顶点着色器
			vertexShader = 0;
		}

		if( fragmentShader )
		{
			glDeleteShader( fragmentShader );//删除片段着色器
			fragmentShader = 0;
		}

		if( program )
		{
			glDeleteProgram(program);//删除程序
			program = 0;
		}
	}
}



