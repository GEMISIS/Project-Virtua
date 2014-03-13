#include "pv/MinOpenGL.h"

pv_glCreateShaderFunction pv_glCreateShader = NULL;
pv_glShaderSourceFunction pv_glShaderSource = NULL;
pv_glCompileShaderFunction pv_glCompileShader = NULL;
pv_glDeleteShaderFunction pv_glDeleteShader = NULL;
pv_glCreateProgramFunction pv_glCreateProgram = NULL;
pv_glAttachShaderFunction pv_glAttachShader = NULL;
pv_glLinkProgramFunction pv_glLinkProgram = NULL;

void initMinGL()
{
	pv_glCreateShader = (pv_glCreateShaderFunction)glXGetProcAddress("glCreateShader");
	pv_glShaderSource = (pv_glShaderSourceFunction)glXGetProcAddress("glShaderSource");
	pv_glCompileShader = (pv_glCompileShaderFunction)glXGetProcAddress("glCompileShader");
	pv_glDeleteShader = (pv_glDeleteShaderFunction)glXGetProcAddress("glDeleteShader");
	pv_glCreateProgram = (pv_glCreateProgramFunction)glXGetProcAddress("glCreateProgram");
	pv_glAttachShader = (pv_glAttachShaderFunction)glXGetProcAddress("glAttachShader");
	pv_glLinkProgram = (pv_glLinkProgramFunction)glXGetProcAddress("glLinkProgram");
}