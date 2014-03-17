#include <stdio.h>
#include <GL/glew.h>
#include "pvmm/windowSystem.h"
#include "pv/OculusRift.h"
#include "pvmm/MidOpenGL.h"

using namespace PV;

bool done = false;

#define quadVertices 2

#define quadVetex_size 4
#define quadColor_size 4
#define quadTex_size 4

GLuint _VRTexture;
GLuint _VRFBO;
unsigned int verticesBufferHandle;
unsigned int colorsBufferHandle;
unsigned int texCoordsBufferHandle;
unsigned int verticesArrayHandle;

void initQuad()
{
	float quadVerts[quadVetex_size * quadVertices] = {
		0.25f, 0.25f,
		0.25f, 0.75f,
		0.75f, 0.25f,
		0.75f, 0.75f
	};
	float quadColor[quadColor_size * 3] = {
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	float quadTexture[quadTex_size * 2] = {
		0.0, 0.0,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	// Create the vertex array handle.
	glGenVertexArrays(1, &verticesArrayHandle);
	glBindVertexArray(verticesArrayHandle);

	// Create the vertices buffer.
	glGenBuffers(1, &verticesBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &colorsBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColor), quadColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &texCoordsBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexture), quadTexture, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	glGenTextures(1, &_VRTexture);
	glBindTexture(GL_TEXTURE_2D, _VRTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &_VRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
}

void onResize(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, float(width) / height, 0.1f, 100);
  glMatrixMode(GL_MODELVIEW);
}

LRESULT CALLBACK windowProcess(HWND winHandle, UINT message, WPARAM windowParam, LPARAM messageParam)
{
	switch(message)
	{
	case WM_SIZE:
		onResize(LOWORD(messageParam),HIWORD(messageParam));
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(winHandle, message, windowParam, messageParam);
}

int drawGLScene(OculusRift rift)
{
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();

	glDepthMask(GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
	glBindVertexArray(verticesArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return 1;
}

int main()
{
	Window testWindow;
#ifdef _DEBUG
	testWindow.create(L"testing - debug", 1280, 800, false, windowProcess);
#else
	testWindow.create(L"testing - release", 1280, 800, false, windowProcess);
#endif
	testWindow.setWindowDrawingStateGL();
	testWindow.setVisible(true);

	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	initMidGL();
	wglSwapIntervalEXT(1);

	OculusRift rift(true);
	if (rift.isConnected())
	{
		testWindow.SetFullscreen(false);
		onResize(1280 / 2, 800);
	}
	else
	{
		onResize(1280 / 2, 800);
	}

	printf("Rift Connected: %s\n", (rift.isConnected()) ? "Yes" : "No");
	int version[] = { 0, 0 };
	glGetIntegerv(PV_GL_MAJOR_VERSION, &version[0]);
	glGetIntegerv(PV_GL_MINOR_VERSION, &version[1]);
	printf("Version: %d.%d\n", version[0], version[1]);

	initQuad();

	MSG msg;
	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		rift.Update();
		
		testWindow.MakeCurrentGLContext();
		glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
		drawGLScene(rift);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		rift.ShiftView(Left);
		rift.ComposeFinalImage(_VRTexture);
		rift.ShiftView(Right);
		rift.ComposeFinalImage(_VRTexture);

		testWindow.Update();
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	return 0;
}