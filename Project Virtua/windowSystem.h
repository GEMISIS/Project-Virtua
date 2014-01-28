#ifdef _WIN32
// Include windows header file for Windows (duh)
#include <Windows.h>

/**
 * The default bits per pixel to use for an OpenGL context.
 */
#define DEFAULT_BITS_PER_PIXEL 16

typedef LRESULT CALLBACK windowProcessCallback(HWND winHandle, UINT message, WPARAM windowParam, LPARAM messageParam);

/**
 * A list of potential errors when creating and destroy a window and it's parts.
 */
enum WINDOW_ERRORS
{
	/**
	 * This indicates that everything was ok with creating a window.
	 */
	OK = 1,
	/**
	 * This indicates that an unhandled error occured!  If this happens, please file a bug report!
	 */
	RANDOM_FAILURE = 0,
	/**
	 * This indicates that the window-class could not be registered properly.
	 */
	REGISTER_ERROR = -1,
	/**
	 * This indicates that there was an error creating a fullscreen window.
	 */
	FULLSCREEN_ERROR = -2,
	/**
	 * This indicates that there was an error actually creating the window.
	 */
	CREATION_ERROR = -3,
	/**
	 * This indicates there was an error releasing the device context.
	 */
	DC_RELEASE_ERROR = -4,
	/**
	 * This indicates that there was a problem destroying a window associated with a window handle.
	 */
	WH_DESTROY_ERROR = -5,
	/**
	 * This indicates that there was an error unregistering the window's class.
	 */
	WC_UNREGISTER_ERROR = -6,
	/**
	 * This indicates that there was an error changing the OpenGL context for destroying the context.
	 */
	WDGL_CHANGE_CONTEXT_ERROR = -7,
	/**
	 * This indicates there was an error actually deleting the OpenGL context.
	 */
	WDGL_DELETE_CONTEXT_ERROR = -8,
	/**
	 * This indicates that there was an error getting the device context.
	 */
	WCGL_GET_CONTEXT_ERROR = -9,
	/**
	 * This indicates that there was an error getting the pixel format.
	 */
	WCGL_GET_PF_ERROR = -10,
	/**
	 * This indicates that there was an error setting the pixel format.
	 */
	WCGL_SET_PF_ERROR = -11,
	/**
	 * This indicates that there was an error creating the OpenGL context.
	 */
	WCGL_CREATE_CONTEXT_ERROR = -12,
	/**
	 * This indicates that there was an error setting the OpenGL context.
	 */
	WCGL_SET_CONTEXT_ERROR = -13
};

/**
 * A class used for creating and managing windows.
 *
 * A class for creating windows.  These windows can then be associated with OpenGL rendering contexts.
 * TODO Still: Add support for directX.
 */
class Window
{
// The variables that are visible to everything using an instance of this
// class.
public:
	/**
	 * This method creates a window using the given information and returns its status.
	 *
	 * This method creates a window using the given information.  If an error occurs, it is returned.
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
	WINDOW_ERRORS create(LPCWSTR title, unsigned int width, unsigned int height, bool fullscreen, int bitsPerPixel);

	/**
	 * This method creates a window using the given information and then returns its status.
	 *
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
	WINDOW_ERRORS create(LPCWSTR title, unsigned int width, unsigned int height, bool fullscreen, windowProcessCallback callback);

	/**
	 * Set the window's process callback method.
	 *
	 * Set the window's process callback method.  This is used when doing things such as resizing
	 * the window, closing the window, etc.
	 * @param callback The function to use.
	 */
	void setWindowProcessCallback(windowProcessCallback callback);

	/**
	 * Sets whether a window is visible or not.
	 *
	 * Sets whether a window is visible or not.
	 * @param visible If true, the window will be shown, otherwise it will be hidden.
	 * @return This will return 1 if it was previously visible and 0 if it was previously hidden.
	 */
	int setVisible(bool visible);
	/**
	 * This will set the window as the current focus.
	 *
	 * This will set the window as the current focus.
	 * @return This will return 1 if the window was brougth to the foreground and 0 otherwise.
	 */
	int Focus();
	/**
	 * This will update the window's buffer.
	 */
	void Update();
	/**
	 * This will set the window to use an OpenGL drawing context.
	 *
	 * This will set the window to use an OpenGL drawing context.
	 * @return Returns an error if something goes wrong when setting the drawing state.
	 */
	int setWindowDrawingStateGL();
	/**
	 * This will make this window the current OpenGL context for rendering to.
	 *
	 * Make this window the current OpenGL context for rendering to.
	 * @return Return OK if there is no error, otherwise return the error.
	 */
	int MakeCurrentGLContext();
	/**
	 * Get the window's handle object.
	 *
	 * This method returns the window's handle object if it exists.  It will return NULL otherwise.
	 * @return Returns the window's handle if it exists.  Will return NULL otherwise.
	 */
	const HWND GetWindowHandle();
	/**
	 * Gets the applications instance.
	 *
	 * Gets the applications instance if it exists.  It will return NULL otherwise.
	 * @return Returns the applications instance if it exists.  Will return NULL if it does not.
	 */
	const HINSTANCE GetAppInstance();
	/**
	 * Gets whether the window is active or not.
	 *
	 * Gets whether the window is active or not.
	 * @return Returns true if the window is active, false otherwise.
	 */
	const bool IsActive();
	/**
	 * Gets whether the window is fullscreen or not.
	 *
	 * Gets whether the window is fullscreen or not.
	 * @return Returns true if the window is fullscreen, false otherwise.
	 */
	const bool IsFullscreen();
	/**
	 * Gets bits per pixel for the window.
	 *
	 * Gets bits per pixel for the window.
	 * @return Returns an integer indicating the bits per pixel for the window.
	 */
	const int BitsPerPixel();
	/**
	 * The title of the window.
	 *
	 * Gets the title of the window.
	 * @return The title of the window as a 16-bit unicode string.
	 */
	const LPCWSTR Title();
	/**
	 * Destroy the window and its associated properties.
	 *
	 * This will destroy the window and its associated properties.  Be careful calling this if
	 * the window is already destroyed by the user!
	 */
	void destroy();
	/**
	 * This will destroy the OpenGL context associated with this window.
	 */
	void destroyGLSystem();
private:
	/**
	 * The rendering context for where to draw stuff to.
	 */
	HGLRC renderingContext;

	/**
	 * A device context for connecting the window to the graphics
	 * interface device.
	 */
	HDC deviceContext;

	/**
	 * A window handle for managing the actual window.
	 */
	HWND windowHandle;

	/**
	 * A variable for an instance of the application.
	 */
	HINSTANCE appInstance;

	/**
	 * A boolean variable indicating whether the window is active.
	 */
	bool active;

	/**
	 * A boolean variable indicating whether the window is fullscreen or not.
	 */
	bool isFullscreen;

	/**
	 * The bits per pixel for this window.
	 */
	int bitsPerPixel;

	/**
	 * An instance of the window.
	 */
	WNDCLASS windowsClass;

	/**
	 * The window's process method for processing its events.
	 */
	LRESULT windowProcess;

	/**
	 * The window's title.
	 */
	LPCWSTR title;
};

#elif __APPLE__
#elif __linux__
#endif
