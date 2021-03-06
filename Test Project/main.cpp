#include <stdio.h>
#include "pv/Kinect1.h"
#include "pv/OculusRift.h"
#include "pvmm/windowSystem.h"
#include "pvmm/MidOpenGL.h"
#include "pvmm/WavefrontObject.h"

using namespace PV;

#define quadVertices 3

#define quadVetex_size 4
#define quadColor_size 4
#define quadTex_size 4

unsigned int verticesBufferHandle;
unsigned int colorsBufferHandle;
unsigned int texCoordsBufferHandle;
unsigned int verticesArrayHandle;
float rotation = 0.0f;

void initQuad()
{
	float quadVerts[quadVetex_size * quadVertices] = {
		-100.0, -1.0, -100.0,
		-100.0, -1.0, 100.0,
		100.0, -1.0, -100.0,
		100.0, -1.0, 100.0
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
	pv_glGenVertexArrays(1, &verticesArrayHandle);
	pv_glBindVertexArray(verticesArrayHandle);

	pv_glEnableVertexAttribArray(0);
	pv_glEnableVertexAttribArray(1);
	pv_glEnableVertexAttribArray(2);

	// Create the vertices buffer.
	pv_glGenBuffers(1, &verticesBufferHandle);
	pv_glBindBuffer(PV_GL_ARRAY_BUFFER, verticesBufferHandle);
	pv_glBufferData(PV_GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, PV_GL_STATIC_DRAW);
	pv_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	pv_glGenBuffers(1, &colorsBufferHandle);
	pv_glBindBuffer(PV_GL_ARRAY_BUFFER, colorsBufferHandle);
	pv_glBufferData(PV_GL_ARRAY_BUFFER, sizeof(quadColor), quadColor, PV_GL_STATIC_DRAW);
	pv_glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	pv_glGenBuffers(1, &texCoordsBufferHandle);
	pv_glBindBuffer(PV_GL_ARRAY_BUFFER, texCoordsBufferHandle);
	pv_glBufferData(PV_GL_ARRAY_BUFFER, sizeof(quadTexture), quadTexture, PV_GL_STATIC_DRAW);
	pv_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	pv_glBindVertexArray(0);
	pv_glBindBuffer(PV_GL_ARRAY_BUFFER, 0);
}

void handleInput(OculusRift* rift, Kinect1* kinect, Math::vec3 &position, Math::vec3 &rotation)
{
	if (rift->isConnected())
	{
		rift->Update();
	}

	const NUI_SKELETON_DATA skeleton = kinect->getMainPerson();
	if (skeleton.eTrackingState == NUI_SKELETON_TRACKED)
	{
		position.x = -skeleton.Position.x;
		position.y = 0;
		position.z = -skeleton.Position.z;
	}

	if ((1 << 16) & GetAsyncKeyState(VK_LEFT))
	{
		rift->DismissWarningScreen();
		rotation.y += 1.0f * (float)M_PI / 180.0f;
	}
	if ((1 << 16) & GetAsyncKeyState(VK_RIGHT))
	{
		rift->DismissWarningScreen();
		rotation.y -= 1.0f * (float)M_PI / 180.0f;
	}
	if ((1 << 16) & GetAsyncKeyState(VK_UP))
	{
		rift->DismissWarningScreen();
		position.x += sin(rotation.y);
		position.z += cos(rotation.y);
	}
	if ((1 << 16) & GetAsyncKeyState(VK_DOWN))
	{
		rift->DismissWarningScreen();
		position.x -= sin(rotation.y);
		position.z -= cos(rotation.y);
	}

}

void drawGLScene(unsigned int program, Math::Matrix<float> perspectiveMatrix, Math::Matrix<float> viewMatrix)
{
	Math::Matrix<float> modelMatrix(4, 4);
	modelMatrix.Translate(1.0f, 1.0f, 1.0f);
	modelMatrix.Rotate(0, 0, 0);
	modelMatrix.Translate(-1.0f, -1.0f, -1.0f);

	if (rotation >= 360.0f)
	{
		rotation = -0.01f;
	}
	rotation += 0.01f;

	pv_glUseProgram(program);

	Math::Matrix<float> mvp = perspectiveMatrix * viewMatrix * modelMatrix;
	unsigned int mvpLocation = pv_glGetUniformLocation(program, "mvp");
	pv_glUniformMatrix4fv(mvpLocation, 1, false, mvp.getArray());

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pv_glEnableVertexAttribArray(0);
	pv_glEnableVertexAttribArray(1);
	pv_glEnableVertexAttribArray(2);

	pv_glBindVertexArray(verticesArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	pv_glBindVertexArray(0);
}

int main()
{
	InitRift();
	Window testWindow;
	//Matrices to handle camera view and warping for OculusRift
	Math::Matrix<float> perspectiveMatrix(4, 4);
	Math::Matrix<float> viewMatrix(4, 4);
	Math::Matrix<float> viewOffsetMatrix(4, 4);

	//Position and rotation of the camera
	Math::vec3 position = { 0, 0, -2.0f };
	Math::vec3 rotation = { 0, 0, 0 };

	Kinect1* kinect = new Kinect1();
	MSG msg;

	testWindow.create(L"Project Virtua - Test Project", 1280, 800, false, *(windowProcessCallback*)NULL);
	testWindow.setWindowDrawingStateGL();
	testWindow.setVisible(true);

	initMidGL();
	wglSwapIntervalEXT(1);

	glEnable(GL_LINE_SMOOTH);

	//Create OculusRift object, passing the rendering context, device context, and handle of this window
	OculusRift rift(false, testWindow.renderingContext, testWindow.windowHandle, testWindow.deviceContext);

	initQuad();
	unsigned int program = createShaders("vertexShader.vs", "fragShader.fs");
	createPerspectiveMatrix(perspectiveMatrix, 45.0f, 1280.0f / 800.0f, 0.1f, 1000.0f);

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

		handleInput(&rift, kinect, position, rotation);
		createLookAtMatrix(viewOffsetMatrix, position, rotation);

		testWindow.MakeCurrentGLContext();
		//Attempts to render to OculusRift
		if (rift.StartRender())
		{
			//Renders left eye
			rift.StartEyeRender(Left, viewMatrix);
			{
				rift.getPerspectiveMatrix(Left, perspectiveMatrix);
				drawGLScene(program, perspectiveMatrix, viewMatrix * viewOffsetMatrix);
			}
			rift.EndEyeRender(Left);

			//Renders right eye
			rift.StartEyeRender(Right, viewMatrix);
			{
				rift.getPerspectiveMatrix(Right, perspectiveMatrix);
				drawGLScene(program, perspectiveMatrix, viewMatrix * viewOffsetMatrix);
			}
			rift.EndEyeRender(Right);

			//Frame buffering is not handled automatically!
			pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			rift.EndRender();
			glEnable(GL_DEPTH_TEST);
			glClearDepth(1);
		}
		//Rendering to window if OculusRift fails
		else
		{
			pv_glBindFramebuffer(PV_GL_FRAMEBUFFER, 0);
			drawGLScene(program, perspectiveMatrix, viewMatrix);
			testWindow.Update();
		}
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	return 0;
}