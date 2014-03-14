#ifdef _WIN32
#include <Windows.h>
#endif
#include "pvmm/MidOpenGL.h"

namespace PV
{
	PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

	void initMidGL()
	{
		initMinGL();
		wglSwapIntervalEXT = (PFGNWGLSWAPINTERVALEXTPROC)glGetProcAddress("wglSwapIntervalEXT");
	}
};