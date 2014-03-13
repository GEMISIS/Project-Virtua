#include <Windows.h>
#include "pvmm/MidOpenGL.h"

PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

void initializeBaseOpenGL()
{
	wglSwapIntervalEXT = (PFGNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
}
