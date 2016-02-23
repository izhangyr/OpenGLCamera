package com.kugou.openglcamera;

import java.io.IOException;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLSurfaceView;

public class CameraView
{
//	private final Context mContext;
	private final CameraRender mRenderer;
	private GLSurfaceView mGlSurfaceView = null;

	public CameraView( final Context context )
	{
		if ( !supportsOpenGLES2( context ) )
		{
			throw new IllegalStateException( "OpenGL ES 2.0 is not supported on this phone." );
		}

//		mContext = context;
		mRenderer = new CameraRender();
	}

	private boolean supportsOpenGLES2( final Context context )
	{
		final ActivityManager activityManager = ( ActivityManager ) context
				.getSystemService( Context.ACTIVITY_SERVICE );
		final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
		return configurationInfo.reqGlEsVersion >= 0x20000;
	}

	public void setGLSurfaceView( final GLSurfaceView view )
	{
		mGlSurfaceView = view;
		mGlSurfaceView.setEGLContextClientVersion( 2 );
		mGlSurfaceView.setRenderer( mRenderer );
		mGlSurfaceView.setRenderMode( GLSurfaceView.RENDERMODE_WHEN_DIRTY );
		mGlSurfaceView.requestRender();
	}

	public void setUpCamera( final Camera camera )
	{
		setUpCamera( camera, 0, false, false );
	}

	public void setUpCamera( final Camera camera, final int degrees, final boolean flipHorizontal,
			final boolean flipVertical )
	{
		mGlSurfaceView.setRenderMode( GLSurfaceView.RENDERMODE_CONTINUOUSLY );
		try
		{
			camera.setPreviewTexture( mRenderer.getSurfaceTexture() );
		}
		catch ( IOException e )
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		camera.setPreviewCallback( mRenderer );
		camera.startPreview();

		Camera.Parameters newParams = camera.getParameters();
		Size sizeCamera = newParams.getPreviewSize();

		mRenderer.setImageSize( sizeCamera.width, sizeCamera.height );

		mRenderer.setRotation( degrees, flipHorizontal, flipVertical );
	}
	
	public int getFilterType()
	{
		return mRenderer.getFilterType();
	}

	public void changeFilterType( final int type )
	{
		mRenderer.changeFilterType( type );
	}
	
	public boolean getGaussFilterFlag()
	{
		return mRenderer.getGaussFilterFlag();
	}

	public void setGaussFilterFlag( final boolean flag )
	{
		mRenderer.setGaussFilterFlag( flag );
	}
	
	public void startRecord( String filePath )
	{
		mRenderer.startRecord( filePath );
	}
	
	public void stopRecord()
	{
		mRenderer.stopRecord();
	}
}
