#include <Windows.h>
#include "pv/MidOpenGL.h"

PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

void initializeBaseOpenGL()
{
	wglSwapIntervalEXT = (PFGNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
}
