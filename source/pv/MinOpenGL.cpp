#include "pv/MinOpenGL.h"

namespace PV
{
	pv_glCreateShaderFunction pv_glCreateShader = NULL;
	pv_glShaderSourceFunction pv_glShaderSource = NULL;
	pv_glCompileShaderFunction pv_glCompileShader = NULL;
	pv_glDeleteShaderFunction pv_glDeleteShader = NULL;
	pv_glCreateProgramFunction pv_glCreateProgram = NULL;
	pv_glAttachShaderFunction pv_glAttachShader = NULL;
	pv_glLinkProgramFunction pv_glLinkProgram = NULL;

	void initMinGL()
	{
		pv_glCreateShader = (pv_glCreateShaderFunction)glGetProcAddress("glCreateShader");
		pv_glShaderSource = (pv_glShaderSourceFunction)glGetProcAddress("glShaderSource");
		pv_glCompileShader = (pv_glCompileShaderFunction)glGetProcAddress("glCompileShader");
		pv_glDeleteShader = (pv_glDeleteShaderFunction)glGetProcAddress("glDeleteShader");
		pv_glCreateProgram = (pv_glCreateProgramFunction)glGetProcAddress("glCreateProgram");
		pv_glAttachShader = (pv_glAttachShaderFunction)glGetProcAddress("glAttachShader");
		pv_glLinkProgram = (pv_glLinkProgramFunction)glGetProcAddress("glLinkProgram");
	}
};


