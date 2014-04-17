#include "pv/OculusRift.h"
#include "pv/MinOpenGL.h"
#include "pv/FileIO.h"
#include "Kernel/OVR_Math.h"

namespace PV
{
	void InitRift()
	{
		// Check if we are debugging or not.
		ovr_Initialize();
	}
	bool DetectDevice()
	{
		InitRift();
		bool active = false;
		return active;
	}
	/**
 * This is the constructor used to create a new Oculus Rift device.
 *
 * This constructor will automatically attempt to initialize and setup an
 * Oculus Rift device that is connected to the computer.  Use the IsConnected
 * method to see if the device was successfully setup.
 */
	OculusRift::OculusRift(bool useDemoRift, HGLRC openGlContext, HWND window, HDC gdiDc)
	{
		// Setup the initial values for all of the rotations.
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

		this->virtuallyConnected = false;

		// Initialize the Oculus Rift.
		this->Initialize();

		if (this->connected)
		{
			// Setup the user's data for the Oculus Rift.
			this->Setup(openGlContext, window, gdiDc);
		}
		else if (useDemoRift)
		{
			// If using a demo Oculus Rift, setup the virtual data.
			/*
			this->HMD.HResolution = 1280;
			this->HMD.VResolution = 800;
			this->HMD.HScreenSize = 0.149759993f;
			this->HMD.VScreenSize = 0.0935999975f;
			this->HMD.VScreenCenter = 0.0467999987f;
			this->HMD.EyeToScreenDistance = 0.0410000011f;
			this->HMD.LensSeparationDistance = 0.0635000020f;
			this->HMD.InterpupillaryDistance = 0.0640000030f;
			this->HMD.DistortionK[0] = 1.00000000f;
			this->HMD.DistortionK[1] = 0.219999999f;
			this->HMD.DistortionK[2] = 0.239999995f;
			this->HMD.DistortionK[3] = 0.000000000f;
			this->HMD.ChromaAbCorrection[0] = 0.995999992f;
			this->HMD.ChromaAbCorrection[1] = -0.00400000019f;
			this->HMD.ChromaAbCorrection[2] = 1.01400006f;
			this->HMD.ChromaAbCorrection[3] = 0.000000000f;
			this->HMD.DesktopX = 0;
			this->HMD.DesktopY = 0;
			this->viewport = OVR::Util::Render::Viewport(0, 0, this->HMD.HResolution, this->HMD.VResolution);
			*/

			// Say that it is connected, but virtually.
			this->connected = true;
			this->virtuallyConnected = true;

			// Setup the user's data for the Oculus Rift.
			this->Setup(openGlContext, window, gdiDc);
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
		InitRift();

		// Set the boolean indicating that there is an oculus rift to false.
		this->connected = false;

		this->HMD = ovrHmd_Create(0);

		if (this->HMD)
		{
			ovrHmd_GetDesc(this->HMD, &this->HMDDesc);
			if (this->HMDDesc.Type == ovrHmd_DK1 || this->HMDDesc.Type == ovrHmd_DK2)
			{
				ovrHmd_StartSensor(this->HMD, ovrHmdCap_Orientation | ovrHmdCap_YawCorrection | ovrHmdCap_Position | ovrHmdCap_LowPersistence, ovrHmdCap_Orientation);
				this->sensorState = ovrHmd_GetSensorState(this->HMD, 0.0);
				this->connected = true;
			}
		}
	}

	/**
	 * This method will setup the Oculus Rift headset.
	 *
	 * This method goes through and sets up the user's data for the Oculus Rift headset.
	 * This data includes things such as the interpupillary distance, field of view, and more.
	 */
	void OculusRift::Setup(HGLRC openGlContext, HWND window, HDC gdiDc)
	{
		OVR::Sizei size = ovrHmd_GetFovTextureSize(this->HMD, ovrEye_Left, this->HMDDesc.DefaultEyeFov[0], 1.0f);
		OVR::Sizei size2 = ovrHmd_GetFovTextureSize(this->HMD, ovrEye_Left, this->HMDDesc.DefaultEyeFov[0], 1.0f);

		this->renderTargetSize.w = 1280;
		this->renderTargetSize.h = 800;

		this->eyes[0].Eye = ovrEye_Left;
		this->eyes[0].Fov = this->HMDDesc.DefaultEyeFov[0];
		this->eyes[0].TextureSize = this->renderTargetSize;
		this->eyes[0].RenderViewport.Pos = OVR::Vector2i(0, 0);
		this->eyes[0].RenderViewport.Size = OVR::Sizei(this->renderTargetSize.w / 2, this->renderTargetSize.h);

		this->eyes[1].Eye = ovrEye_Right;
		this->eyes[1].Fov = this->HMDDesc.DefaultEyeFov[1];
		this->eyes[1].TextureSize = this->renderTargetSize;
		this->eyes[1].RenderViewport.Pos = OVR::Vector2i((this->renderTargetSize.w + 1) / 2, 0);
		this->eyes[1].RenderViewport.Size = this->eyes[0].RenderViewport.Size;

		this->openGLConfig.OGL.Header.API = ovrRenderAPI_OpenGL;
		this->openGLConfig.OGL.Header.RTSize = OVR::Sizei(HMDDesc.Resolution.w, HMDDesc.Resolution.h);
		this->openGLConfig.OGL.Header.Multisample = 0;
		this->openGLConfig.OGL.WglContext = openGlContext;
		this->openGLConfig.OGL.Window = window;
		this->openGLConfig.OGL.GdiDc = gdiDc;

		this->eyes[0].TextureSize.w = 1280 / 2;
		this->eyes[1].TextureSize.w = 1280 / 2;
		if (!ovrHmd_ConfigureRendering(this->HMD, &this->openGLConfig.Config, ovrHmdCap_NoVSync, this->HMDDesc.DistortionCaps, eyes, eyeRenderDesc))
		{
			this->connected = false;
			printf("Error configuring Oculus Rift renderer!\n");
		}
	}

	void OculusRift::SetRenderTextures(unsigned int leftEyeTexture, unsigned int rightEyeTexture)
	{
		this->eyeTextures[0].OGL.Header.API = ovrRenderAPI_OpenGL;
		this->eyeTextures[0].OGL.Header.TextureSize = this->renderTargetSize;
		this->eyeTextures[0].OGL.Header.RenderViewport = this->eyes[0].RenderViewport;
		this->eyeTextures[0].OGL.TexId = leftEyeTexture;

		this->eyeTextures[1].OGL.Header.API = ovrRenderAPI_OpenGL;
		this->eyeTextures[1].OGL.Header.TextureSize = this->renderTargetSize;
		this->eyeTextures[1].OGL.Header.RenderViewport = this->eyes[1].RenderViewport;
		this->eyeTextures[1].OGL.TexId = rightEyeTexture;
	}

	/**
	 * Returns a boolean indicating whether the Oculus Rift is connected.
	 *
	 * This method checks to see if the Oculus Rift is connected and returns
	 * a boolean indicating whether it is or not.
	 * @return Returns true if the Oculus Rift is connected, false otherwise.
	 */
	const bool OculusRift::isConnected() const
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
		if (this->connected && !this->virtuallyConnected)
		{
			this->sensorState = ovrHmd_GetSensorState(this->HMD, 0.0);
			if (this->sensorState.StatusFlags & (ovrStatus_OrientationTracked))
			{
				this->OldOrientation.yaw = this->Orientation.yaw;
				this->OldOrientation.pitch = this->Orientation.pitch;
				this->OldOrientation.roll = this->Orientation.roll;

				OVR::Posef pose = this->sensorState.Recorded.Pose;
				pose.Orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&this->Orientation.yaw, &this->Orientation.pitch, &this->Orientation.roll);

				this->Orientation.pitch = this->Orientation.pitch * 180.0f / 3.14159265358979323846f;
				this->Orientation.yaw = this->Orientation.yaw * 180.0f / 3.14159265358979323846f;
				this->Orientation.roll = this->Orientation.roll * 180.0f / 3.14159265358979323846f;

				this->Rotation.x -= (this->Orientation.pitch - this->OldOrientation.pitch);
				this->Rotation.y -= (this->Orientation.yaw - this->OldOrientation.yaw);
				this->Rotation.z -= (this->Orientation.roll - this->OldOrientation.roll);
			}
		}
	}

	void OculusRift::getPerspectiveMatrix(RiftEye eye, Math::Matrix<float> &perspectiveMatrix)
	{
		ovrMatrix4f proj = ovrMatrix4f_Projection(this->eyes[HMDDesc.EyeRenderOrder[eye]].Fov, 0.01f, 10000.0f, true);
		for (int y = 0; y < 4; y += 1)
		{
			for (int x = 0; x < 4; x += 1)
			{
				perspectiveMatrix[x + y * 4] = proj.M[x][y];
			}
		}
	}

	bool OculusRift::StartRender()
	{
		if (this->isConnected())
		{
			ovrHmd_BeginFrame(this->HMD, 0);
			return true;
		}
		return false;
	}

	void OculusRift::StartEyeRender(RiftEye eye, Math::Matrix<float> &viewMatrix)
	{
		currentEyePose = ovrHmd_BeginEyeRender(this->HMD, this->HMDDesc.EyeRenderOrder[eye]);
		OVR::Quatf orientation = OVR::Quatf(currentEyePose.Orientation);
		OVR::Matrix4f viewShift = OVR::Matrix4f(orientation.Inverted()) * OVR::Matrix4f::Translation(currentEyePose.Position);
	}

	void OculusRift::EndEyeRender(RiftEye eye)
	{
		glViewport(this->eyeRenderDesc[eye].Desc.RenderViewport.Pos.x, 
			this->eyeRenderDesc[eye].Desc.RenderViewport.Pos.y,
			this->eyeRenderDesc[eye].Desc.RenderViewport.Size.w,
			this->eyeRenderDesc[eye].Desc.RenderViewport.Size.h);
		ovrHmd_EndEyeRender(this->HMD, this->HMDDesc.EyeRenderOrder[eye], this->currentEyePose, &this->eyeTextures[this->HMDDesc.EyeRenderOrder[eye]].Texture);
	}

	void OculusRift::EndRender()
	{
		ovrHmd_EndFrame(this->HMD);
	}

	/**
	 * Get the rotation values for the angle of rotation for where the user is looking.
	 * This is in Euler angles.
	 * @return The rotation on the X, Y, and Z axis in Euler angles.
	 */
	const rotation_t OculusRift::GetRotation() const
	{
		return this->Rotation;
	}

	void OculusRift::ComposeFinalImage(unsigned int leftEyeTexture, unsigned int rightEyeTexture)
	{
	}


	/**
	 * This is the deconstructor for the Oculus Rift device.
	 *
	 * This is the deconstructor for he Oculus Rift device.  This method will automatically
	 * cleanup all resources associated with the Oculus Rift device.
	 */
	OculusRift::~OculusRift()
	{
		ovrHmd_Destroy(this->HMD);
		ovr_Shutdown();
	}
};
