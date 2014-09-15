#include "pv/OculusRift.h"
#include "pv/MinOpenGL.h"
#include "pv/FileIO.h"
#include "Kernel/OVR_Math.h"

namespace PV
{
	bool InitRift()
	{
		return ovr_Initialize();
	}
	bool DetectDevice()
	{
		InitRift();
		bool active = false;
		return active;
	}
	OculusRift::OculusRift(bool useDemoRift, HGLRC openGlContext, HWND window, HDC deviceContext)
	{
		// Setup the initial values for all of the rotations.
		this->Orientation.yaw = 0.0f;
		this->Orientation.pitch = 0.0f;
		this->Orientation.roll = 0.0f;

		this->Orientation_quart.angle = 0.0f;
		this->Orientation_quart.axis.x = 0.0f;
		this->Orientation_quart.axis.y = 0.0f;
		this->Orientation_quart.axis.z = 0.0f;

		this->virtuallyConnected = false;

		// Initialize the Oculus Rift.
		this->Initialize();

		if (this->connected)
		{
			// Get the list of enabled capabilities, then attach it to a window if the extend
			// desktop capability is disabled.
			int caps = ovrHmd_GetEnabledCaps(this->HMD);
			if (!(ovrHmd_GetEnabledCaps(this->HMD) & ovrHmdCap_ExtendDesktop))
			{
				ovrHmd_AttachToWindow(this->HMD, window, nullptr, nullptr);
			}

			// Setup the OpenGL side for the Oculus Rift.
			this->Setup(openGlContext, window, deviceContext);
		}
		else
		{
			this->HMD = NULL;
		}
		/*
		else if (useDemoRift)
		{
			// If using a demo Oculus Rift, setup the virtual data.

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

			// Say that it is connected, but virtually.
			this->connected = true;
			this->virtuallyConnected = true;

			// Setup the user's data for the Oculus Rift.
			this->Setup(openGlContext, window, deviceContext);
		}
		*/
	}

	void OculusRift::Initialize()
	{
		// Set the boolean indicating that there is an oculus rift to false.
		this->connected = false;

		// Get the number of rifts, and attach the HMD handle to the first instance of an Oculus Rift.
		int rifts = ovrHmd_Detect();
		if (rifts > 0)
		{
			this->HMD = ovrHmd_Create(0);

			// If the HMD was found, check that it is an Oculus Rift, then configure it's tracking and
			// tell the class it's connected.
			if (this->HMD)
			{
				if (this->HMD->Type == ovrHmd_DK1 || this->HMD->Type == ovrHmd_DK2)
				{
					ovrHmd_ConfigureTracking(this->HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position | ovrHmdCap_LowPersistence, ovrTrackingCap_Orientation);
					this->sensorState = ovrHmd_GetTrackingState(this->HMD, 0.0);
					this->connected = true;
				}
			}
		}
	}

	void OculusRift::Setup(HGLRC openGlContext, HWND window, HDC deviceContext)
	{
		// Set the render size for the window.
		this->renderSize.w = this->HMD->Resolution.w;
		this->renderSize.h = this->HMD->Resolution.h;

		// Configure the OpenGL header, then attach it to the normal config header.
		this->openGLConfig.OGL.Header.API = ovrRenderAPI_OpenGL;
		this->openGLConfig.OGL.Header.RTSize = OVR::Sizei(this->renderSize.w, this->renderSize.h);
		this->openGLConfig.OGL.Header.Multisample = 0;
		this->openGLConfig.OGL.Window = window;
		this->openGLConfig.OGL.DC = deviceContext;
		this->openGLConfig.Config.Header = this->openGLConfig.OGL.Header;

		// Setup the field of views for each eye and configure the rendering with them.
		const ovrFovPort eyeFovs[2] = { this->HMD->DefaultEyeFov[0], this->HMD->DefaultEyeFov[1] };
		if (!ovrHmd_ConfigureRendering(this->HMD, &this->openGLConfig.Config, ovrDistortionCap_TimeWarp |
			ovrDistortionCap_Chromatic |
			ovrDistortionCap_Vignette, eyeFovs, this->eyes))
		{
			this->connected = false;
			printf("Error configuring Oculus Rift renderer!\n");
		}

		// Set the cabilities for what is needed for the head tracking.
		ovrHmd_SetEnabledCaps(this->HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position | ovrHmdCap_LowPersistence);

		// Setup the textures and frame buffers.
		((ovrGLTexture&)this->eyeTextures[0]).OGL.TexId = this->leftEyeTexture;
		((ovrGLTexture&)this->eyeTextures[1]).OGL.TexId = this->rightEyeTexture;
		this->setupFrameBuffer();
		this->SetRenderTextures(this->leftEyeTexture, this->rightEyeTexture);
	}

