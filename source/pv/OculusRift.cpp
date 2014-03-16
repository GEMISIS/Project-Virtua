#include "pv/OculusRift.h"
#include "pv/MinOpenGL.h"

namespace PV
{
	/**
 * This is the constructor used to create a new Oculus Rift device.
 *
 * This constructor will automatically attempt to initialize and setup an
 * Oculus Rift device that is connected to the computer.  Use the IsConnected
 * method to see if the device was successfully setup.
 */
	OculusRift::OculusRift(bool useDemoRift)
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
			this->Setup();

			this->viewport = Viewport(0, 0, this->HMD.HResolution, this->HMD.VResolution);
		}
		else if (useDemoRift)
		{
			this->HMD.HResolution = 1280;
			this->HMD.VResolution = 800;
			this->HMD.HScreenSize = 0.14976f;
			this->HMD.VScreenSize = 0.0936f;
			this->HMD.VScreenCenter = this->HMD.VScreenSize / 2;
			this->HMD.EyeToScreenDistance = 0.041f;
			this->HMD.LensSeparationDistance = 0.0635f;
			this->HMD.InterpupillaryDistance = 0.064f;

			this->viewport = Viewport(0, 0, this->HMD.HResolution, this->HMD.VResolution);

			this->Setup();

			this->connected = true;
			this->virtuallyConnected = true;
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
		if (this->HMDHardwareDevice)
		{
			// If so, set the oculus rift's HMD object.
			if (this->HMDHardwareDevice->GetDeviceInfo(&HMD))
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

	void OculusRift::renderGLBelow2(RiftEye eye)
	{
		gluPerspective(this->FieldOfView, this->AspectRatio, 0.001f, 10000.0f);
		if (eye == Left)
		{
			glViewport(this->LeftEye.VP.x, this->LeftEye.VP.y, this->LeftEye.VP.w, this->LeftEye.VP.h);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(this->ProjectionCenterOffset, 0.0f, 0.0f);
			glTranslatef(this->HalfIPD * this->ViewCenter, 0.0f, 0.0f);
		}
		else if (eye == Right)
		{
			glViewport(this->RightEye.VP.x, this->RightEye.VP.y, this->RightEye.VP.w, this->RightEye.VP.h);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(-this->ProjectionCenterOffset, 0.0f, 0.0f);
			glTranslatef(-this->HalfIPD * this->ViewCenter, 0.0f, 0.0f);
		}
	}
	void OculusRift::renderGLAbove2(RiftEye eye)
	{
		gluPerspective(this->FieldOfView, this->AspectRatio, 0.001f, 10000.0f);

		int defaultProgram = 0;
		int fragmentShader = 0;
		int vertexShader = 0;
		this->SetupShaders(defaultProgram, fragmentShader, vertexShader);

		this->UpdateUniforms(eye, defaultProgram);
	}

	void OculusRift::ShiftView(RiftEye eye)
	{
		int openGlVersion[] = { 0, 0 };
		glGetIntegerv(PV_GL_MAJOR_VERSION, &openGlVersion[0]);
		glGetIntegerv(PV_GL_MINOR_VERSION, &openGlVersion[1]);

		this->ShiftView(eye, openGlVersion[0], openGlVersion[1]);
	}

	void OculusRift::ShiftView(RiftEye eye, float matrix[16])
	{
		if (eye == Left)
		{
			glViewport(this->LeftEye.VP.x, this->LeftEye.VP.y, this->LeftEye.VP.w, this->LeftEye.VP.h);
			matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
			matrix[3] = this->ProjectionCenterOffset + (this->HalfIPD * this->ViewCenter);
		}
		else if (eye == Right)
		{
			glViewport(this->RightEye.VP.x, this->RightEye.VP.y, this->RightEye.VP.w, this->RightEye.VP.h);
			matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
			matrix[3] = -this->ProjectionCenterOffset + (-this->HalfIPD * this->ViewCenter);
		}
	}

	void OculusRift::ShiftView(RiftEye eye, int majorVersion, int minorVersion)
	{
		switch (majorVersion)
		{
		case 1:
			this->renderGLBelow2(eye);
			break;
		default:
			this->renderGLAbove2(eye);
			break;
		}
	}

	void OculusRift::UpdateUniforms(RiftEye eye, GLuint program)
	{
		int textureRange = pv_glGetUniformLocation(program, "textureRange");
		int lensCenterOffset = pv_glGetUniformLocation(program, "lensCenterOffset");
		int distortion = pv_glGetUniformLocation(program, "distortion");
		int aspectRatio = pv_glGetUniformLocation(program, "aspectRatio");
		int fillScale = pv_glGetUniformLocation(program, "fillScale");
		int translation = pv_glGetUniformLocation(program, "translation");

		float translationMatrix[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		this->ShiftView(eye, translationMatrix);
		pv_glUniformMatrix4fv(translation, 1, GL_FALSE, translationMatrix);

		pv_glUniform2f(textureRange, 1.0f, 1.0f);
		if (eye == Left)
		{
			pv_glUniform2f(lensCenterOffset, this->LeftEye.pDistortion->XCenterOffset, this->LeftEye.pDistortion->YCenterOffset);
		}
		else if (eye == Right)
		{
			pv_glUniform2f(lensCenterOffset, this->RightEye.pDistortion->XCenterOffset, this->RightEye.pDistortion->YCenterOffset);
		}
		pv_glUniform4f(distortion, this->HMD.DistortionK[0], this->HMD.DistortionK[1], this->HMD.DistortionK[2], this->HMD.DistortionK[3]);
		pv_glUniform1f(aspectRatio, this->AspectRatio);
		pv_glUniform1f(fillScale, 1.0f);
	}

	void OculusRift::SetupShaders(int &program, int &fragment, int &vertex)
	{
		this->SetupShaders(NULL, NULL, program, fragment, vertex);
	}

	void OculusRift::SetupShaders(const char* fragmentShader, const char* vertexShader, int &program, int &fragment, int &vertex)
	{
		program = pv_glCreateProgram();
		fragment = pv_glCreateShader(PV_GL_FRAGMENT_SHADER);
		vertex = pv_glCreateShader(PV_GL_VERTEX_SHADER);

		if (fragmentShader == NULL)
		{
			char* fragmentShaderSource = "#version 120\nuniform sampler2D sampler0;\nuniform vec2 textureRange;\nuniform vec2 lensCenterOffset;\nuniform vec4 distortion;\nuniform float aspectRatio;\nuniform float fillScale;\nfloat ScaleDistortion(vec2 offset)\n{\nvec2 squaredOffset = offset * offset;\nfloat squaredRadius = squaredOffset.x + squaredOffset.y;\nfloat distortionScale = //\ndistortion[0] + //\ndistortion[1] * squaredRadius + //\ndistortion[2] * squaredRadius * squaredRadius + //\ndistortion[3] * squaredRadius * squaredRadius * squaredRadius;\nreturn distortionScale;\n}\nvec2 textureCoordsToDistortionOffsetCoords(vec2 texCoord)\n{\nvec2 result = texCoord * 2.0 - 1.0;\nresult -= lensCenterOffset;\nresult.y /= aspectRatio;\nreturn result;\n}\nvec2 distortionOffsetCoordsToTextureCoords(vec2 offset)\n{\nvec2 result = offset / fillScale;\nresult.y *= aspectRatio;\nresult += lensCenterOffset;\nresult /= 2.0;\nresult += 0.5;return result;\n}\nvoid main()\n{\nvec2 offset = textureCoordsToDistortionOffsetCoords(gl_TexCoord[0].st);\nfloat scale = ScaleDistortion(offset);\nvec2 distortedOffset = offset * scale;\nvec2 actualTextureCoords = distortionOffsetCoordsToTextureCoords(distortedOffset);\nactualTextureCoords *= textureRange;\nvec2 clamped = clamp(actualTextureCoords, vec2(0, 0), textureRange);\nif(!all(equal(clamped, actualTextureCoords)))\n{\ngl_FragColor = vec4(0, 0, 0, 1);\n}\nelse\n{\ngl_FragColor = texture2D(sampler0, actualTextureCoords);\n}\n}\n";
			int length = strlen(fragmentShaderSource);
			pv_glShaderSource(fragment, 1, &fragmentShaderSource, &length);
			pv_glCompileShader(fragment);

			int result = false;
			int logLength = 0;
			pv_glGetShaderiv(fragment, PV_GL_COMPILE_STATUS, &result);
			pv_glGetShaderiv(fragment, PV_GL_INFO_LOG_LENGTH, &logLength);
			char* VertexShaderErrorMessage = (char*)malloc(sizeof(char)* logLength);
			pv_glGetShaderInfoLog(fragment, logLength, NULL, VertexShaderErrorMessage);
			fprintf(stdout, "%s\n", VertexShaderErrorMessage);
		}

		if (vertexShader == NULL)
		{
			char* vertexShaderSource = "#version 120\nuniform mat4 translation;\nvarying vec2 texture_coordinate;\nvoid main()\n{\ngl_Position = gl_ModelViewProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\ngl_Position = translation * gl_Position;\ntexture_coordinate = vec2(gl_MultiTexCoord0);\ngl_TexCoord[0] = gl_MultiTexCoord0;\n}\n";
			int length = strlen(vertexShaderSource);
			pv_glShaderSource(vertex, 1, &vertexShaderSource, &length);
			pv_glCompileShader(vertex);

			int result = false;
			int logLength = 0;
			pv_glGetShaderiv(vertex, PV_GL_COMPILE_STATUS, &result);
			pv_glGetShaderiv(vertex, PV_GL_INFO_LOG_LENGTH, &logLength);
			char* VertexShaderErrorMessage = (char*)malloc(sizeof(char)* logLength);
			pv_glGetShaderInfoLog(vertex, logLength, NULL, VertexShaderErrorMessage);
			fprintf(stdout, "%s\n", VertexShaderErrorMessage);
		}

		pv_glAttachShader(program, fragment);
		pv_glAttachShader(program, vertex);

		pv_glLinkProgram(program);
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

	/**
	 * Get viewport for the Oculus Rift.  This should be split in half for each eye.
	 * @return The viewport of what the Oculus Rift can see.
	 */
	const Viewport OculusRift::GetViewport() const
	{
		return this->viewport;
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
};
