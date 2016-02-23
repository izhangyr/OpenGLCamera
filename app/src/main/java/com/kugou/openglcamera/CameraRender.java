package com.kugou.openglcamera;

import java.util.LinkedList;
import java.util.Queue;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView.Renderer;
import android.os.SystemClock;

public class CameraRender implements Renderer, PreviewCallback
{
	private final Queue< byte [] > mFromCameraBuffer;

	private final Queue< byte [] > mToCameraBuffer;

	private final Queue< Runnable > mRunOnDraw;
	
	private OpenGLNative mOpenGL = null;

	private int mDegress;
	private boolean mFlipHorizontal;
	private boolean mFlipVertical;

	private int mFilterType = 0;
	private boolean mGaussFilterFlag = false;

	// private int mOutputWidth;
	// private int mOutputHeight;
	private int mImageWidth;
	private int mImageHeight;

	private SurfaceTexture mSurfaceTexture = null;

	public CameraRender()
	{
		mFromCameraBuffer = new LinkedList< byte [] >();
		mToCameraBuffer = new LinkedList< byte [] >();

		mRunOnDraw = new LinkedList< Runnable >();
	}

	@Override
	public void onDrawFrame( GL10 gl )
	{
		synchronized ( mRunOnDraw )
		{
			while ( !mRunOnDraw.isEmpty() )
			{
				mRunOnDraw.poll().run();
			}
		}

		byte [] pByte = null;
		synchronized ( this )
		{
			if ( mFromCameraBuffer.isEmpty() )
			{
				if( mOpenGL != null )
				{
					mOpenGL.render( null, mImageWidth, mImageHeight );
				}
				return;
			}
			pByte = mFromCameraBuffer.poll();
		}

		long before = SystemClock.elapsedRealtime();
		if( mOpenGL != null )
		{
			mOpenGL.render( pByte, mImageWidth, mImageHeight );
		}
		before = SystemClock.elapsedRealtime() - before;
//		Log.e( "abc", "render time:" + before );

		synchronized ( this )
		{
			mToCameraBuffer.add( pByte );
		}
	}

	@Override
	public void onSurfaceChanged( GL10 gl, int width, int height )
	{
		if( mOpenGL != null )
		{
			mOpenGL.surfaceChange( width, height );
		}
	}

	@Override
	public void onSurfaceCreated( GL10 gl, EGLConfig config )
	{
		mOpenGL = new OpenGLNative();
		if( mOpenGL != null )
		{
			mOpenGL.setRotation( mDegress, mFlipHorizontal ? 1 : 0, mFlipVertical ? 1 : 0 );
		}
	}

	@Override
	public void onPreviewFrame( byte [] data, Camera camera )
	{
		synchronized ( this )
		{
			if ( mFromCameraBuffer.isEmpty() )
			{
				mFromCameraBuffer.add( data );
			}
			else
			{
				camera.addCallbackBuffer( data );
			}

			byte [] bytesFromRenderer = null;

			if ( mToCameraBuffer.isEmpty() )
			{
				return;
			}
			bytesFromRenderer = mToCameraBuffer.poll();
			camera.addCallbackBuffer( bytesFromRenderer );
		}
	}

	public void setImageSize( final int width, final int height )
	{
		mImageWidth = width;
		mImageHeight = height;
	}

	public void setRotation( final int degrees, final boolean flipHorizontal, final boolean flipVertical )
	{
		mDegress = degrees;
		mFlipHorizontal = flipHorizontal;
		mFlipVertical = flipVertical;
		if( mOpenGL != null )
		{
			mOpenGL.setRotation( degrees, flipHorizontal ? 1 : 0, flipVertical ? 1 : 0 );
		}
	}

	public SurfaceTexture getSurfaceTexture()
	{
		if ( mSurfaceTexture == null )
		{
			int [] textures = new int [ 1 ];

			// generate one texture pointer and bind it as an
			// external
			// texture.
			GLES20.glGenTextures( 1, textures, 0 );
			GLES20.glBindTexture( GLES20.GL_TEXTURE_2D, textures[ 0 ] );

			// No mip-mapping with camera source.
			GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST );
			GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST );

			// Clamp to edge is only option.
			GLES20.glTexParameteri( GLES20.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE );
			GLES20.glTexParameteri( GLES20.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE );

			mSurfaceTexture = new SurfaceTexture( textures[ 0 ] );
		}
		return mSurfaceTexture;
	}
	
	public int getFilterType()
	{
		return mFilterType;
	}

	public void changeFilterType( final int type )
	{
		mFilterType = type;
		
		if( mOpenGL != null )
		{
			mOpenGL.changeFilterType( mFilterType, mGaussFilterFlag ? 1 : 0 );
		}
	}
	
	public boolean getGaussFilterFlag()
	{
		return mGaussFilterFlag;
	}

	public void setGaussFilterFlag( final boolean flag )
	{
		mGaussFilterFlag = flag;
		if( mOpenGL != null )
		{
			mOpenGL.changeFilterType( mFilterType, mGaussFilterFlag ? 1 : 0 );
		}
	}
	
	public void startRecord( String filePath )
	{
		if( mOpenGL != null )
		{
			mOpenGL.startRecord( filePath, mImageWidth, mImageHeight );
		}
	}
	
	public void stopRecord()
	{
		if( mOpenGL != null )
		{
			mOpenGL.stopRecord();
		}
	}
}
