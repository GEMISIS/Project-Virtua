#include <stdio.h>
#include <GL/glew.h>
#include "pv/Kinect1.h"
#include "pvmm/windowSystem.h"
#include "pv/OculusRift.h"
#include "pvmm/MidOpenGL.h"

using namespace PV;

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

void initQuad(int textureWidth, int textureHeight)
{
	float quadVerts[quadVetex_size * quadVertices] = {
		-1.0, -1.0, -1.0f,
		-1.0, 1.0, -1.0f,
		1.0, -1.0, -1.0f,
		1.0, 1.0, -1.0f
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &_VRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);

	glGenTextures(1, &_VRTexture2);
	glBindTexture(GL_TEXTURE_2D, _VRTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &_VRFBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _VRTexture2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);
}

void drawGLScene(OculusRift &rift)
{
	glBindVertexArray(verticesArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	Window testWindow;
#ifdef _DEBUG
	testWindow.create(L"testing - debug");
#else
	testWindow.create(L"testing - release");
#endif
	testWindow.setWindowDrawingStateGL();
	testWindow.setVisible(true);

	if (glewInit() != GLEW_OK)
	{
		return -1;
	}
	initMidGL();
	wglSwapIntervalEXT(1);

	glEnable(GL_LINE_SMOOTH);

	OculusRift rift(false, testWindow.renderingContext, testWindow.windowHandle, testWindow.deviceContext);

	printf("Rift Connected: %s\n", (rift.isConnected()) ? "Yes" : "No");
	int version[] = { 0, 0 };
	glGetIntegerv(PV_GL_MAJOR_VERSION, &version[0]);
	glGetIntegerv(PV_GL_MINOR_VERSION, &version[1]);
	printf("OpenGL Version: %d.%d\n", version[0], version[1]);

	initQuad(rift.renderTargetSize.w, rift.renderTargetSize.h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	unsigned int program = createShaders("vertexShader.vs", "fragShader.fs");

	Math::Matrix<float> mvp(4, 4);
	Math::Matrix<float> perspectiveMatrix(4, 4);
	Math::Matrix<float> viewMatrix(4, 4);
	Math::Matrix<float> viewOffsetMatrix(4, 4);
	Math::Matrix<float> modelMatrix(4, 4);

	Math::vec3 position = { 0, 0, -2 };
	Math::vec3 rotation = { 0, 0, 0 };

	if (rift.isConnected())
	{
		//createPerspectiveMatrix(perspectiveMatrix, rift.FieldOfView, rift.AspectRatio, 0.1f, 1000.0f);
	}
	else
	{
		createPerspectiveMatrix(perspectiveMatrix, 45.0f, 1280.0f / 800.0f, 0.1f, 1000.0f);
	}

	Kinect1* kinect = new Kinect1();
	if (kinect->Status() == Ready)
	{
		printf("\nKinect Found!\n");
	}

	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (rift.isConnected())
		{
			rift.Update();

			rotation.x = -rift.GetRotation().x * M_PI / 180.0f;
			rotation.y = -rift.GetRotation().y * M_PI / 180.0f;
			rotation.z = -rift.GetRotation().z * M_PI / 180.0f;
		}

		const NUI_SKELETON_DATA skeleton = kinect->getMainPerson();
		if (skeleton.eTrackingState == NUI_SKELETON_TRACKING_STATE::NUI_SKELETON_TRACKED)
		{
			position.x = -skeleton.Position.x;
			position.y = 0;
			position.z = -skeleton.Position.z;
		}

		if ((1 << 16) & GetAsyncKeyState(VK_LEFT))
		{
			rotation.y += 1 * M_PI / 180.0f;
		}
		if ((1 << 16) & GetAsyncKeyState(VK_RIGHT))
		{
			rotation.y -= 1 * M_PI / 180.0f;
		}
		if ((1 << 16) & GetAsyncKeyState(VK_UP))
		{
			position.x += sin(rotation.y);
			position.z += cos(rotation.y);
		}
		if ((1 << 16) & GetAsyncKeyState(VK_DOWN))
		{
			position.x -= sin(rotation.y);
			position.z -= cos(rotation.y);
		}

		createLookAtMatrix(viewMatrix, position, rotation);
		rift.SetRenderTextures(_VRTexture, _VRTexture2);

		testWindow.MakeCurrentGLContext();
		int mvpLocation = 0;
		if (rift.StartRender())
		{
			rift.StartEyeRender(Left, viewOffsetMatrix);
			{
				glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO);
				glViewport(0, 0, 640, 800);

				pv_glUseProgram(program);
				rift.getPerspectiveMatrix(Left, perspectiveMatrix);
				mvp = perspectiveMatrix * viewMatrix;
				mvpLocation = pv_glGetUniformLocation(program, "mvp");
				pv_glUniformMatrix4fv(mvpLocation, 1, false, mvp.getArray());

				drawGLScene(rift);
			}
			rift.EndEyeRender(Left);

			rift.StartEyeRender(Right, viewOffsetMatrix);
			{
				glBindFramebuffer(GL_FRAMEBUFFER, _VRFBO2);

				pv_glUseProgram(program);
				rift.getPerspectiveMatrix(Right, perspectiveMatrix);
				mvp = perspectiveMatrix * viewMatrix;
				mvpLocation = pv_glGetUniformLocation(program, "mvp");
				pv_glUniformMatrix4fv(mvpLocation, 1, false, mvp.getArray());

				drawGLScene(rift);
			}
			rift.EndEyeRender(Right);

			rift.EndRender();
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(program);
			mvp = perspectiveMatrix * viewMatrix;
			mvpLocation = pv_glGetUniformLocation(program, "mvp");
			pv_glUniformMatrix4fv(mvpLocation, 1, false, mvp.getArray());

			drawGLScene(rift);
			testWindow.Update();
		}
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	return 0;
}