	void OculusRift::setupFrameBuffer()
	{
		glGenTextures(1, &this->leftEyeTexture);
		glBindTexture(GL_TEXTURE_2D, this->leftEyeTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, PV_GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, PV_GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, PV_GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->renderSize.w, this->renderSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		pv_glGenerateMipmap(GL_TEXTURE_2D);

		glGenTextures(1, &this->rightEyeTexture);
		glBindTexture(GL_TEXTURE_2D, this->rightEyeTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, PV_GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, PV_GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, PV_GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->renderSize.w, this->renderSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		pv_glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, this->leftEyeTexture);
		pv_glGenFramebuffers(1, &this->leftFrameBuffer);
		pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, this->leftFrameBuffer);
		pv_glGenRenderbuffers(1, &this->leftDepthBuffer);
		pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, this->leftDepthBuffer);
		pv_glRenderbufferStorage(PV_GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->renderSize.w, this->renderSize.h);
		pv_glFramebufferRenderbuffer(PV_GL_FRAMEBUFFER, PV_GL_DEPTH_ATTACHMENT, PV_GL_RENDERBUFFER, this->leftDepthBuffer);
		pv_glFramebufferTexture2D(PV_GL_FRAMEBUFFER, PV_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->leftEyeTexture, 0);

		glBindTexture(GL_TEXTURE_2D, this->rightEyeTexture);
		pv_glGenFramebuffers(1, &this->rightFrameBuffer);
		pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, this->rightFrameBuffer);
		pv_glGenRenderbuffers(1, &this->rightDepthBuffer);
		pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, this->rightDepthBuffer);
		pv_glRenderbufferStorage(PV_GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->renderSize.w, this->renderSize.h);
		pv_glFramebufferRenderbuffer(PV_GL_FRAMEBUFFER, PV_GL_DEPTH_ATTACHMENT, PV_GL_RENDERBUFFER, this->rightDepthBuffer);
		pv_glFramebufferTexture2D(PV_GL_FRAMEBUFFER, PV_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->rightEyeTexture, 0);

		pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, 0);
		pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, 0);
	}

	void OculusRift::SetRenderTextures(unsigned int leftEyeTexture, unsigned int rightEyeTexture)
	{
		this->eyeTextures[0].Texture.Header.TextureSize.w = this->renderSize.w;
		this->eyeTextures[0].Texture.Header.TextureSize.h = this->renderSize.h;
		this->eyeTextures[0].Texture.Header.RenderViewport.Size.w = this->renderSize.w;
		this->eyeTextures[0].Texture.Header.RenderViewport.Size.h = this->renderSize.h;
		this->eyeTextures[0].Texture.Header.RenderViewport.Pos.x = 0;
		this->eyeTextures[0].Texture.Header.RenderViewport.Pos.y = 0;
		this->eyeTextures[0].Texture.Header.API = ovrRenderAPI_OpenGL;

		this->eyeTextures[1].Texture.Header.TextureSize.w = this->renderSize.w;
		this->eyeTextures[1].Texture.Header.TextureSize.h = this->renderSize.h;
		this->eyeTextures[1].Texture.Header.RenderViewport.Size.w = this->renderSize.w;
		this->eyeTextures[1].Texture.Header.RenderViewport.Size.h = this->renderSize.h;
		this->eyeTextures[1].Texture.Header.RenderViewport.Pos.x = 0;
		this->eyeTextures[1].Texture.Header.RenderViewport.Pos.y = 0;
		this->eyeTextures[1].Texture.Header.API = ovrRenderAPI_OpenGL;

		this->eyeTextures[0].OGL.Header.API = ovrRenderAPI_OpenGL;
		this->eyeTextures[0].OGL.Header.TextureSize.w = this->renderSize.w;
		this->eyeTextures[0].OGL.Header.TextureSize.h = this->renderSize.h;
		this->eyeTextures[0].OGL.Header.RenderViewport = this->eyes[0].DistortedViewport;
		this->eyeTextures[0].OGL.TexId = leftEyeTexture;

		this->eyeTextures[1].OGL.Header.API = ovrRenderAPI_OpenGL;
		this->eyeTextures[1].OGL.Header.TextureSize.w = this->renderSize.w;
		this->eyeTextures[1].OGL.Header.TextureSize.h = this->renderSize.h;
		this->eyeTextures[1].OGL.Header.RenderViewport = this->eyes[1].DistortedViewport;
		this->eyeTextures[1].OGL.TexId = rightEyeTexture;
	}

	/**
	 * Returns a boolean indicating whether the Oculus Rift is connected.
	 *
	 * Checks if the Oculus Rift is connected and returns
	 * a boolean indicating whether it is or not.
	 * @return Returns true if the Oculus Rift is connected, false otherwise.
	 */
	const bool OculusRift::isConnected() const
	{
		return (const bool)this->connected;
	}

	/**
	 * Updates the data from the Oculus Rift headset.
	 *
	 * Updates data recieved from the Oculus Rift.  It pulls the change in
	 * orientation and updates the rotation of where the user is looking.
	 */
	void OculusRift::Update()
	{
		if (this->connected && !this->virtuallyConnected)
		{
			this->sensorState = ovrHmd_GetTrackingState(this->HMD, 0.0);
			if (this->sensorState.StatusFlags & (ovrStatus_OrientationTracked))
			{
				//this->Rotation.x = this->Orientation.pitch * 180.0f / 3.14159265358979323846f;
				//this->Rotation.y = this->Orientation.yaw * 180.0f / 3.14159265358979323846f;
				//this->Rotation.z = this->Orientation.roll * 180.0f / 3.14159265358979323846f;
			}
		}
	}

	void OculusRift::DismissWarningScreen()
	{
		static ovrHSWDisplayState hswDisplayState;
		ovrHmd_GetHSWDisplayState(this->HMD, &hswDisplayState);
		if (hswDisplayState.Displayed) {
			ovrHmd_DismissHSWDisplay(this->HMD);
			return;
		}
	}

	bool OculusRift::StartRender()
	{
		if (this->isConnected())
		{
			static int frameIndex = 0;
			ovrHmd_BeginFrame(this->HMD, frameIndex);
			frameIndex++;
			return true;
		}
		return false;
	}

	void OculusRift::StartEyeRender(RiftEye eye, Math::Matrix<float> &viewMatrix)
	{
		if (this->isConnected())
		{
			this->eyePoses[this->HMD->EyeRenderOrder[eye]] = 
				ovrHmd_GetEyePose(this->HMD, this->HMD->EyeRenderOrder[eye]);

			OVR::Quat<float> getters = this->eyePoses[this->HMD->EyeRenderOrder[eye]].Orientation;
			getters.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&this->Orientation.yaw, &this->Orientation.pitch, &this->Orientation.roll);
			
			viewMatrix.SetIdentity();
			viewMatrix.Rotate(this->Orientation.pitch, this->Orientation.yaw, this->Orientation.roll);
			viewMatrix.Translate(this->eyePoses[this->HMD->EyeRenderOrder[eye]].Position.x,
				this->eyePoses[this->HMD->EyeRenderOrder[eye]].Position.y,
				this->eyePoses[this->HMD->EyeRenderOrder[eye]].Position.z);

			switch (this->HMD->EyeRenderOrder[eye])
			{
			case ovrEye_Left:
				glViewport(0, 0, 640, 800);
				pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, this->leftFrameBuffer);
				pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, this->leftDepthBuffer);
				break;
			case ovrEye_Right:
				glViewport(640, 0, 640, 800);
				pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, this->rightFrameBuffer);
				pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, this->rightDepthBuffer);
				break;
			}
		}
	}

	void OculusRift::EndEyeRender(RiftEye eye)
	{
		if (this->isConnected())
		{
			glViewport(0, 0, 1280, 800);
			pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, 0);
			pv_glBindRenderbuffer(PV_GL_RENDERBUFFER, 0);
			switch (this->HMD->EyeRenderOrder[eye])
			{
			case ovrEye_Left:
				glBindTexture(GL_TEXTURE_2D, this->leftEyeTexture);
				pv_glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;
			case ovrEye_Right:
				glBindTexture(GL_TEXTURE_2D, this->rightEyeTexture);
				pv_glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;
			}
		}
	}

	void OculusRift::EndRender()
	{
		if (this->isConnected())
		{
			ovrTexture textures[2] = { this->eyeTextures[0].Texture, this->eyeTextures[1].Texture };
			ovrHmd_EndFrame(this->HMD, this->eyePoses, textures);
		}
	}

	void OculusRift::getPerspectiveMatrix(RiftEye eye, Math::Matrix<float> &perspectiveMatrix)
	{
		ovrMatrix4f proj = ovrMatrix4f_Projection(this->eyes[this->HMD->EyeRenderOrder[eye]].Fov, 0.01f, 10000.0f, true);
		for (int y = 0; y < 4; y += 1)
		{
			for (int x = 0; x < 4; x += 1)
			{
				perspectiveMatrix[x + y * 4] = proj.M[x][y];
			}
		}
	}

	/**
	* Gets the rotation values for the angle of rotation for where the user is looking.
	* This is in Euler angles.
	* @return The rotation yaw, pitch, and roll in Euler angles.
	*/
	const orientation_t OculusRift::GetRotation() const
	{
		return this->Orientation;
	}

	const OVR::Sizei OculusRift::getRenderSize() const
	{
		return (const OVR::Sizei)this->renderSize;
	}

	/**
	 * Deconstructor for the Oculus Rift device.
	 *
	 * Seconstructor for he Oculus Rift device.  Automatically
	 * cleans up all resources associated with the Oculus Rift device.
	 */
	OculusRift::~OculusRift()
	{
		if (this->HMD != NULL)
		{
			ovrHmd_Destroy(this->HMD);
		}
		ovr_Shutdown();
	}
};
