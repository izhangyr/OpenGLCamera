package com.kugou.openglcamera;

import java.util.List;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.util.Log;

public class CameraLoader
{
	private int mCurrentCameraId = 0;

	private Camera mCameraInstance = null;

	private CameraHelper mCameraHelper = null;

	private CameraView mCameraView = null;

	private Activity mActivity = null;

	private int mPreviewWidth = 320;

	private int mPreviewHeight = 240;

	public CameraLoader( int cameraid, Activity activity, CameraHelper cameraHelper, CameraView cameraView )
	{
		mCurrentCameraId = cameraid;
		mActivity = activity;
		mCameraHelper = cameraHelper;
		mCameraView = cameraView;
	}

	public void onResume()
	{
		setUpCamera( mActivity, mCurrentCameraId );
	}

	public void onPause()
	{
		releaseCamera();
	}

	public void setPreviewSize( int width, int height )
	{
		mPreviewWidth = width;
		mPreviewHeight = height;
	}

	public void switchCamera()
	{
		releaseCamera();
		mCurrentCameraId = ( mCurrentCameraId + 1 ) % mCameraHelper.getNumberOfCameras();
		setUpCamera( mActivity, mCurrentCameraId );
	}

	@SuppressLint("InlinedApi")
	private void setUpCamera( final Activity activity, final int id )
	{
		mCameraInstance = getCameraInstance( id );

		Parameters parameters = mCameraInstance.getParameters();
		parameters.setPreviewSize( mPreviewWidth, mPreviewHeight );
		if ( parameters.getSupportedFocusModes().contains( Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE ) )
		{
			parameters.setFocusMode( Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE );
		}
		parameters.setPreviewFormat( ImageFormat.NV21 );
//		Log.e( "abc", "max zoom:" + parameters.getMaxZoom() );
//		parameters.setZoom( 10 );
//		parameters.setPreviewFpsRange( 9, 10 );
		mCameraInstance.setParameters( parameters );

		Camera.Parameters newParams = mCameraInstance.getParameters();
		Size sizeCamera = newParams.getPreviewSize();

		int alignedWidth = ( ( sizeCamera.width + 15 ) / 16 ) * 16;
		int alignedHeight = ( ( sizeCamera.height + 15 ) / 16 ) * 16;

		int bufferSize = 3 * alignedWidth * alignedHeight / 2;
		for ( int i = 0; i < 3; i++ )
		{
			mCameraInstance.addCallbackBuffer( new byte [ bufferSize ] );
		}

		int orientation = mCameraHelper.getCameraDisplayOrientation( activity, mCurrentCameraId );
		CameraInfo cameraInfo = new CameraInfo();
		mCameraHelper.getCameraInfo( mCurrentCameraId, cameraInfo );
		boolean flipVertical = cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT ? true : false;

		mCameraView.setUpCamera( mCameraInstance, orientation, false, flipVertical );
	}

	/** A safe way to get an instance of the Camera object. */
	private Camera getCameraInstance( final int id )
	{
		Camera c = null;
		try
		{
			c = mCameraHelper.openCamera( id );
		}
		catch ( Exception e )
		{
			e.printStackTrace();
		}
		return c;
	}

	private void releaseCamera()
	{
		mCameraInstance.setPreviewCallback( null );
		mCameraInstance.release();
		mCameraInstance = null;
	}
}
