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

	unsigned int createShaders(const char* vertexShader, const char* fragmentShader)
	{
		unsigned int program = pv_glCreateProgram();
		unsigned int fragment = pv_glCreateShader(PV_GL_FRAGMENT_SHADER);
		unsigned int vertex = pv_glCreateShader(PV_GL_VERTEX_SHADER);
		File* fragFile = new File(fragmentShader, true);
		if (fragFile->DataLoaded())
		{
			int length = fragFile->Size();
			char* data = (char*)malloc(length * sizeof(char)+1);
			strcpy(data, fragFile->Data());
			data[length] = '\0';
			pv_glShaderSource(fragment, 1, &data, &length);
			pv_glCompileShader(fragment);

			int result = false;
			int logLength = 0;
			pv_glGetShaderiv(fragment, PV_GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE)
			{
				pv_glGetShaderiv(fragment, PV_GL_INFO_LOG_LENGTH, &logLength);
				char* FragmentShaderErrorMessage = (char*)malloc(sizeof(char)* logLength);
				pv_glGetShaderInfoLog(fragment, logLength, NULL, FragmentShaderErrorMessage);
				fprintf(stdout, "%s\nFragment shader failed to load!\n", FragmentShaderErrorMessage);
				free(FragmentShaderErrorMessage);
			}
		}
		else
		{
			fprintf(stdout, "Could not find fragment shader file %s!\n", fragmentShader);
		}
		File* vertFile = new File(vertexShader, true);
		if (vertFile->DataLoaded())
		{
			int length = vertFile->Size();
			char* data = (char*)malloc(length * sizeof(char)+1);
			strcpy(data, vertFile->Data());
			data[length] = '\0';
			pv_glShaderSource(vertex, 1, &data, &length);
			pv_glCompileShader(vertex);

			int result = false;
			int logLength = 0;
			pv_glGetShaderiv(vertex, PV_GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE)
			{
				pv_glGetShaderiv(vertex, PV_GL_INFO_LOG_LENGTH, &logLength);
				char* VertexShaderErrorMessage = (char*)malloc(sizeof(char)* logLength);
				pv_glGetShaderInfoLog(vertex, logLength, NULL, VertexShaderErrorMessage);
				fprintf(stdout, "%s\nVertex shader failed!\n", VertexShaderErrorMessage);
				free(VertexShaderErrorMessage);
			}
		}
		else
		{
			fprintf(stdout, "Could not find vertex shader file %s!\n", vertexShader);
		}
		pv_glAttachShader(program, fragment);
		pv_glAttachShader(program, vertex);

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
		return program;
	}
};
