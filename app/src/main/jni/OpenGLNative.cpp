#include <stdio.h>
#include <assert.h>
#include <jni.h>

#include "OpenGL.h"

using namespace KugouPlayer;

struct fields_t
{
	jclass clazz;
	jfieldID context;
};

static struct fields_t fields;

static const char* const kClassPathName = "com/kugou/openglcamera/OpenGLNative";

OpenGL* getOpenGL( JNIEnv* env, jobject thiz )
{
    return reinterpret_cast< OpenGL* >( env->GetLongField( thiz, fields.context ) );
}

static void setOpenGL( JNIEnv* env, jobject thiz, OpenGL* player )
{
    env->SetLongField( thiz, fields.context, reinterpret_cast< jlong >( player ) );
}

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in KugouPlayer, which won't run until the
// first time an instance of this class is used.
static void opengl_native_native_init( JNIEnv *env )
{
    jclass clazz = env->FindClass( kClassPathName );
    if( clazz == NULL )
    {
       return;
    }
	fields.clazz = static_cast< jclass >( env->NewGlobalRef( clazz ) );

    fields.context = env->GetFieldID( fields.clazz, "mNativeContext", "J" );
    if( fields.context == NULL )
    {
       return;
    }
}

static void opengl_native_init( JNIEnv* env, jobject obj )
{
	OpenGL* gl = new OpenGL();//为OpenGL锟洁动态锟斤拷锟斤拷一锟斤拷锟节达拷
	if( gl != NULL )
	{
		setOpenGL( env, obj, gl );//锟斤拷锟斤拷锟斤拷锟斤拷系统锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	}
}

//锟叫伙拷锟斤拷锟斤拷锟斤拷锟�
static void opengl_native_surfaceChange( JNIEnv* env, jobject obj, jint width, jint height )
{
	OpenGL* gl = getOpenGL( env, obj );//锟斤拷系统锟斤拷锟斤拷锟叫伙拷取OpenGL锟斤拷锟街革拷锟�
	if( gl != NULL )
	{
		gl->surfaceChange( width, height );
	}
}

//锟侥憋拷锟剿撅拷锟斤拷锟斤拷,锟斤拷锟侥革拷锟斤拷锟斤拷gaussfilter为锟角凤拷使锟矫革拷斯锟剿诧拷
static void opengl_native_changeFilterType( JNIEnv* env, jobject obj, jint type, jint gaussfilter )
{
	OpenGL* gl = getOpenGL( env, obj );
	if( gl != NULL )
	{
		gl->setFilterType( type, ( gaussfilter == 1 ) ? true : false );
	}
}

//锟斤拷转
static void opengl_native_setRotation( JNIEnv* env, jobject obj, jint degrees, jint flipHorizontal, jint flipVertical )
{
	OpenGL* gl = getOpenGL( env, obj );
	if( gl != NULL )
	{
		gl->setRotation( degrees, flipHorizontal, flipVertical );
	}
}

//锟斤拷染(也锟斤拷锟斤拷锟角匡拷始锟斤拷直锟斤拷锟斤拷锟斤拷锟较斤拷锟叫伙拷锟斤拷指锟斤拷锟斤拷锟剿撅拷效锟斤拷)
static void opengl_native_render( JNIEnv* env, jobject obj, jbyteArray array, jint widthTexture, jint heightTexture )
{
	OpenGL* gl = getOpenGL( env, obj );
	if( gl != NULL )
	{
		if( array != NULL )
		{
			jbyte* pBuff = env->GetByteArrayElements( array, 0 );

			gl->render( ( unsigned char* )pBuff, widthTexture, heightTexture );

			/*
			 * 锟斤拷染锟斤拷锟街拷锟�,锟斤拷锟斤拷要锟酵凤拷锟节达拷
			 */
			if( pBuff )
			{
				env->ReleaseByteArrayElements( array, pBuff, JNI_ABORT );
			}
		}
		else
		{
			gl->render( NULL, widthTexture, heightTexture );
		}
	}
}

//锟斤拷始录锟斤拷
static void opengl_native_startRecord( JNIEnv* env, jobject obj, jstring path, jint width, jint height )
{
	OpenGL* gl = getOpenGL( env, obj );
	if( gl != NULL )
	{
		const char *str = env->GetStringUTFChars( path, NULL );

		gl->startRecord( str, width, height );

		env->ReleaseStringUTFChars( path, str );
	}
}

//停止录锟斤拷
static void opengl_native_stopRecord( JNIEnv* env, jobject obj )
{
	OpenGL* gl = getOpenGL( env, obj );
	if( gl != NULL )
	{
		gl->stopRecord();
	}
}

/*
 * 锟结构锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷峁╋拷锟斤拷喜锟斤拷锟矫的接口和猴拷锟斤拷锟斤拷锟斤拷,锟斤拷锟节猴拷锟斤拷锟斤拷锟较低匙拷锟�
 */
static JNINativeMethod gMethods[] =
{
	{ "native_init",             "()V",                        ( void* )opengl_native_native_init },
    { "init",                         "()V",                        ( void* )opengl_native_init },
	{ "surfaceChange",      "(II)V",                      ( void* )opengl_native_surfaceChange },
	{ "setRotation",           "(III)V",                     ( void* )opengl_native_setRotation },
    { "changeFilterType",  "(II)V",                      ( void* )opengl_native_changeFilterType },
	{ "render",                   "([BII)V",                    ( void* )opengl_native_render },
	{ "startRecord",           "(Ljava/lang/String;II)V",    ( void* )opengl_native_startRecord },
	{ "stopRecord",           "()V",                        ( void* )opengl_native_stopRecord },
};

//锟较诧拷锟斤拷氐锟斤拷锟节猴拷锟斤拷
jint JNI_OnLoad( JavaVM* vm, void* reserved )
{
    JNIEnv* env = NULL;
    jclass clazz;
    jint result = -1;

    LOGE("JNI OnLoad in\r\n");
    //锟斤拷锟较低筹拷锟斤拷锟�
    if( vm->GetEnv( ( void** )&env, JNI_VERSION_1_4 ) != JNI_OK )
    {
       return result;
    }
    assert( env != NULL );

    //锟斤拷夤わ拷锟铰凤拷锟斤拷锟斤拷诘锟斤拷锟�
    clazz = env->FindClass( kClassPathName );
    if( clazz == NULL )
    {
        return result;
    }
    /*
              注锟斤拷锟较诧拷锟斤拷玫慕涌诤锟斤拷锟�
     */
    if( env->RegisterNatives( clazz, gMethods, sizeof( gMethods ) / sizeof( gMethods[ 0 ] ) ) < 0 )
    {
        return result;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    LOGE("JNI OnLoad out\r\n");
    return result;
}
