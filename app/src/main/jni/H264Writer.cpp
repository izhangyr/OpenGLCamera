#include <stdlib.h>

#include "H264Writer.h"

namespace KugouPlayer
{
	H264Writer::H264Writer( const char* path, int width, int height )
		: pFile( NULL ),
		  handle( NULL ),
		  param( NULL ),
		  picture( NULL ),
		  nal( NULL )
	{
		param = ( x264_param_t* )malloc( sizeof( x264_param_t ) );
		picture = ( x264_picture_t* )malloc( sizeof( x264_picture_t ) );
		x264_param_default( param ); //set default param
		//param->rc.i_rc_method = X264_RC_CQP;
		param->i_log_level = X264_LOG_NONE;
		param->i_width = width; //set frame width
		param->i_height = height; //set frame height
		param->rc.i_lookahead = 0;
		param->i_bframe = 0;
		param->i_fps_num = 12;
		param->i_fps_den = 1;
		handle = x264_encoder_open( param );

		/* Create a new pic */
		x264_picture_alloc( picture, X264_CSP_I420, param->i_width, param->i_height );

		pFile = fopen( path, "wb" );
	}

	H264Writer::~H264Writer()
	{
		if( picture )
		{
			x264_picture_clean( picture );
			free( picture );
			picture = NULL;
		}
		if( param )
		{
			free( param );
			param = NULL;
		}
		if( handle )
		{
			x264_encoder_close( handle );
			handle = NULL;
		}

		if( pFile != NULL )
		{
			fclose( pFile );
			pFile = NULL;
		}
	}

	int H264Writer::writeVideo( unsigned char* buffer )
	{
		x264_picture_t pic_out;
		int nNal = -1;
		int i = 0;
		int nPicSize = param->i_width * param->i_height;

		//YUV420的数据
		//memcpy( picture->img.plane[ 0 ], buffer, nPicSize );//Y数据
		//memcpy( picture->img.plane[ 1 ], buffer + nPicSize, nPicSize / 2 );//UV数据

		unsigned char* y = picture->img.plane[ 0 ];
		unsigned char* v = picture->img.plane[ 1 ];
		unsigned char* u = picture->img.plane[ 2 ];

		memcpy( picture->img.plane[ 0 ], buffer, nPicSize );//将YUV420SP转换为YUV420
		for (i = 0; i < nPicSize/4; i++)
		{
			*(u+i) = *(buffer + nPicSize + i*2);
			*(v+i) = *(buffer + nPicSize + i*2 + 1);
		}

		picture->i_type = X264_TYPE_AUTO;
		if( x264_encoder_encode( handle, &nal, &nNal, picture ,&pic_out ) < 0 )//将YUV420数据进行H264编码
		{
			return -1;
		}

		for( i = 0; i < nNal; i++ )
		{
			fwrite( nal[ i ].p_payload, nal[ i ].i_payload, 1, pFile );//将编码后的码流写入文件
		}

		//return 0;
		return nal[ i ].i_payload;
	}
}
