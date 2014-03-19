#include <stdio.h>
#include <GL/glew.h>
#include "pvmm/windowSystem.h"
#include "pv/OculusRift.h"
#include "pvmm/MidOpenGL.h"

using namespace PV;

bool done = false;

#define quadVertices 3

#define quadVetex_size 4
#define quadColor_size 4
#define quadTex_size 4

GLuint _VRTexture;
GLuint _VRFBO;
GLuint _VRTexture2;
GLuint _VRFBO2;
unsigned int verticesBufferHandle;
unsigned int colorsBufferHandle;
unsigned int texCoordsBufferHandle;
unsigned int verticesArrayHandle;

void initQuad()
{
	float quadVerts[quadVetex_size * quadVertices] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, 0.75f, -1.0f,
		0.75f, -1.0f, -1.0f,
		0.75f, 0.75f, -1.0f
	};
	float quadColor[quadColor_size * 3] = {
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &_VRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);

	glGenTextures(1, &_VRTexture2);
	glBindTexture(GL_TEXTURE_2D, _VRTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &_VRFBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
}

void onResize(int width, int height)
{
  glViewport(0, 0, width, height);
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

int drawGLScene(OculusRift rift, float offsetMatrix[16])
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glDepthMask(GL_TRUE);

	initMidGL();
	wglSwapIntervalEXT(1);

	OculusRift rift(false);
	if (rift.isConnected())
	{
		testWindow.SetFullscreen(false);
		onResize(1280, 800);
	}
	else
	{
		onResize(1280, 800);
	}

	printf("Rift Connected: %s\n", (rift.isConnected()) ? "Yes" : "No");
	int version[] = { 0, 0 };
	glGetIntegerv(PV_GL_MAJOR_VERSION, &version[0]);
	glGetIntegerv(PV_GL_MINOR_VERSION, &version[1]);
	printf("Version: %d.%d\n", version[0], version[1]);

	initQuad();

	float offsetMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	unsigned int program = createShaders("vertexShader.vs", "fragShader.fs");

	MSG msg;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
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
<<<<<<< HEAD
		int translation = 0;
		if (rift.isConnected())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
			if (rift.isConnected())
			{
				rift.ShiftView(Left, offsetMatrix);
			}
			glUseProgram(program);
			translation = pv_glGetUniformLocation(program, "translation");
			pv_glUniformMatrix4fv(translation, 1, false, offsetMatrix);
			drawGLScene(rift, offsetMatrix);

			glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
			if (rift.isConnected())
			{
				rift.ShiftView(Right, offsetMatrix);
			}
			glUseProgram(program);
			translation = pv_glGetUniformLocation(program, "translation");
			pv_glUniformMatrix4fv(translation, 1, false, offsetMatrix);
			drawGLScene(rift, offsetMatrix);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
=======
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1280, 800);
		drawGLScene(rift);

		glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
>>>>>>> 7cf7af7b0262bf27c858bd38ddc27eea3c59fc62

			gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			rift.ComposeFinalImage(Left, _VRTexture);
			rift.ComposeFinalImage(Right, _VRTexture2);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(program);
			translation = pv_glGetUniformLocation(program, "translation");
			pv_glUniformMatrix4fv(translation, 1, false, offsetMatrix);
			drawGLScene(rift, offsetMatrix);
		}

		testWindow.Update();
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	return 0;
}