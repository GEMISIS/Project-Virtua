#include "pv/MinOpenGL.h"

typedef bool(__stdcall* PFGNWGLSWAPINTERVALEXTPROC)(int interval);

extern PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

void initMidGL();
