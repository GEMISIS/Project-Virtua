#include "pv/MinOpenGL.h"

namespace PV
{
	pv_glCreateShaderFunction pv_glCreateShader = NULL;
	pv_glShaderSourceFunction pv_glShaderSource = NULL;
	pv_glCompileShaderFunction pv_glCompileShader = NULL;
	pv_glDeleteShaderFunction pv_glDeleteShader = NULL;
	pv_glGetShaderivFunction pv_glGetShaderiv = NULL;
	pv_glGetShaderInfoLogFunction pv_glGetShaderInfoLog = NULL;
	pv_glCreateProgramFunction pv_glCreateProgram = NULL;
	pv_glAttachShaderFunction pv_glAttachShader = NULL;
	pv_glLinkProgramFunction pv_glLinkProgram = NULL;
	pv_glGetUniformLocationFunction pv_glGetUniformLocation = NULL;
	pv_glUniform1fFunction pv_glUniform1f = NULL;
	pv_glUniform2fFunction pv_glUniform2f = NULL;
	pv_glUniform3fFunction pv_glUniform3f = NULL;
	pv_glUniform4fFunction pv_glUniform4f = NULL;
	pv_glUniformMatrix4fvFunction pv_glUniformMatrix4fv = NULL;

	void initMinGL()
	{
		pv_glCreateShader = (pv_glCreateShaderFunction)glGetProcAddress("glCreateShader");
		pv_glShaderSource = (pv_glShaderSourceFunction)glGetProcAddress("glShaderSource");
		pv_glCompileShader = (pv_glCompileShaderFunction)glGetProcAddress("glCompileShader");
		pv_glDeleteShader = (pv_glDeleteShaderFunction)glGetProcAddress("glDeleteShader");
		pv_glGetShaderiv = (pv_glGetShaderivFunction)glGetProcAddress("glGetShaderiv");
		pv_glGetShaderInfoLog = (pv_glGetShaderInfoLogFunction)glGetProcAddress("glGetShaderInfoLog");
		pv_glCreateProgram = (pv_glCreateProgramFunction)glGetProcAddress("glCreateProgram");
		pv_glAttachShader = (pv_glAttachShaderFunction)glGetProcAddress("glAttachShader");
		pv_glLinkProgram = (pv_glLinkProgramFunction)glGetProcAddress("glLinkProgram");
		pv_glGetUniformLocation = (pv_glGetUniformLocationFunction)glGetProcAddress("glGetUniformLocation");
		pv_glUniform1f = (pv_glUniform1fFunction)glGetProcAddress("glUniform1f");
		pv_glUniform2f = (pv_glUniform2fFunction)glGetProcAddress("glUniform2f");
		pv_glUniform3f = (pv_glUniform3fFunction)glGetProcAddress("glUniform3f");
		pv_glUniform4f = (pv_glUniform4fFunction)glGetProcAddress("glUniform4f");
		pv_glUniformMatrix4fv = (pv_glUniformMatrix4fvFunction)glGetProcAddress("glUniformMatrix4fv");
	}
};


