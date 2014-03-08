#ifndef _OCULUS_RIFT_H_
#define _OCULUS_RIFT_H_

#include <ovr/Include/OVR.h>

using namespace OVR;
using namespace OVR::Util::Render;

#include "pv/types.h"

class OculusRift
{
public:
	/**
	 * This is the constructor used to create a new Oculus Rift device.
	 *
	 * This constructor will automatically attempt to initialize and setup an
	 * Oculus Rift device that is connected to the computer.  Use the IsConnected
	 * method to see if the device was successfully setup.
	 */
	OculusRift();
	/**
	 * This method will initialize the Oculus Rift headset.
	 *
	 * This method goes through and connect to the Oculus Rift hardware.  It then retrieves
	 * the sensor, as well as a sensor fusion, both of which can be used to retrieve data
	 * from the Oculus Rift.
	 */
	void Initialize();
	/**
	 * This method will setup the Oculus Rift headset.
	 *
	 * This method goes through and sets up the user's data for the Oculus Rift headset.
	 * This data includes things such as the interpupillary distance, field of view, and more.
	 */
	void Setup();

	/**
	 * Returns a boolean indicating whether the Oculus Rift is connected.
	 *
	 * This method checks to see if the Oculus Rift is connected and returns
	 * a boolean indicating whether it is or not.
	 * @return Returns true if the Oculus Rift is connected, false otherwise.
	 */
	const bool isConnected();

	/**
	 * This method updates the data from the Oculus Rift headset.
	 *
	 * This method updates data recieved from the Oculus Rift.  It currently pulls the change in
	 * orientation and updates the rotation of where the user is looking.
	 */
	void Update();

	/**
	 * Shifts the view for a specific eye.
	 *
	 * Shifts the view for a specific eye.  This will use OpenGL specifically right now, and will
	 * make sure to use the best methods for the supported version of OpenGL.
	 */
	void ShiftView(RiftEye eye);

	/**
	 * Shifts the view for a specific eye.
	 *
	 * Shifts the view for a specific eye.  This will use OpenGL specifically right now, and allows
	 * you to specify which version of OpenGL to render with. (Shaders for > 2.0, deprecated methods for
	 * less than 2.0.
	 */
	void ShiftView(RiftEye eye, int majorVersion, int minorVersion);

	/**
	 * Get the rotation values for where the user is looking.
	 *
	 * Get the rotation values for the angle of rotation for where the user is looking.
	 * This is in Euler angles.
	 * @return The rotation on the X, Y, and Z axis in Euler angles.
	 */
	const rotation_t getRotation();

	/**
	 * This is the deconstructor for the Oculus Rift device.
	 *
	 * This is the deconstructor for he Oculus Rift device.  This method will automatically
	 * cleanup all resources associated with the Oculus Rift device.
	 */
	~OculusRift();
protected:
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
	orientation_t Orientation;
	/**
	  * The orientation of the oculus rift. (Quaternion)
	  */
	orientation_quart_t Orientation_quart;
	/**
	  * The previous orientation of the oculus rift. (yaw, pitch, roll)
	  *
	  * The previous orientation of the oculus rift. (yaw, pitch, roll) This is used to
	  * update the orientation data properly.
	  */
	orientation_t OldOrientation;
	/**
	  * The previous orientation of the oculus rift. (Quaternion)
	  *
	  * The previous orientation of the oculus rift. (Quaternion) This is used to
	  * update the orientation data properly.
	  */
	orientation_quart_t OldOrientation_quart;

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

	/**
	 * The rotation data for where the user is looking.
	 */
	rotation_t Rotation;

	/**
	 * Renders the view for the Oculus Rift with OpenGL using methods compatible with < Version 2.0.
	 *
	 * Renders to the screen for the Oculus Rift's eyes using OpenGL.  This is the compatiblity version
	 * to be used on OpenGL versions less than 2.0.
	 * @param eye The eye to render for.
	 */
	void renderGLBelow2(RiftEye eye);
};

#endif