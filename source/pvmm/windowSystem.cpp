#include "pvmm/windowSystem.h"

namespace PV
{
#ifdef _WIN32
	Window::Window()
	{
		this->windowsClass.hInstance = NULL;
	}
	/**
	 * This method creates a window using the given information.  If an error occurs, it is returned.
	 * a window using the given information.  If an error occurs, it is returned.
	 * @param title The title to be displayed in the window.
	 * @param width An unsigned integer for the width of the window.
	 * @param height An unsigned integer for the height of the window.
	 * @param fullscreen If true, the window will be made full screen, with the width and height determining the
	 * screen resolution.  If false, it will simply create the window with the selected width and height as its
	 * size.
	 * @param bitsPerPixel The bits per pixel to use for the window.
	 * @return On success, this method will return 1.
	 * On fail, it will return an error code.
	 */
	WINDOW_ERRORS Window::create(LPCWSTR title, unsigned int width, unsigned int height, bool fullscreen, int bitsPerPixel)
	{
		this->width = width;
		this->height = height;

		// Set the window as not active.
		this->active = false;

		// Create a variable for storing the window's extended styling.
		unsigned long windowExtendedStyle;
		// Create a variable for storing the window's styling.
		unsigned long windowStyle;

		// Set the window's title.
		this->title = title;

		// Set the bits per pixel for this window.
		this->bitsPerPixel = bitsPerPixel;

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
		this->isFullscreen = false;

		// Get a handle to the instance of the application.
		appInstance = GetModuleHandle(NULL);
		// Set the style to redraw vertically and horizantally, as well as to own it's own device context.
		windowsClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
		if (!RegisterClass(&windowsClass))
		{
#ifdef _DEBUG
			// If debugging, display a simple message indicating where to look in the code (IE: Here).
			MessageBox(NULL, (LPCWSTR)L"Error registering class!", (LPCWSTR)L"CREATE WINDOW ERROR", MB_OK | MB_ICONERROR);
#else
			// If not debugging, then display a clean error to the user so they can report the issue.
			MessageBox(NULL, (LPCWSTR)L"Error starting application!\nError code: WC" + REGISTER_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
#endif
			// Finally, return that there was an error registering the window.
			return REGISTER_ERROR;
		}

		// If the window is not fullscreen, set the extended style to still be an application
		// window, and also set the window to have edges.
		windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		// Finally, set the normal window style to be an overlapped window still.
		windowStyle = WS_OVERLAPPEDWINDOW;

		// Next, the window needs to have it's rectangle adjusted.
		AdjustWindowRectEx(&windowRectangle, windowStyle, false, windowExtendedStyle);

		// Finally, the window is actually created and assigned to it's handle.
		if (!(this->windowHandle = CreateWindowEx(windowExtendedStyle, this->title, this->title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windowStyle, 0, 0, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, NULL, NULL, appInstance, NULL)))
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

		// Show the window.
		ShowWindow(windowHandle, SW_SHOW);

		this->SetFullscreen(fullscreen);

		// Return that everythign went okay.
		return OK;
	}

	/**
	 * This method creates a window using the given information.  If an error occurs, it is returned.
	 * @param title The title to be displayed in the window.
	 * @param width An unsigned integer for the width of the window.
	 * @param height An unsigned integer for the height of the window.
	 * @param fullscreen If true, the window will be made full screen, with the width and height determining the
	 * screen resolution.  If false, it will simply create the window with the selected width and height as its
	 * size.
	 * @param callback The callback to use when updating the window.
	 * @return On success, this method will return 1.
	 * On fail, it will return an error code.
	 */
	WINDOW_ERRORS Window::create(LPCWSTR title, unsigned int width, unsigned int height, bool fullscreen, windowProcessCallback callback)
	{
		// Check that a callback was given.
		if (callback != NULL)
		{
			this->setWindowProcessCallback(callback);
		}
		else
		{
			//		this->setWindowProcessCallback(&Window::process);
		}
		// Create the window returning any errors that come up.
		return this->create(title, width, height, fullscreen, DEFAULT_BITS_PER_PIXEL);
	}

	/**
	* This method creates a window using the given information and then returns its status.
	* @param title The title to be displayed in the window.
	* @param callback The callback to use when updating the window.
	* @param fullscreen If true, the window will be made full screen, with the width and height determining the
	* screen resolution.  If false, it will simply create the window with the selected width and height as its
	* size.
	* @return On success, this method will return 1.
	* On fail, it will return an error code.
	*/
	WINDOW_ERRORS Window::create(LPCWSTR title, bool fullscreen, windowProcessCallback callback)
	{
		// Check that a callback was given.
		if (callback != NULL)
		{
			this->setWindowProcessCallback(callback);
		}
		else
		{
			//		this->setWindowProcessCallback(&Window::process);
		}
		// Create the window returning any errors that come up.
		return this->create(title, 1280, 800, fullscreen, DEFAULT_BITS_PER_PIXEL);
	}

	/**
	* This method creates a window using the given information and then returns its status.
	* @param title The title to be displayed in the window.
	* @param callback The callback to use when updating the window.
	* @return On success, this method will return 1.
	* On fail, it will return an error code.
	*/
	WINDOW_ERRORS Window::create(LPCWSTR title, windowProcessCallback callback)
	{
		// Check that a callback was given.
		if (callback != NULL)
		{
			this->setWindowProcessCallback(callback);
		}
		else
		{
			//		this->setWindowProcessCallback(&Window::process);
		}
		// Create the window returning any errors that come up.
		return this->create(title, 1280, 800, false, DEFAULT_BITS_PER_PIXEL);
	}

	/**
	 * Set the window's process callback method.  This is used when doing things such as resizing
	 * the window, closing the window, etc.
	 * @param callback The function to use.
	 */
	void Window::setWindowProcessCallback(windowProcessCallback callback)
	{
		// Set the callback casted.
		windowsClass.lpfnWndProc = (WNDPROC)callback;
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
		// Make sure the window is visible first.
		this->setVisible(true);
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
	void Window::Update()
	{
		// Swap the buffers to update the display for the device context.
		SwapBuffers(deviceContext);
	}

	/**
	 * This will set the window to use an OpenGL drawing context.
	 * @return Returns an error if something goes wrong when setting the drawing state.
	 */
	int Window::setWindowDrawingStateGL()
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
			this->bitsPerPixel,
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
		if (!(deviceContext = GetDC(windowHandle)))
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
		if (!(pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc)))
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
		if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
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
		if (!(renderingContext = wglCreateContext(deviceContext)))
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
		if (!wglMakeCurrent(deviceContext, renderingContext))
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
		if (renderingContext)
		{
			// Try to set the current OpenGL context.
			if (!wglMakeCurrent(deviceContext, renderingContext))
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
	 * Get the window's handle object.
	 *
	 * This method returns the window's handle object if it exists.  It will return NULL otherwise.
	 */
	const HWND Window::GetWindowHandle()
	{
		return this->windowHandle;
	}

	/**
	 * Gets the applications instance.
	 *
	 * Gets the applications instance if it exists.  it will return NULL otherwise.
	 */
	const HINSTANCE Window::GetAppInstance()
	{
		return this->appInstance;
	}

	/**
	 * Gets whether the window is active or not.
	 *
	 * Gets whether the window is active or not.
	 * @return Returns true if the window is active, false otherwise.
	 */
	const bool Window::IsActive()
	{
		return this->active;
	}

	/**
	 * Gets whether the window is fullscreen or not.
	 *
	 * Gets whether the window is fullscreen or not.
	 * @return Returns true if the window is fullscreen, false otherwise.
	 */
	const bool Window::IsFullscreen()
	{
		return this->isFullscreen;
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		this->setVisible(false);

		this->isFullscreen = fullscreen;

		if (this->isFullscreen)
		{
			DEVMODE deviceModeScreenSettings;
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceModeScreenSettings);
			deviceModeScreenSettings.dmPelsWidth = (DWORD)this->width;
			deviceModeScreenSettings.dmPelsHeight = (DWORD)this->height;
			deviceModeScreenSettings.dmBitsPerPel = (DWORD)this->bitsPerPixel;

			long success = ChangeDisplaySettings(&deviceModeScreenSettings, CDS_FULLSCREEN);
			if (success == DISP_CHANGE_SUCCESSFUL)
			{
				AdjustWindowRectEx(&windowRectangle, WS_POPUP, false, WS_EX_APPWINDOW);
				SetWindowLong(this->windowHandle, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
				ShowCursor(false);
			}
		}
		else
		{
			windowRectangle.right = width;
			windowRectangle.bottom = height;
			AdjustWindowRectEx(&windowRectangle, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
			SetWindowPos(this->windowHandle, HWND_TOPMOST, 0, 0, this->width, this->height, SWP_DEFERERASE);
			SetWindowLong(this->windowHandle, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW);
			ShowCursor(true);
		}

		this->setVisible(true);
	}

	/**
	 * Gets bits per pixel for the window.
	 *
	 * Gets bits per pixel for the window.
	 * @return Returns an integer indicating the bits per pixel for the window.
	 */
	const int Window::BitsPerPixel()
	{
		return this->bitsPerPixel;
	}

	/**
	 * The title of the window.
	 *
	 * Gets the title of the window.
	 * @return The title of the window as a 16-bit unicode string.
	 */
	const LPCWSTR Window::Title()
	{
		return this->title;
	}

	/**
	 * This will destroy the window and its associated properties.  Be careful calling this if
	 * the window is already destroyed by the user!
	 */
	void Window::destroy()
	{
		// Check whether the window is fullscreen or not.
		if (this->isFullscreen)
		{
			// Change the display settings for the window to not be full screen.
			if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
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

			//		// Check if there is a device context and attempt to release it.
			//		if(deviceContext && !deviceContext->unused && !ReleaseDC(windowHandle, deviceContext))
			//		{
			//#ifdef _DEBUG
			//			// If debugging, display an error indicating that the device context couldn't be released.
			//			MessageBox(NULL, (LPCWSTR)L"Error releasing device context!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
			//#else
			//			// Display a nice error for the user if not debugging.
			//			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + DC_RELEASE_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
			//#endif
			//			// Set the device context to NULL.
			//			deviceContext = NULL;
			//		}
			//		// Check if there is a window handle and attempt to destroy the window associated with it.
			//		if(windowHandle && !windowHandle->unused && !DestroyWindow(windowHandle))
			//		{
			//#ifdef _DEBUG
			//			// If debugging, display an error indicating that the window handle couldn't be destroyed.
			//			MessageBox(NULL, (LPCWSTR)L"Error destroying window handle!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
			//#else
			//			// Dispaly a nice error to the user if not debugging.
			//			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + WH_DESTROY_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
			//#endif
			//			// Set the window handle to NULL.
			//			windowHandle = NULL;
			//		}
			//		// Attempt to unregister the title associated with the window.
			//		if(!UnregisterClass(this->title, appInstance))
			//		{
			//#ifdef _DEBUG
			//			// If debugging, display  an error indicating that the window class could not be unregistered.
			//			MessageBox(NULL, (LPCWSTR)L"Error unregistering class!", (LPCWSTR)L"Window Destroy ERROR", MB_OK | MB_ICONERROR);
			//#else
			//			// Display a nice error to the user if not debugging.
			//			MessageBox(NULL, (LPCWSTR)L"Error closing application!\nError code: WD" + WC_UNREGISTER_ERROR, (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
			//#endif
			//			// Set the applications instance handle to NULL.
			//			appInstance = NULL;
			//		}
		}
	}

	/**
	 * This will destroy the OpenGL context associated with this window.
	 */
	void Window::destroyGLSystem()
	{
		// Check if there is a rendering context associated with the window.
		if (renderingContext)
		{
			// If so, get rid of the current OpenGL context.
			if (!wglMakeCurrent(NULL, NULL))
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
			if (!wglDeleteContext(renderingContext))
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
};
