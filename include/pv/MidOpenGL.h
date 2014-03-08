#include "pv/MinOpenGL.h"

typedef bool (APIENTRY* PFGNWGLSWAPINTERVALEXTPROC)(int interval);

extern PFGNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

void initializeBaseOpenGL();
