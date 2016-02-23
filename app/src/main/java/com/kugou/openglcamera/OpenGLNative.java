package com.kugou.openglcamera;

public class OpenGLNative
{
	static
	{
		System.loadLibrary( "x264" );
		System.loadLibrary( "openglcamera" );
		
		native_init();
	}

	private long mNativeContext; // accessed by native methods

	public OpenGLNative()
	{
		init();
	}
	
	public native static void native_init();
	
	public  native void init();

	public  native void surfaceChange( int width, int height );

	public  native void setRotation( int degrees, int flipHorizontal, int flipVertical );
	
	public native void changeFilterType( int type, int gaussfilter );

	public  native void render( byte[] array, int width, int height );
	
	public native void startRecord( String filePath, int width, int height );
	
	public native void stopRecord();
}
