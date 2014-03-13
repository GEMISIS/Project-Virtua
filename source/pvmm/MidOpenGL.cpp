#include <Windows.h>
#include "pvmm/MidOpenGL.h"

PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

void initMidGL()
{
	initMinGL();
	wglSwapIntervalEXT = (PFGNWGLSWAPINTERVALEXTPROC)glXGetProcAddress("wglSwapIntervalEXT");
}
