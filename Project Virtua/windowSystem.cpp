#include "windowSystem.h"

#ifdef _WIN32

/**
 * Creates a window for use with OpenGL and returns an instance of it.
 *
 * This method takes in a title, width, hight, and full screen boolean, and then creates
 * a window using the given information.  An instance of the window is the returned from the method.
 * @param title The title to be displayed in the window.
 * @param width An unsigned integer for the width of the window.
 * @param height An unsigned integer for the height of the window.
 * @param fullscreen If true, the window will be made full screen, with the width and height determining the
 * screen resolution.  If false, it will simply create the window with the selected width and height as its
 * size.
 * @param bitsPerPixel The bits per pixel to use for the window.
 * @return On success, this method will return a full created window instance that can be used later on.
 * On fail, it will return NULL.
 */
WINDOW_ERRORS Window::create(LPCWSTR title, unsigned int width, unsigned int height, bool fullscreen, int bitsPerPixel)
{
	// Create a variable for storing the window's extended styling.
	unsigned long windowExtendedStyle;
	// Create a variable for storign the window's styling.
	unsigned long windowStyle;

	// Set the window's title.
	this->title = title;
	
	// Create a rectangle variable for the size of the window.
	RECT windowRectangle;
	// Set the left side of the window.
	windowRectangle.left = 0;
	// Set the right side of the window.
	windowRectangle.right = width;
	// Set the top of the window.
	windowRectangle.top = 0;
	// Set the bottom of the window.
	windowRectangle.bottom = height;

	// Set the boolean indicating whether the window is full screen or not to
	// false by default.  This will be changed later on if it is successfully
	// made full screen.
	isFullscreen = false;

	// Get a handle to the instance of the application.
	appInstance = GetModuleHandle(NULL);
	// Set the style to redraw vertically and horizantally, as well as to own it's on device context.
	windowsClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// Set the handle to the window processing method (this is set on the developer's end).
	windowsClass.lpfnWndProc = (WNDPROC)this->windowProcess;
	// The amount of extra bytes to allocate for the window-class structure.
	windowsClass.cbClsExtra = 0;
	// The amount of extra bytes to allocate for the window's instance.
	windowsClass.cbWndExtra = 0;
	// Set the handle to the application's instance.
	windowsClass.hInstance = appInstance;
	// Set the handle to the window-class icon.
	windowsClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	// Set the handle to the window-class cursor.
	windowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Set the handle to the class background brush to be NULL.
	windowsClass.hbrBackground = NULL;
	// Set the resource name for the window-class menu to NULL.
	windowsClass.lpszMenuName = NULL;
	// Set the window-class name to be the same as the window's title.
	windowsClass.lpszClassName = this->title;

	// Check to see if the window can be properly registered.
	if(!RegisterClass(&windowsClass))
	{
#ifdef _DEBUG
		// If debugging, display a simple message indicating where to look in the code (IE: Here).
		MessageBox(NULL, (LPCWSTR)L"Error registering class!", (LPCWSTR)L"CREATE WINDOW ERROR", MB_OK | MB_ICONERROR);
#else
		// If not debugging, then display a clean error to the user so they can report the issue.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WC" REGISTER_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Finally, return that there was an error registering the window.
		return REGISTER_ERROR;
	}

	// Check if the window should be full screen.
	if(fullscreen)
	{
		// If so, create a devmode variable for the screen's settings.
		DEVMODE deviceModeScreenSettings;
		// Use memset to initialize the memory for the window to 0.
		memset(&deviceModeScreenSettings, 0, sizeof(deviceModeScreenSettings));
		// Set the size of the settings variable.
		deviceModeScreenSettings.dmSize = sizeof(deviceModeScreenSettings);

		// Check if the display settings could not be gotten properly.
		if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceModeScreenSettings))
		{
			// If so, set the bits per pixel manually.
			deviceModeScreenSettings.dmBitsPerPel = bitsPerPixel;
			// Set the width of the device surface.
			deviceModeScreenSettings.dmPelsWidth = width;
			// Set the height of the device surface.
			deviceModeScreenSettings.dmPelsHeight = height;
			// Set the fields that have been initialized.
			deviceModeScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		}

		// Next try to change the display settings to the screen settings retrived and see if it
		// was unsuccessful.
		if(ChangeDisplaySettings(&deviceModeScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If it was unsuccessful, try to change the display settings based on a NULL value and see
			// if that is unsuccessful.
			if(ChangeDisplaySettings(NULL, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// If so, prompt the user that fullscreen mode is not available and ask if windowed mode is ok.
				if(MessageBox(NULL, (LPCWSTR)L"The requested fullscreen mode is not supported by\nyour video card. Use windowed mode instead?", (LPCWSTR)L"Warning", MB_YESNO | MB_ICONWARNING) == IDYES)
				{
					// If they answer yes, set the fullscreen mode boolean to false.
					this->isFullscreen = false;
				}
				else
				{
#ifdef _DEBUG
					// Otherwise, if debugging the application, indicate where in the code the error has occured.
					MessageBox(NULL, (LPCWSTR)L"Error going fullscreen!", (LPCWSTR)L"CREATE WINDOW ERROR", MB_OK | MB_ICONERROR);
#else
					// Otherwise, display a nice clean error to the end user.
					MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WC" + FULLSCREEN_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
					return FULLSCREEN_ERROR;
				}
			}
		}
		// Otherwise, if everything was successful, set the fullscreen boolean to true.
		this->isFullscreen = true;
	}
	
	// Check if the window was successfully made full screen.
	if(this->isFullscreen)
	{
		// If so, set the extended window style to be an application window.
		windowExtendedStyle = WS_EX_APPWINDOW;
		// Then set the normal window style to be an overlapped window.
		windowStyle = WS_OVERLAPPEDWINDOW;
		// Finally, hide the cursor.
		ShowCursor(false);
	}
	else
	{
		// If the window is not fullscreen, set the extended style to still be an application
		// window, and also set the window to have edges.
		windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		// Finally, set the normal window style to be an overlapped window still.
		windowStyle = WS_OVERLAPPEDWINDOW;
	}

	// Next, the window needs to have it's rectangle adjusted.
	AdjustWindowRectEx(&windowRectangle, windowStyle, false, windowExtendedStyle);

	// Finally, the window is actually created and assigned to it's handle.
	if(!(this->windowHandle = CreateWindowEx(windowExtendedStyle, this->title, this->title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windowStyle, 0, 0, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, NULL, NULL, appInstance, NULL)))
	{
		// If this fails, the window is destroyed.
		this->destroy();
#ifdef _DEBUG
		// If debugging, an error indicating where to further debug is shown.
		MessageBox(NULL, (LPCWSTR)L"Error creating window!", (LPCWSTR)L"CREATE WINDOW ERROR", MB_OK | MB_ICONERROR);
#else
		// If not debugging, a nice and clean error is displayed to the user.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WC" + CREATION_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Return the error indicating that the window could not be created.
		return CREATION_ERROR;
	}

	// Check if the window is full screen.
	if(isFullscreen)
	{
		// If it is, set the window's styling to get rid of the frame.
		SetWindowLong(windowHandle, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
	}

	// Show the window.
	ShowWindow(windowHandle, SW_SHOW);

	// Return that everythign went okay.
	return OK;
}

/**
 * Sets whether a window is visible or not.
 *
 * @param visible If true, the window will be shown, otherwise it will be hidden.
 * @return This will return 1 if it was previously visible and 0 if it was previously hidden.
 */
int Window::setVisible(bool visible)
{
	// Show the window and return its results ANDed with 1, creating a result of 0 or 1.
	return (ShowWindow(windowHandle, (visible) ? SW_SHOW : SW_HIDE) & 1);
}

/**
 * This will set the window as the current focus.
 * @return This will return 1 if the window was brougth to the foreground and 0 otherwise.
 */
int Window::Focus()
{
	// Set the window as the topmost window and record its results.
	int returnValue = SetForegroundWindow(windowHandle);
	// Set the window to be the current focus.
	SetFocus(windowHandle);
	// Return the success value.
	return returnValue;
}

/**
 * This will update the window's buffer.
 */
void Window::updateWindow()
{
	// Swap the buffers to update the display for the device context.
	SwapBuffers(deviceContext);
}

/**
 * This will set the window to use an OpenGL drawing context.
 * @param bitsPerPixel The bits per pixel to use for the OpenGL context.
 * @return Returns an error if something goes wrong when setting the drawing state.
 */
int Window::setWindowDrawingStateGL(int bitsPerPixel)
{
	// Create a variable for storing the pixel format of the window.
	int pixelFormat;
	// Setup the pixel format descriptor.
	static PIXELFORMATDESCRIPTOR pixelFormatDesc = 
	{
		// Set the size of the descriptor.
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		// Set the properties to draw to a window, support OpenGL, and be double buffered.
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		// Set the window to have the normal RGB spectrum, as well as an alpha value.
		PFD_TYPE_RGBA,
		// Set the bits per pixel to the desired value.
		bitsPerPixel,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		16,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	// Attempt to get a device context from the window handle.
	if(!(deviceContext = GetDC(windowHandle)))
	{
		// If this failed, destroy the OpenGL system.
		this->destroyGLSystem();
		// Also destroy the window.
		this->destroy();
#ifdef _DEBUG
		// If debugging, display an error indicating that the device context couldn't be gotten.
		MessageBox(NULL, (LPCWSTR)L"Error getting device context!", (LPCWSTR)L"Create GL Context ERROR", MB_OK | MB_ICONERROR);
#else
		// Display a nice error to the user if not debugging.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WCGL" + WCGL_GET_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Return an error indicating that the device context couldn't be gotten.
		return WCGL_GET_CONTEXT_ERROR;
	}
	// Attempt to get the pixel format.
	if(!(pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc)))
	{
		// If this failed, destroy the OpenGL system.
		this->destroyGLSystem();
		// Also destroy the window.
		this->destroy();
#ifdef _DEBUG
		// If debugging, display that there was an error finding a usable pixel format.
		MessageBox(NULL, (LPCWSTR)L"Error finding a usable pixel format!", (LPCWSTR)L"Create GL Context ERROR", MB_OK | MB_ICONERROR);
#else
		// Display a nice error to the user if not debugging.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WCGL" + WCGL_GET_PF_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Return an error indicating that the pixel format couldn't be gotten.
		return WCGL_GET_PF_ERROR;
	}
	// Try to set the pixel format for the device context.
	if(!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
	{
		// If this failed, destroy the OpenGL system.
		this->destroyGLSystem();
		// Also destroy the window.
		this->destroy();
#ifdef _DEBUG
		// If debugging, display an error indicating that the pixel format couldn't be set.
		MessageBox(NULL, (LPCWSTR)L"Error setting pixel format!", (LPCWSTR)L"Create GL Context ERROR", MB_OK | MB_ICONERROR);
#else
		// Display a nice error to the user if not debugging.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WCGL" + WCGL_SET_PF_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Return an error indicating that the pixel format couldn't be set.
		return WCGL_SET_PF_ERROR;
	}
	// Attempt to create an OpenGL context and store it.
	if(!(renderingContext = wglCreateContext(deviceContext)))
	{
		// If this failed, destroy the OpenGL system.
		this->destroyGLSystem();
		// Also destroy the window.
		this->destroy();
#ifdef _DEBUG
		// Display an error indicating what happened if debuging.
		MessageBox(NULL, (LPCWSTR)L"Error creating rendering context!", (LPCWSTR)L"Create GL Context ERROR", MB_OK | MB_ICONERROR);
#else
		// Display a nice error to the user otherwise.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WCGL" + WCGL_CREATE_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		// Return an error indicating that the OpenGL context couldn't be created.
		return WCGL_CREATE_CONTEXT_ERROR;
	}
	// Try to set the current OpenGL context.
	if(!wglMakeCurrent(deviceContext, renderingContext))
	{
		// If this failed, destroy the OpenGL system.
		this->destroyGLSystem();
		// Also destroy the window.
		this->destroy();
#ifdef _DEBUG
		// If debugging, display an error indicating that the current context couldn't be set.
		MessageBox(NULL, (LPCWSTR)L"Error making rendering context the current one!", (LPCWSTR)L"Create GL Context ERROR", MB_OK | MB_ICONERROR);
#else
		// If not debugging, display a nice error to the user.
		MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WCGL" + WCGL_SET_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		/**
		 * Return an error indicating that the OpenGL context couldn't be set.
		 */
		return WCGL_SET_CONTEXT_ERROR;
	}
	return OK;
}

/**
 * Make this window the current OpenGL context for rendering to.
 * @return Return OK if there is no error, otherwise return the error.
 */
int Window::MakeCurrentGLContext()
{
	// Check that there is an OpenGL rendering context.
	if(renderingContext)
	{
		// Try to set the current OpenGL context.
		if(!wglMakeCurrent(deviceContext, renderingContext))
		{
			// If this failed, destroy the OpenGL system.
			this->destroyGLSystem();
			// Also destroy the window.
			this->destroy();
#ifdef _DEBUG
			// If debugging, display an error indicating that the current context couldn't be set.
			MessageBox(NULL, (LPCWSTR)L"Error making rendering context the current one!", (LPCWSTR)L"Set GL Context ERROR", MB_OK | MB_ICONERROR);
#else
			// If not debugging, display a nice error to the user.
			MessageBox(NULL, (LPCWSTR)L"Error running application!\nError code: WMGL" + WCGL_SET_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			/**
			 * Return an error indicating that the OpenGL context couldn't be set.
			 */
			return WCGL_SET_CONTEXT_ERROR;
		}
	}
	// Return OK otherwise.
	return OK;
}

/**
 * This will destroy the window and its associated properties.  Be careful calling this if
 * the window is already destroyed by the user!
 */
void Window::destroy()
{
	// Check whether the window is fullscreen or not.
	if(this->isFullscreen)
	{
		// Change the display settings for the window to not be full screen.
		if(ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
		{
#ifdef _DEBUG
			// If debugging, display an error indicating where the program failed.
			MessageBox(NULL, (LPCWSTR)L"Changing display from fullscreen failed!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
#else
			// If not debugging, display a nice error code to reference for the user.
			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + FULLSCREEN_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			// Make sure that the cursor is shown finally too.
			ShowCursor(true);
		}

		// Set the fullscreen boolean to false.
		this->isFullscreen = false;

		// Check if there is a device context and attempt to release it.
		if(deviceContext && !deviceContext->unused && !ReleaseDC(windowHandle, deviceContext))
		{
#ifdef _DEBUG
			// If debugging, display an error indicating that the device context couldn't be released.
			MessageBox(NULL, (LPCWSTR)L"Error releasing device context!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
#else
			// Display a nice error for the user if not debugging.
			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + DC_RELEASE_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			// Set the device context to NULL.
			deviceContext = NULL;
		}
		// Check if there is a window handle and attempt to destroy the window associated with it.
		if(windowHandle && !windowHandle->unused && !DestroyWindow(windowHandle))
		{
#ifdef _DEBUG
			// If debugging, display an error indicating that the window handle couldn't be destroyed.
			MessageBox(NULL, (LPCWSTR)L"Error destroying window handle!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
#else
			// Dispaly a nice error to the user if not debugging.
			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + WH_DESTROY_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			// Set the window handle to NULL.
			windowHandle = NULL;
		}
		// Attempt to unregister the title associated with the window.
		if(!UnregisterClass(this->title, appInstance))
		{
#ifdef _DEBUG
			// If debugging, display  an error indicating that the window class could not be unregistered.
			MessageBox(NULL, (LPCWSTR)L"Error unregistering class!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
#else
			// Display a nice error to the user if not debugging.
			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + WC_UNREGISTER_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			// Set the applications instance handle to NULL.
			appInstance = NULL;
		}
	}
}

/**
 * This will destroy the OpenGL context associated with this window.
 */
void Window::destroyGLSystem()
{
	// Check if there is a rendering context associated with the window.
	if(renderingContext)
	{
		// If so, get rid of the current OpenGL context.
		if(!wglMakeCurrent(NULL, NULL))
		{
#ifdef _DEBUG
			// If debugging, display an error indicating that the OpenGL system could not be destroyed.
			MessageBox(NULL, (LPCWSTR)L"Changing window OpenGL context Failed!", (LPCWSTR)L"Destroy OpenGL System ERROR", MB_OK | MB_ICONERROR);
#else
			// If not debugging, display a nice error to the user.
			MessageBox(NULL, (LPCWSTR)L"Error closing window!\nError code: WDGL" + WDGL_CHANGE_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		}
		// Attempt to delete the OpenGL context.
		if(!wglDeleteContext(renderingContext))
		{
#ifdef _DEBUG
			// If debugging, display an error indicating that there was a problem deleting the OpenGL context.
			MessageBox(NULL, (LPCWSTR)L"Error destroying the OpenGL context!", (LPCWSTR)L"Destroy OpenGL System ERROR", MB_OK | MB_ICONERROR);
#else
			// Display a nice error to the user if not debugging.
			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WDGL" + WDGL_DELETE_CONTEXT_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
		}
		// Set the rendering context to NULL finally.
		renderingContext = NULL;
	}
}
#elif __APPLE__
#elif __linux__
#endif
