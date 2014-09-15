#pragma once

#include "pv/types.h"
#include "pv/Matrices.h"
#include "OVR_CAPI_GL.h"

namespace PV
{
	/**
	* Initializes the Oculus VR SDK.
	*/
	bool InitRift();
	/**
	 * Checks to see if there is an Oculus Rift available.
	 */
	bool DetectDevice();

	/**
	 * A class for managing any Oculus Rift devices connected to the computer.  This structure can be used
	 * to get various properties of the Oculus Rift, such as the head rotation, as well as to warp the output
	 * to be displayed in OpenGL.  DirectX support is still under construction as of now.
	 */
	class OculusRift
	{
	public:
		/**
		 * Constructor used to create a new Oculus Rift device.
		 *
		 * This constructor will automatically attempt to initialize and setup an
		 * Oculus Rift device that is connected to the computer.  Use the IsConnected
		 * method to see if the device was successfully setup.
		 * @param useDemoRift Class creates a virtual Oculus Rift when true.
		 * @param openGlContext The OpenGL context to use for rendering the final scene to.
		 * @param window The window that contains the OpenGL context.
		 * @param deviceContext The device context that is used for the window.
		 */
		OculusRift(bool useDemoRift, HGLRC openGlContext, HWND window, HDC deviceContext);
		/**
		 * Initializes the Oculus Rift headset.
		 *
		 * Connects to the Oculus Rift hardware. Then retrieves the sensor as well
		 * as a sensor fusion, both of which can be used to retrieve data from the
		 * Oculus Rift.
		 */
		void Initialize();

		/**
		 * Sets up the Oculus Rift for use.
		 * @param openGlContext The OpenGL context to use for rendering the final scene to.
		 * @param window The window that contains the OpenGL context.
		 * @param deviceContext The device context that is used for the window.
		 */
		void Setup(HGLRC openGlContext, HWND window, HDC deviceContext);

		/**
		 * Sets the textures for the left and right eyes.
		 * @param leftEyeTexture The texture to display to the left eye.
		 * @param rightEyeTexture The texture to display to the right eye.
		 */
		void SetRenderTextures(unsigned int leftEyeTexture, unsigned int rightEyeTexture);

		/**
		 * Checks to see if the Oculus Rift is connected and returns
		 * a boolean indicating whether it is or not.
		 * @return Returns true if the Oculus Rift is connected, false otherwise.
		 */
		const bool isConnected() const;

		/**
		 * Updates data recieved from the Oculus Rift.  It currently pulls the change in
		 * orientation and updates the rotation of where the user is looking.
		 */
		void Update();

		/**
		 * Dismisses the warning screen shown when the application starts on the Oculus Rift.
		 */
		void DismissWarningScreen();

		/**
		 * Begins rendering a scene to the Oculus Rift.
		 */
		bool StartRender();
		/**
		 * Begins rendering a specific eye for the Oculus Rift scene and retrieves the view offset matrix for it.
		 * @param eye The eye to render the scene for.
		 * @param viewMatrix The matrix to store the view offset into.
		 */
		void StartEyeRender(RiftEye eye, Math::Matrix<float> &viewMatrix);
		/** 
		 * Ends the rendering of a specific eye.
		 * @param eye The eye to stop rendering for.
		 */
		void EndEyeRender(RiftEye eye);
		/**
		 * Ends rendering the scene for the Oculus Rift, calling the updates to swap the window's buffers as well.
		 */
		void EndRender();

		/** 
		 * Retrieves the perspective matrix.
		 * @param eye The eye to retrieve the perspective for.
		 * @param perspectiveMatrix The matrix to store the perspective view in.
		 */
		void getPerspectiveMatrix(RiftEye eye, Math::Matrix<float> &perspectiveMatrix);

		/**
		 * Gets the rotation values for the angle of rotation for where the user is looking.
		 * This is in Euler angles.
		 * @return The rotation yaw, pitch, and roll in Euler angles.
		 */
		const orientation_t GetRotation() const;

		/**
		* Gets the size of the textures to render to.
		*/
		const OVR::Sizei getRenderSize() const;

		/**
		 * Deconstructor for the Oculus Rift device.  Automatically
		 * cleans up all resources associated with the Oculus Rift device.
		 */
		~OculusRift();
	protected:
		/**
		  * The oculus rift's information in the form of a Head Mounted Display (HMD).  This is used
		  * to get information specially needed for HMD's, rather than hardware information.
		  */
		ovrHmd HMD;

		/**
		 * The size of the textures to render the scene to.
		 */
		OVR::Sizei renderSize;

		/**
		 * A structure to hold the state of the Oculus Rift's sensors (Gyroscope, etc.)
		 */
		ovrTrackingState sensorState;

		/**
		 * The OpenGL configuration for the Oculus Rift's renderer.
		 */
		ovrGLConfig openGLConfig;

		/**
		 * The OpenGL textures to use for each eye.
		 */
		ovrGLTexture eyeTextures[2];

		/**
		 * The Oculus Rift's descriptions for each eye.
		 */
		ovrEyeRenderDesc eyes[2];

		/** 
		 * The poses of each eye from the Oculus Rift.
		 */
		ovrPosef eyePoses[2];

		/**
		The texture to use for the left eye.
		*/
		unsigned int leftEyeTexture;
		/**
		The texture to use for the right eye.
		*/
		unsigned int rightEyeTexture;

		/**
		* The frame buffer to use for the left eye.
		*/
		unsigned int leftFrameBuffer;
		/**
		* The frame buffer to use for the right eye.
		*/
		unsigned int rightFrameBuffer;

		/**
		* The buffer to use for storing the depth data for the left eye.
		*/
		unsigned int leftDepthBuffer;
		/**
		* The buffer to use for storing the depth data for the right eye.
		*/
		unsigned int rightDepthBuffer;

		/**
		  * A boolean indicating whether an oculus rift is connected or not.
		  */
		bool connected;

		/**
		* A boolean indicating whether the connected Rift is a virtual one or not. (IE: Hardware or emulated)
		*/
		bool virtuallyConnected;

		/**
		  * The orientation of the oculus rift. (yaw, pitch, roll)
		  */
		orientation_t Orientation;
		/**
		  * The orientation of the oculus rift. (Quaternion)
		  */
		orientation_quart_t Orientation_quart;

		/**
		 * Sets up the frame buffers for the left and right eyes.
		 */
		void setupFrameBuffer();
	};
};
