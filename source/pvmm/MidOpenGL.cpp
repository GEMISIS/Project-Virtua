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
			char* data = (char*)malloc(length * sizeof(char) + 1);
			strcpy(data, vertFile->Data());
			data[length] = '\0';
			compileShaders(vertex, data);
		}
		else
		{
			fprintf(stdout, "Could not find vertex shader file %s!\n", vertexShader);
		}
		File* fragFile = new File(fragmentShader, true);
		if (fragFile->DataLoaded())
		{
			int length = fragFile->Size();
			char* data = (char*)malloc(length * sizeof(char)+1);
			strcpy(data, fragFile->Data());
			data[length] = '\0';
			compileShaders(fragment, data);
		}
		else
		{
			fprintf(stdout, "Could not find fragment shader file %s!\n", fragmentShader);
		}
		linkShaders(program, vertex, fragment);
		return program;
	}
};
