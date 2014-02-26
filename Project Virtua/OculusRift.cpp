#include "OculusRift.h"

/**
 * This is the constructor used to create a new Oculus Rift device.
 *
 * This constructor will automatically attempt to initialize and setup an
 * Oculus Rift device that is connected to the computer.  Use the IsConnected
 * method to see if the device was successfully setup.
 */
OculusRift::OculusRift()
{
	// Setup the initial values.
	this->Rotation.x = 0.0f;
	this->Rotation.y = 0.0f;
	this->Rotation.z = 0.0f;
	this->Rotation.w = 0.0f;

	this->Orientation.yaw = 0.0f;
	this->Orientation.pitch = 0.0f;
	this->Orientation.roll = 0.0f;

	this->OldOrientation.yaw = 0.0f;
	this->OldOrientation.pitch = 0.0f;
	this->OldOrientation.roll = 0.0f;

	this->Orientation_quart.angle = 0.0f;
	this->Orientation_quart.axis.x = 0.0f;
	this->Orientation_quart.axis.y = 0.0f;
	this->Orientation_quart.axis.z = 0.0f;

	this->OldOrientation_quart.angle = 0.0f;
	this->OldOrientation_quart.axis.x = 0.0f;
	this->OldOrientation_quart.axis.y = 0.0f;
	this->OldOrientation_quart.axis.z = 0.0f;

	// Set the viewport to the max of the Oculus Rift currently (1280x800).
	this->viewport = Viewport(0, 0, 1280, 800);
	// Initialize the Oculus Rift.
	this->Initialize();

	if(this->connected)
	{
		// Setup the user's data for the Oculus Rift.
		this->Setup();
	}
}

/**
 * This method will initialize the Oculus Rift headset.
 *
 * This method goes through and connect to the Oculus Rift hardware.  It then retrieves
 * the sensor, as well as a sensor fusion, both of which can be used to retrieve data
 * from the Oculus Rift.
 */
void OculusRift::Initialize()
{
	// Check if we are debugging or not.
#ifdef _DEBUG
	// If so, log all interactions with the oculus rift.
	System::Init(Log::ConfigureDefaultLog(LogMask_All));
#else
	// Otherwise, log no interactions with the oculus rift.
	System::Init(Log::ConfigureDefaultLog(LogMask_None));
#endif

	// Then create a device manager.
	this->deviceManager = *DeviceManager::Create();
	// Then get the oculus rift's HMD device.
	this->HMDHardwareDevice = *this->deviceManager->EnumerateDevices<HMDDevice>().CreateDevice();

	// Set the boolean indicating that there is an oculus rift to false.
	this->connected = false;

	// Check if the oculus rift device was successfully gotten.
	if(this->HMDHardwareDevice)
	{
		// If so, set the oculus rift's HMD object.
		if(this->HMDHardwareDevice->GetDeviceInfo(&HMD))
		{
			// Then get the oculus rift sensor.
			this->Sensor = *this->HMDHardwareDevice->GetSensor();

			// Check that the sensor was successfully gotten.
			if (this->Sensor)
			{
				// Then attach the sensor to a sensor fusion object.
				this->sensorFusion = new SensorFusion(this->Sensor);
				// Finally, set the boolean indicating that the oculus rift was gotten to true.
				this->connected = true;
			}
		}
	}
}

/**
 * This method will setup the Oculus Rift headset.
 *
 * This method goes through and sets up the user's data for the Oculus Rift headset.
 * This data includes things such as the interpupillary distance, field of view, and more.
 */
