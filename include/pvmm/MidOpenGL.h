#ifndef _MID_OPEN_GL_H_
#define _MID_OPEN_GL_H_

#include "pv/MinOpenGL.h"

namespace PV
{
	typedef bool(__stdcall* PFGNWGLSWAPINTERVALEXTPROC)(int interval);

	extern PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

	void initMidGL();
	unsigned int createShaders(const char* vertexShader, const char* fragmentShader);
};

#endif