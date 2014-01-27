#ifndef _OCULUS_RIFT_H_
#define _OCULUS_RIFT_H_

#include <ovr/Include/OVR.h>

using namespace OVR;
using namespace OVR::Util::Render;

struct orientation_t
{
	float yaw, pitch, roll;
};

struct orentation_quart_t
{
	Vector3f axis;
	float angle;
};

class OculusRift
{
public:
	/**
	  * Manages the USB devices connected to the computer.
	  */
	Ptr<DeviceManager> deviceManager;
	/**
	  * Manages a Head Mounted Display (HMD) device (IE: The oculus rift).  This is used
	  * mainly for getting hardware information rather than user info (IE: No eye distance info).
	  */
	Ptr<HMDDevice> HMDHardwareDevice;
	/**
	  * The actual oculus rift sensor.  Can be used to get various things from the actual
	  * device once created.
	  */
	Ptr<SensorDevice> Sensor;
	/**
	  * The oculus rift's information in the form of a Head Mounted Display (HMD).  This is used
	  * to get information specially needed for HMD's, rather than hardware information.
	  */
	HMDInfo HMD;
	/**
	  * The fusion sensor object is used to get information from the HDM's built in sensors.
	  * (IE: Accelerometer information, orientation, etc.)
	  */
	SensorFusion* sensorFusion;

	/**
	  * A boolean indicating whether an oculus rift is connected or not.
	  */
	bool connected;

	/**
	  * The orientation of the oculus rift. (yaw, pitch, roll)
	  */
	orientation_t OculusRiftOrientation;
	/**
	  * The orientation of the oculus rift. (Quaternion)
	  */
	orentation_quart_t OculusRiftOrientation_quart;
	/**
	  * The configuration for the stereo-scopic viewing.  Contains properties like how big
	  * the display for each eye is.
	  */
	StereoConfig StereoConfiguration;
	/**
	  * The stereo parameters for the left eye.
	  */
	StereoEyeParams LeftEye;
	/**
	  * The stereo parameters for the right eye.
	  */
	StereoEyeParams RightEye;
	/**
	  * The center viewing point.
	  */
	float ViewCenter;
	/**
	  * The offset for the projection view due to the eyes and screen size.
	  */
	float ProjectionCenterOffset;
	/**
	  * This is half of the interpupillary distance (IE: Distance between the eyes).
	  */
	float HalfIPD;

	/**
	  * The field of view on the Y-axis that can be seen by the camera.
	  */
	float FieldOfView;
	/**
	  * The aspect ration to display with the camera.
	  */
	float AspectRatio;

	/**
	 * The viewport for what the user's eyes can see.
	 */
	Viewport viewport;

	OculusRift();
	void Initialize();
	void Setup();
	~OculusRift();
};

#endif