void OculusRift::Setup()
{
	/**
	  * Set the aspect ration.  Note that this has to be statically converted, as the normal
	  * data type is an unsigned integer for the screen resolution values.
	  */
	this->AspectRatio = static_cast<float>(this->HMD.HResolution * 0.5f)
		/ static_cast<float>(this->HMD.VResolution);
	/**
	  * The aspect ratio is determined based on the distance between the eyes and
	  * the size of the screen.  This is then converted to degrees for general use.
	  */
	this->FieldOfView = RadToDegree(2.0f * 
		atan((this->HMD.VScreenSize / 2.0f) / this->HMD.EyeToScreenDistance));

	// Get the center of the viewing screen.
	this->ViewCenter = this->HMD.HScreenSize * 0.25f;
	// Get the offest for the screen's center.
	this->ProjectionCenterOffset = 4.0f * (this->ViewCenter - (this->HMD.LensSeparationDistance * 0.5f))
		/ this->HMD.HScreenSize;
	// Get the distance between the eyes and halve it.
	this->HalfIPD = this->HMD.InterpupillaryDistance * 0.5f;

	// Set the viewport for what both eyes can see.
	this->StereoConfiguration.SetFullViewport(this->viewport);
	// Set the stereo mode to cache.
	this->StereoConfiguration.SetStereoMode(Stereo_LeftRight_Multipass);
	// Set which HMD to use.
	this->StereoConfiguration.SetHMDInfo(this->HMD);
	// Set the distortion scaling values.
	this->StereoConfiguration.SetDistortionFitPointVP(-1.0f, 0.0f);

	// Get the rendering properties for the left eye.
	this->LeftEye = this->StereoConfiguration.GetEyeRenderParams(StereoEye_Left);
	// Get the rendering properties for the right eye.
	this->RightEye = this->StereoConfiguration.GetEyeRenderParams(StereoEye_Right);
}

/**
 * Returns a boolean indicating whether the Oculus Rift is connected.
 *
 * This method checks to see if the Oculus Rift is connected and returns
 * a boolean indicating whether it is or not.
 * @return Returns true if the Oculus Rift is connected, false otherwise.
 */
const bool OculusRift::isConnected()
{
	return (const bool)this->connected;
}

/**
 * This method updates the data from the Oculus Rift headset.
 *
 * This method updates data recieved from the Oculus Rift.  It currently pulls the change in
 * orientation and updates the rotation of where the user is looking.
 */
void OculusRift::Update()
{
	if(this->connected)
	{
		this->OldOrientation.yaw = this->Orientation.yaw;
		this->OldOrientation.pitch = this->Orientation.pitch;
		this->OldOrientation.roll = this->Orientation.roll;

		this->sensorFusion->GetOrientation().GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&this->Orientation.yaw, &this->Orientation.pitch, &this->Orientation.roll);

		this->Orientation.yaw = RadToDegree(this->Orientation.yaw);
		this->Orientation.pitch = RadToDegree(this->Orientation.pitch);
		this->Orientation.roll = RadToDegree(this->Orientation.roll);

		this->Rotation.x -= (this->Orientation.pitch - this->OldOrientation.pitch);
		this->Rotation.y -= (this->Orientation.yaw - this->OldOrientation.yaw);
		this->Rotation.z -= (this->Orientation.roll - this->OldOrientation.roll);

		this->OldOrientation_quart.angle = this->Orientation_quart.angle;
		this->OldOrientation_quart.axis.x = this->Orientation_quart.axis.x;
		this->OldOrientation_quart.axis.y = this->Orientation_quart.axis.y;
		this->OldOrientation_quart.axis.z = this->Orientation_quart.axis.z;

		this->sensorFusion->GetOrientation().GetAxisAngle(&this->Orientation_quart.axis, &this->Orientation_quart.angle);
		this->Orientation_quart.angle = -RadToDegree(this->Orientation_quart.angle);
	}
}

void ShiftEye(RiftEye eye)
{
}

/**
 * Get the rotation values for where the user is looking.
 *
 * Get the rotation values for the angle of rotation for where the user is looking.
 * This is in Euler angles.
 * @return The rotation on the X, Y, and Z axis in Euler angles.
 */
const rotation_t OculusRift::getRotation()
{
	return (const rotation_t)this->Rotation;
}


/**
 * This is the deconstructor for the Oculus Rift device.
 *
 * This is the deconstructor for he Oculus Rift device.  This method will automatically
 * cleanup all resources associated with the Oculus Rift device.
 */
OculusRift::~OculusRift()
{
	//System::Destroy();
}