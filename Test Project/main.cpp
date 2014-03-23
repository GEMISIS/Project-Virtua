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
		-0.75, -0.75, -1.0f,
		-0.75, 0.75, -1.0f,
		0.75, -0.75, -1.0f,
		0.75, 0.75, -1.0f
	};
	float quadColor[quadColor_size * 3] = {
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
}

LRESULT CALLBACK windowProcess(HWND winHandle, UINT message, WPARAM windowParam, LPARAM messageParam)
{
	switch(message)
	{
	case WM_SIZE:
		glViewport(0, 0, LOWORD(messageParam), HIWORD(messageParam));
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(winHandle, message, windowParam, messageParam);
}

void drawGLScene(OculusRift &rift)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(verticesArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	Window testWindow;
#ifdef _DEBUG
	testWindow.create(L"testing - debug", windowProcess);
#else
	testWindow.create(L"testing - release", windowProcess);
#endif
	bool active = PV::DetectDevice();
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
		glViewport(0, 0, 1280 / 2, 800);
	}
	else
	{
		glViewport(0, 0, 1280, 800);
	}

	printf("Rift Connected: %s\n", (rift.isConnected()) ? "Yes" : "No");
	int version[] = { 0, 0 };
	glGetIntegerv(PV_GL_MAJOR_VERSION, &version[0]);
	glGetIntegerv(PV_GL_MINOR_VERSION, &version[1]);
	printf("Version: %d.%d\n", version[0], version[1]);

	initQuad();
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
#define M_PI 3.14159265358979323846

	float fov = rift.FieldOfView;
	float aspect = rift.AspectRatio;
	float nearV = 0.1f;
	float farV = 100000.0f;

	float angle = (fov / 180.0f) * M_PI;
	float f = 1.0f / tan(angle * 0.5f);

	float perspectiveMatrixCore[16] = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (farV + nearV) / (nearV - farV), -1.0f,
		0, 0, (2.0f * farV * nearV) / (nearV - farV), 0
	};
	float perspectiveMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	float viewMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	unsigned int program = createShaders("vertexShader.vs", "fragShader.fs");

	MSG msg;
	while (!done)
	{
		float ang = rift.GetRotation().y * M_PI / 180.0f;
		float rotMatrix[16] = {
			cos(ang), 0, -sin(ang), 0,
			0, 1, 0, 0,
			sin(ang), 0, cos(ang), 0,
			0, 0, 0, 1
		};
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
		int perspCore = 0;
		int perspTranslation = 0;
		int viewTranslation = 0;
		int rot = 0;
		if (rift.isConnected())
		{
			perspectiveMatrix[12] = 0.0f;
			viewMatrix[12] = 0.0f;
			glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
			glViewport(0, 0, 640, 800);
			rift.ShiftView(Left, perspectiveMatrix, viewMatrix);
			glUseProgram(program);
			perspCore = pv_glGetUniformLocation(program, "perspCore");
			pv_glUniformMatrix4fv(perspCore, 1, false, perspectiveMatrixCore);
			perspTranslation = pv_glGetUniformLocation(program, "perspTranslation");
			pv_glUniformMatrix4fv(perspTranslation, 1, false, perspectiveMatrix);
			viewTranslation = pv_glGetUniformLocation(program, "viewTranslation");
			pv_glUniformMatrix4fv(viewTranslation, 1, false, viewMatrix);
			rot = pv_glGetUniformLocation(program, "rotMatrix");
			pv_glUniformMatrix4fv(rot, 1, false, rotMatrix);
			drawGLScene(rift);

			perspectiveMatrix[12] = 0.0f;
			viewMatrix[12] = 0.0f;
			glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
			glViewport(0, 0, 640, 800);
			rift.ShiftView(Right, perspectiveMatrix, viewMatrix);
			glUseProgram(program);
			perspCore = pv_glGetUniformLocation(program, "perspCore");
			pv_glUniformMatrix4fv(perspCore, 1, false, perspectiveMatrixCore);
			perspTranslation = pv_glGetUniformLocation(program, "perspTranslation");
			pv_glUniformMatrix4fv(perspTranslation, 1, false, perspectiveMatrix);
			viewTranslation = pv_glGetUniformLocation(program, "viewTranslation");
			pv_glUniformMatrix4fv(viewTranslation, 1, false, viewMatrix);
			rot = pv_glGetUniformLocation(program, "rotMatrix");
			pv_glUniformMatrix4fv(rot, 1, false, rotMatrix);
			drawGLScene(rift);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			rift.ComposeFinalImage(_VRTexture, _VRTexture2);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(program);
			perspTranslation = pv_glGetUniformLocation(program, "perspTranslation");
			pv_glUniformMatrix4fv(perspTranslation, 1, false, perspectiveMatrix);
			viewTranslation = pv_glGetUniformLocation(program, "viewTranslation");
			pv_glUniformMatrix4fv(viewTranslation, 1, false, viewMatrix);
			drawGLScene(rift);
		}

		testWindow.Update();
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	return 0;
}