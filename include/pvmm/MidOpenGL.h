#ifndef _MID_OPEN_GL_H_
#define _MID_OPEN_GL_H_

#include "pv/MinOpenGL.h"
#include "pv/Matrices.h"

#define _USE_MATH_DEFINES
#include <Math.h>

namespace PV
{
	typedef bool(__stdcall* PFGNWGLSWAPINTERVALEXTPROC)(int interval);

	/**
	 * Set the display to use vertical synching.
	 * @param interval The interval to update at (use 1 for every interval, 0 for never).
	 * @return Returns true on success, false otherwise.
	 */
	extern PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

	/**
	 * Initializes the middle-man OpenGL functions.
	 */
	void initMidGL();

	/**
	 * Compile the source code of a shader and attach it to the specified shader.
	 * @param shader The shader to attach the compiled source code to.
	 * @param shaderSource The shader's source code as a null terminated string.
	 */
	void compileShaders(unsigned int shader, const char* shaderSource);
	/** 
	 * Link a vertex shader and fragment shader to a specified program.
	 * @param program The program to attach and link the shaders to.
	 * @param vertexShader The vertex shader to link to the program.
	 * @param fragmentShader The fragment shader to link to the program.
	 */
	void linkShaders(unsigned int program, unsigned int vertexShader, unsigned int fragmentShader);
	/**
	 * Creates a vertex and fragment shader from the specified files and attach them to a program.
	 * @param vertexShader The filename of the vertex shader to load and attach to a program.
	 * @param fragmentShader The filename of the fragment shader to load and attach to a program.
	 * @return Returns the program's handle ID on success.
	 */
	unsigned int createShaders(const char* vertexShader, const char* fragmentShader);

	/**
	* Creates a perspective matrix based on the given values and stores it in an array of floats.
	* @param array The array to store the perspective view in.
	* @param fieldOfView The field of view angle for what the perspective view can see.
	* @param aspectRatio The aspect ratio for what the perspective view can see.
	* @param nearValue The cutoff value for objects closer to the screen.
	* @param farValue The cutoff value for objects further from the screen.
	*/
	void createPerspectiveMatrix(float* array, float fieldOfView, float aspectRatio, float nearValue, float farValue);
	/**
	 * Creates a perspective matrix based on the given values and stores it in a matrix object.
	 * @param matrix The matrix to store the perspective view in.
	 * @param fieldOfView The field of view angle for what the perspective view can see.
	 * @param aspectRatio The aspect ratio for what the perspective view can see.
	 * @param nearValue The cutoff value for objects closer to the screen.
	 * @param farValue The cutoff value for objects further from the screen.
	 */
	void createPerspectiveMatrix(PV::Math::Matrix<float> &matrix, float fieldOfView, float aspectRatio, float nearValue, float farValue);

	/**
	 * Creates a lookAt matrix using from the desired position and rotation, and then stores it in the desired matrix.
	 * @param matrix The matrix to store the resulting lookAt matrix into.
	 * @param position The position of the camera to use in the lookAt matrix.
	 * @param rotation The rotation of the camera to use in the lookAt matrix.
	 */
	void createLookAtMatrix(PV::Math::Matrix<float> &matrix, Math::vec3 position, Math::vec3 rotation);

	/**
	* Creates a lookAt matrix at the specified eye position, looking at a specific target, and then stores it in the desired matrix.
	* @param matrix The matrix to store the resulting lookAt matrix into.
	* @param eye The position of the eye to use in the lookAt matrix.
	* @param target The target of where the camera is looking in the lookAt matrix.
	* @param up What direction up is relative to the camera.
	*/
	void createLookAtMatrix(PV::Math::Matrix<float> &matrix, Math::vec3 eye, Math::vec3 target, Math::vec3 up);
};

#endif