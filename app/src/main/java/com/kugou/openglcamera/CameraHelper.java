package com.kugou.openglcamera;

import android.app.Activity;
import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.view.Surface;

public class CameraHelper
{
	public CameraHelper( Context context )
	{

	}

	public int getNumberOfCameras()
	{
		return Camera.getNumberOfCameras();
	}

	public Camera openCamera( final int id )
	{
		return Camera.open( id );
	}

	public Camera openDefaultCamera()
	{
		return Camera.open();
	}

	public Camera openFrontCamera()
	{
		return Camera.open( getCameraId( CameraInfo.CAMERA_FACING_FRONT ) );
	}

	public Camera openBackCamera()
	{
		return Camera.open( getCameraId( CameraInfo.CAMERA_FACING_BACK ) );
	}

	public int hasFrontCamera()
	{
		return getCameraId( CameraInfo.CAMERA_FACING_FRONT );
	}

	public int hasBackCamera()
	{
		return getCameraId( CameraInfo.CAMERA_FACING_BACK );
	}

	public void getCameraInfo( final int cameraId, final CameraInfo cameraInfo )
	{
		Camera.getCameraInfo( cameraId, cameraInfo );
	}

	public void setCameraDisplayOrientation( final Activity activity, final int cameraId, final Camera camera )
	{
		int result = getCameraDisplayOrientation( activity, cameraId );
		camera.setDisplayOrientation( result );
	}

	public int getCameraDisplayOrientation( final Activity activity, final int cameraId )
	{
		int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
		int degrees = 0;
		switch ( rotation )
		{
		case Surface.ROTATION_0:
			degrees = 0;
			break;
		case Surface.ROTATION_90:
			degrees = 90;
			break;
		case Surface.ROTATION_180:
			degrees = 180;
			break;
		case Surface.ROTATION_270:
			degrees = 270;
			break;
		}

		int result;
		CameraInfo info = new CameraInfo();
		getCameraInfo( cameraId, info );
		if ( info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT )
		{
			result = ( info.orientation + degrees ) % 360;
		}
		else
		{ // back-facing
			result = ( info.orientation - degrees + 360 ) % 360;
		}
		return result;
	}

	private int getCameraId( final int facing )
	{
		int numberOfCameras = Camera.getNumberOfCameras();
		CameraInfo info = new CameraInfo();
		for ( int id = 0; id < numberOfCameras; id++ )
		{
			Camera.getCameraInfo( id, info );
			if ( info.facing == facing )
			{
				return id;
			}
		}
		return -1;
	}
}
