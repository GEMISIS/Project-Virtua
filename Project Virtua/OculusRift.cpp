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
	// Set the viewport to the max of the Oculus Rift currently (1280x800).
	this->viewport = Viewport(0, 0, 1280, 800);
	// Initialize the Oculus Rift.
	this->Initialize();
	// Setup the user's data for the Oculus Rift.
	this->Setup();
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
 * This is the deconstructor for the Oculus Rift device.
 *
 * This is the deconstructor for he Oculus Rift device.  This method will automatically
 * cleanup all resources associated with the Oculus Rift device.
 */
OculusRift::~OculusRift()
{
	//System::Destroy();
}