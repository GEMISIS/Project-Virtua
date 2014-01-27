#include <stdio.h>
#include "windowSystem.h"
#include "OculusRift.h"

#include <GL/GL.h>
#include <GL/GLU.h>

bool done = false;

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

int drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-1.5f, 0.0f, -6.0f);
	glBegin(GL_TRIANGLES);
	{
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	glEnd();

	glTranslatef(3.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
	}
	glEnd();
	return 1;
}

int main()
{
	OculusRift rift;

	Window testWindow;
	testWindow.create(L"testing", 640, 480, false, windowProcess);
	testWindow.setWindowDrawingStateGL();
	testWindow.setVisible(true);

	onResize(640, 480);
	glEnable(GL_DEPTH_TEST);

	printf("Rift Connected: %s", (rift.connected) ? "Yes" : "No");

	MSG msg;
	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT || msg.message == WM_NCLBUTTONUP)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		testWindow.MakeCurrentGLContext();
		drawGLScene();

		testWindow.updateWindow();
	}

	testWindow.destroyGLSystem();
	testWindow.destroy();
	exit(0);
	return 0;
}