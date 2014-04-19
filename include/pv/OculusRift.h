#ifndef _OCULUS_RIFT_H_
#define _OCULUS_RIFT_H_

#include "pv/types.h"
#include "pv/Matrices.h"
#include "OVR_CAPI_GL.h"

namespace PV
{
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
		 * This constructor will automatically attempt to initialize and setup an
		 * Oculus Rift device that is connected to the computer.  Use the IsConnected
		 * method to see if the device was successfully setup.  You can also specify to create
		 * a virtual Oculus Rift device if the hardware is not found.  This is useful for testing the output
		 * from the device.
		 * @param useDemoRift This will tell the class to create a virtual Oculus Rift when true.
		 * @param openGlContext The OpenGL context to use for rendering the final scene to.
		 * @param window The window that contains the OpenGL context.
		 * @param deviceContext The device context that is used for the window.
		 */
		OculusRift(bool useDemoRift, HGLRC openGlContext, HWND window, HDC deviceContext);
		/**
		 * This method goes through and connect to the Oculus Rift hardware.  It then retrieves
		 * the sensor, as well as a sensor fusion, both of which can be used to retrieve data
		 * from the Oculus Rift.
		 */
		void Initialize();
		/**
		 * This method goes through and sets up the Oculus Rift for use.
		 * @param openGlContext The OpenGL context to use for rendering the final scene to.
		 * @param window The window that contains the OpenGL context.
		 * @param deviceContext The device context that is used for the window.
		 */
		void Setup(HGLRC openGlContext, HWND window, HDC deviceContext);

		/**
		 * Set the textures for the left and right eyes.
		 * @param leftEyeTexture The texture to display to the left eye.
		 * @param rightEyeTexture The texture to display to the right eye.
		 */
		void SetRenderTextures(unsigned int leftEyeTexture, unsigned int rightEyeTexture);

		/**
		 * This method checks to see if the Oculus Rift is connected and returns
		 * a boolean indicating whether it is or not.
		 * @return Returns true if the Oculus Rift is connected, false otherwise.
		 */
		const bool isConnected() const;

		/**
		 * This method updates data recieved from the Oculus Rift.  It currently pulls the change in
		 * orientation and updates the rotation of where the user is looking.
		 */
		void Update();

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
		 * Get the rotation values for the angle of rotation for where the user is looking.
		 * This is in Euler angles.
		 * @return The rotation on the X, Y, and Z axis in Euler angles.
		 */
		const rotation_t GetRotation() const;

		/**
		* Gets the size of the textures to render to.
		*/
		const OVR::Sizei getRenderSize() const;

		/**
		 * Compose the final rendered image that the Rift should see per eye using textured quads.
		 * @param leftEyeTexture The image that the left eye should see.
		 * @param rightEyeTexture The image that the right eye should see.
		 */
		void ComposeFinalImage(unsigned int leftEyeTexture, unsigned int rightEyeTexture);

		/**
		 * This is the deconstructor for he Oculus Rift device.  This method will automatically
		 * cleanup all resources associated with the Oculus Rift device.
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
		 * The head mounted display device description.
		 */
		ovrHmdDesc HMDDesc;

		/**
		 * A structure to hold the state of the Oculus Rift's sensors (Gyroscope, etc.)
		 */
		ovrSensorState sensorState;

		/**
		 * The OpenGL configuration for the Oculus Rift's renderer.
		 */
		ovrGLConfig openGLConfig;

		/** 
		 * The rendering descriptions for each eye.
		 */
		ovrEyeRenderDesc eyeRenderDesc[2];

		/**
		 * The OpenGL textures to use for each eye.
		 */
		ovrGLTexture eyeTextures[2];

		/**
		 * The Oculus Rift's descriptions for each eye.
		 */
		ovrEyeDesc eyes[2];

		/** 
		 * The poses of each eye from the Oculus Rift.
		 */
		OVR::Posef eyePoses[2];

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
		  * The previous orientation of the oculus rift. (yaw, pitch, roll) This is used to
		  * update the orientation data properly.
		  */
		orientation_t OldOrientation;
		/**
		  * The previous orientation of the oculus rift. (Quaternion) This is used to
		  * update the orientation data properly.
		  */
		orientation_quart_t OldOrientation_quart;

		/**
		 * The rotation data for where the user is looking.
		 */
		rotation_t Rotation;

		/**
		 * Sets up the frame buffers for the left and right eyes.
		 */
		void setupFrameBuffer();
	};
};
#endif