#ifndef _OCULUS_RIFT_H_
#define _OCULUS_RIFT_H_

#include <ovr/Include/OVR.h>

using namespace OVR;
using namespace OVR::Util::Render;

/**
 * This is a structure used for the orientation of an Oculus Rift headset.
 *
 * This is a structure used for the orientation of an Oculus Rift headset.  The
 * angles are stored as Eular angles.
 */
struct orientation_t
{
	/**
	 * Rotation on the Y axis.
	 */
	float yaw;
	/**
	 * Rotation on the X axis.
	 */
	float pitch;
	/**
	 * Rotation on the Z axis.
	 */
	float roll;
};

/**
 * This is a structure used for the orientation of an Oculus Rift headset.
 *
 * This is a structure used for the orientation of an Oculus Rift headset.  The
 * angles are stored as Quartonian angles.
 */
struct orientation_quart_t
{
	/**
	 * A vector with X, Y, and Z variables respresenting the axis as normals that
	 * is being roated around.
	 */
	Vector3f axis;
	/**
	 * The angle of rotation for this orientation.
	 */
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
	orientation_quart_t OculusRiftOrientation_quart;
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
	 * This is the deconstructor for the Oculus Rift device.
	 *
	 * This is the deconstructor for he Oculus Rift device.  This method will automatically
	 * cleanup all resources associated with the Oculus Rift device.
	 */
	~OculusRift();
};

#endif