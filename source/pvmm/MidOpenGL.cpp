#ifdef _WIN32
#include <Windows.h>
#endif
#include "pvmm/MidOpenGL.h"
#include "pv/FileIO.h"

namespace PV
{
	PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

	void initMidGL()
	{
		initMinGL();
		wglSwapIntervalEXT = (PFGNWGLSWAPINTERVALEXTPROC)glGetProcAddress("wglSwapIntervalEXT");
	}

	void compileShaders(unsigned int shader, const char* shaderSource)
	{
		pv_glShaderSource(shader, 1, &shaderSource, NULL);
		pv_glCompileShader(shader);

		int result = false;
		int logLength = 0;
		pv_glGetShaderiv(shader, PV_GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			pv_glGetShaderiv(shader, PV_GL_INFO_LOG_LENGTH, &logLength);
			char* FragmentShaderErrorMessage = (char*)malloc(sizeof(char)* logLength);
			pv_glGetShaderInfoLog(shader, logLength, NULL, FragmentShaderErrorMessage);
			fprintf(stdout, "%s\nFragment shader failed to load!\n", FragmentShaderErrorMessage);
			free(FragmentShaderErrorMessage);
		}
	}

	void linkShaders(unsigned int program, unsigned int vertexShader, unsigned int fragmentShader)
	{
		pv_glAttachShader(program, vertexShader);
		pv_glAttachShader(program, fragmentShader);

		pv_glLinkProgram(program);

		int Presult = false;
		int PlogLength = 0;
		pv_glGetProgramiv(program, PV_GL_LINK_STATUS, &Presult);
		if (Presult == GL_FALSE)
		{
			pv_glGetProgramiv(program, PV_GL_INFO_LOG_LENGTH, &PlogLength);
			char* VertexShaderErrorMessage = (char*)malloc(sizeof(char)* PlogLength);
			pv_glGetProgramInfoLog(program, PlogLength, NULL, VertexShaderErrorMessage);
			fprintf(stdout, "%s\nProgram link failed!\n", VertexShaderErrorMessage);
			free(VertexShaderErrorMessage);
		}
	}

	unsigned int createShaders(const char* vertexShader, const char* fragmentShader)
	{
		unsigned int program = pv_glCreateProgram();
		unsigned int vertex = pv_glCreateShader(PV_GL_VERTEX_SHADER);
		unsigned int fragment = pv_glCreateShader(PV_GL_FRAGMENT_SHADER);
		File* vertFile = new File(vertexShader, true);
		if (vertFile->DataLoaded())
		{
			int length = vertFile->Size();
			char* data = (char*)malloc((length + 1) * sizeof(char));
			strncpy(data, vertFile->Data(), length);
			data[length] = '\0';
			compileShaders(vertex, data);
			free(data);
		}
		else
		{
			fprintf(stdout, "Could not find vertex shader file %s!\n", vertexShader);
		}
		File* fragFile = new File(fragmentShader, true);
		if (fragFile->DataLoaded())
		{
			int length = fragFile->Size();
			char* data = (char*)malloc((length + 1) * sizeof(char));
			strncpy(data, fragFile->Data(), length);
			data[length] = '\0';
			compileShaders(fragment, data);
			free(data);
		}
		else
		{
			fprintf(stdout, "Could not find fragment shader file %s!\n", fragmentShader);
		}
		linkShaders(program, vertex, fragment);
		return program;
	}

	void createPerspectiveMatrix(float* array, float fieldOfView, float aspectRatio, float nearValue, float farValue)
	{
		float width = (nearValue * tan(fieldOfView * M_PI / 360.0f)) - -(nearValue * tan(fieldOfView * M_PI / 360.0f));
		float height = (nearValue * tan(fieldOfView * M_PI / 360.0f)) - -(nearValue * tan(fieldOfView * M_PI / 360.0f));
		float depth = farValue - nearValue;

		for (int i = 0; i < 16; i += 1)
		{
			switch (i)
			{
			case 0:
				array[i] = (2 * nearValue / width) / aspectRatio;
				break;
			case 5:
				array[i] = 2 * nearValue / height;
				break;
			case 10:
				array[i] = -(farValue + nearValue) / depth;
				break;
			case 11:
				array[i] = -1;
				break;
			case 14:
				array[i] = -2 * (farValue * nearValue) / depth;
				break;
			default:
				array[i] = 0;
				break;
			}
		}
	}
	void createPerspectiveMatrix(PV::Math::Matrix<float> &matrix, float fieldOfView, float aspectRatio, float nearValue, float farValue)
	{
		float width = (nearValue * tan(fieldOfView * M_PI / 360.0f)) - -(nearValue * tan(fieldOfView * M_PI / 360.0f));
		float height = (nearValue * tan(fieldOfView * M_PI / 360.0f)) - -(nearValue * tan(fieldOfView * M_PI / 360.0f));
		float depth = farValue - nearValue;

		matrix = PV::Math::Matrix <float>(4, 4);
		matrix = new float[16];
		for (int i = 0; i < 16; i += 1)
		{
			switch (i)
			{
			case 0:
				matrix[i] = (2 * nearValue / width) / aspectRatio;
				break;
			case 5:
				matrix[i] = 2 * nearValue / height;
				break;
			case 10:
				matrix[i] = -(farValue + nearValue) / depth;
				break;
			case 11:
				matrix[i] = -1;
				break;
			case 14:
				matrix[i] = -2 * (farValue * nearValue) / depth;
				break;
			default:
				matrix[i] = 0;
				break;
			}
		}
	}
	void createLookAtMatrix(PV::Math::Matrix<float> &matrix, Math::vec3 eye, Math::vec3 target, Math::vec3 up)
	{
		Math::vec3 zAxis = { eye.x - target.x, eye.y - target.y, eye.z - target.z };
		zAxis = Math::normalize(zAxis);
		Math::vec3 xAxis = Math::normalize(Math::crossMultiply(up, zAxis));
		Math::vec3 yAxis = Math::crossMultiply(zAxis, xAxis);

		Math::Matrix<float> orientationMatrix(4, 4);
		Math::Matrix<float> translationMatrix(4, 4);

		orientationMatrix[0] = xAxis.x;
		orientationMatrix[1] = yAxis.x;
		orientationMatrix[2] = zAxis.x;
		orientationMatrix[4] = xAxis.y;
		orientationMatrix[5] = yAxis.y;
		orientationMatrix[6] = zAxis.y;
		orientationMatrix[8] = xAxis.z;
		orientationMatrix[9] = yAxis.z;
		orientationMatrix[10] = zAxis.z;

		translationMatrix[12] = -eye.x;
		translationMatrix[13] = -eye.y;
		translationMatrix[14] = -eye.z;

		matrix = orientationMatrix * translationMatrix;
	}
	void createLookAtMatrix(PV::Math::Matrix<float> &matrix, Math::vec3 position, Math::vec3 rotation)
	{
		Math::Matrix<float> positionMatrix(4, 4);
		Math::Matrix<float> rotationMatrix(4, 4);

		positionMatrix.Translate(position.x, position.y, position.z);
		rotationMatrix.Rotate(rotation.x, rotation.y, rotation.z);

		matrix = rotationMatrix * positionMatrix;
	}
};
