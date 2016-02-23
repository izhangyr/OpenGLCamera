#ifndef __OPENGLRENDER_H__
#define __OPENGLRENDER_H__

#include "Common.h"
#include <GLES2/gl2.h>

namespace KugouPlayer
{
	class OpenGLRender
	{
	public:
		OpenGLRender();
		~OpenGLRender();

		void setDisplayArea( int x, int y, int width, int height );

		void setRotation( int degrees, int flipHorizontal, int flipVertical );

		void enableGaussFilter( bool flag )//是否使能高斯滤波
		{
			mGaussFilterFlag = flag;
		}

		void render( unsigned char* buffer, int widthTexture, int heightTexture );

	private:
		OpenGLRender& operator=( const OpenGLRender & );
		OpenGLRender( const OpenGLRender & );

		static float flip( float i );

		static unsigned int _CompileShader( unsigned int type, const char *shaderSource );

		int _RegenTextures();

		int _LoadShader();
		void Cleanup(void);

	private://OpenGLRender类中的常变量
		static const char* VERTEX_SHADER_STRING;

		static const char* YUV_FRAGMENT_SHADER_STRING;

		static const float squardVertices[ 8 ];//顶点坐标集

		static const float TEXTURE_ROTATED[ 4 ][ 8 ];//用于旋转的一些数值

		static const int ATTRIBUTE_VERTEX = 0;

		static const int ATTRIBUTE_TEXCOORD = 1;

	private:
		float coordVertices[ 8 ];//顶点坐标

		bool mGaussFilterFlag;
		bool mGaussFilterDrawFlag;//默认为TRUE

		unsigned int program;
		unsigned int textureId;

		int texture;//纹理
		int texturesize;
		int enablegaussfilter;

		unsigned int vertexShader;//顶点着色
		unsigned int fragmentShader;//分段着色

		int x;
		int y;
		int width;
		int height;
	};
}

#endif
