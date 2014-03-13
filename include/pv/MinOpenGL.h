#ifndef _MIN_OPEN_GL_H_
#define _MIN_OPEN_GL_H_

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#define glGetProcAddress(funcname) wglGetProcAddress(funcname)

#elif __APPLE__
#include <GL/glx.h>
#define __stdcall
inline void* glGetProcAddress(const char* name)
{
	NSSymbol symbol = NULL;
	char* symbolName;
	symbolName = malloc(strlen(name) + 2);
	strcpy(symbolName + 1, name);
	symbolName[0] = '_';

	if (NSIsSymbolNameDefined(symbolName))
	{
		symbol = NSLookupAndBindSymbol(symbolName);
	}
	free(symbolName);
	return symbol ? NSAddressOfSymbol(symbol) : NULL;
}

#elif __linux
#define __stdcall
#include <GL/glx.h>
#define glGetProcAddress(funcname) glXGetProcAddress((const GLubyte*)funcname)

#elif __unix
#define __stdcall

#elif __posix
#define __stdcall

#else
#define __stdcall
#include <GL/glx.h>
#define glGetProcAddress(funcname) glXGetProcAddress((const GLubyte*)funcname)
#endif

#include <GL/GL.h>
#include <GL/GLU.h>

/**
 * The define for getting the major version (X.0) in OpenGL.
 */
#define PV_GL_MAJOR_VERSION 0x821B
/**
 * The define for getting the minor version (0.X) in OpenGL.
 */
#define PV_GL_MINOR_VERSION 0x821C

/**
 * Specifies to the glCreateShader function to create a fragment shader.
 */
#define PV_GL_FRAGMENT_SHADER 0x8B30
/**
 * Specifies to the glCreateShader function to create a vertex shader.
 */
#define PV_GL_VERTEX_SHADER 0x8B31

/**
 * A function pointer for the glCreateShader function.
 */
typedef GLuint(__stdcall* pv_glCreateShaderFunction) (GLenum shaderType);
/**
 * A function pointer for the glShaderSource function.
 */
typedef void(__stdcall* pv_glShaderSourceFunction) (GLuint shader, GLsizei count, const char* const* string, const GLint* length);
/**
 * A function pointer for the glCompileShader function.
 */
typedef void(__stdcall* pv_glCompileShaderFunction) (GLuint shader);
/**
 * A function pointer for the glDeleteShader function.
 */
typedef void (__stdcall* pv_glDeleteShaderFunction) (GLuint shader);
/**
 * A function pointer for the glCreateProgram function.
 */
typedef GLuint(__stdcall* pv_glCreateProgramFunction) (void);
/**
 * A function pointer for the glAttachShader function.
 */
typedef void(__stdcall* pv_glAttachShaderFunction) (GLuint program, GLuint shader);
/**
 * A function pointer for the glLinkProgram function.
 */
typedef void(__stdcall* pv_glLinkProgramFunction) (GLuint program);

/**
 * The OpenGL method "glCreateShader", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glCreateShader.xml):
 *
 * glCreateShader creates an empty shader object and returns a non-zero value by which it can be referenced. A shader
 * object is used to maintain the source code strings that define a shader. shaderType indicates the type of shader
 * to be created. Two types of shaders are supported. A shader of type GL_VERTEX_SHADER is a shader that is intended 
 * to run on the programmable vertex processor. A shader of type GL_FRAGMENT_SHADER is a shader that is intended to 
 * run on the programmable fragment processor.
 * 
 * When created, a shader object's GL_SHADER_TYPE parameter is set to either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER, 
 * depending on the value of shaderType.
 * 
 * @param shaderType Specifies the type of shader to be created. Must be either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 * @return This function returns 0 if an error occurs creating the shader object.  GL_INVALID_ENUM is generated if shaderType 
 * is not an accepted value.
 */
extern pv_glCreateShaderFunction pv_glCreateShader;

/**
 * The OpenGL method "glShaderSource", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glShaderSource.xml):
 *
 * For implementations that support a shader compiler, glShaderSource sets the source code in shader to the source code 
 * in the array of strings specified by string. Any source code previously stored in the shader object is completely 
 * replaced. The number of strings in the array is specified by count. If length is NULL, each string is assumed to be 
 * null terminated. If length is a value other than NULL, it points to an array containing a string length for each of 
 * the corresponding elements of string. Each element in the length array may contain the length of the corresponding 
 * string (the null character is not counted as part of the string length) or a value less than 0 to indicate that the 
 * string is null terminated. The source code strings are not scanned or parsed at this time; they are simply copied into 
 * the specified shader object.
 * 
 * @param shader Specifies the handle of the shader object whose source code is to be replaced.
 * @param count Specifies the number of elements in the string and length arrays.
 * @param string Specifies an array of pointers to strings containing the source code to be loaded into the shader.
 * @param length Specifies an array of string lengths.
 */
extern pv_glShaderSourceFunction pv_glShaderSource;

/**
 * The OpenGL method "glCompileShader", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glCompileShader.xml):
 *
 * For implementations that support a shader compiler, glCompileShader compiles the source 
 * code strings that have been stored in the shader object specified by shader.
 *
 * The compilation status will be stored as part of the shader object's state. This value will be set to GL_TRUE 
 * if the shader was compiled without errors and is ready for use, and GL_FALSE otherwise. It can be queried by 
 * calling glGetShaderiv with arguments shader and GL_COMPILE_STATUS.
 * 
 * Compilation of a shader can fail for a number of reasons as specified by the OpenGL ES Shading Language Specification. 
 * Whether or not the compilation was successful, information about the compilation can be obtained from the shader object's 
 * information log by calling glGetShaderInfoLog.
 *
 * @param shader Specifies the shader object to be compiled.
 */
extern pv_glCompileShaderFunction pv_glCompileShader;

/**
 * The OpenGL method "glDeleteShader", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glDeleteShader.xml):
 *
 * glDeleteShader frees the memory and invalidates the name associated with the shader object specified by shader. This 
 * command effectively undoes the effects of a call to glCreateShader.
 *
 * If a shader object to be deleted is attached to a program object, it will be flagged for deletion, but it will not be 
 * deleted until it is no longer attached to any program object, for any rendering context (i.e., it must be detached from 
 * wherever it was attached before it will be deleted). A value of 0 for shader will be silently ignored.
 *
 * To determine whether an object has been flagged for deletion, call glGetShaderiv with arguments shader and GL_DELETE_STATUS.
 *
 * @param shader Specifies the shader object to be compiled.
 */
extern pv_glDeleteShaderFunction pv_glDeleteShader;

/**
 * The OpenGL method "glCreateProgram", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glCreateProgram.xml):
 *
 * glCreateProgram creates an empty program object and returns a non-zero value by which it can be referenced. 
 * A program object is an object to which shader objects can be attached. This provides a mechanism to specify the 
 * shader objects that will be linked to create a program. It also provides a means for checking the compatibility 
 * of the shaders that will be used to create a program (for instance, checking the compatibility between a vertex 
 * shader and a fragment shader). When no longer needed as part of a program object, shader objects can be detached.
 *
 * One or more executables are created in a program object by successfully attaching shader objects to it with 
 * glAttachShader, successfully compiling the shader objects with glCompileShader, and successfully linking the program 
 * object with glLinkProgram. These executables are made part of current state when glUseProgram is called. Program objects 
 * can be deleted by calling glDeleteProgram. The memory associated with the program object will be deleted when it is no 
 * longer part of current rendering state for any context.
 *
 * @return This function returns 0 if an error occurs creating the program object.
 */
extern pv_glCreateProgramFunction pv_glCreateProgram;

/**
 * The OpenGL method "glAttachShader", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glAttachShader.xml):
 *
 * In order to create an executable, there must be a way to specify the list of things that will be 
 * linked together. Program objects provide this mechanism. Shaders that are to be linked together in a 
 * program object must first be attached to that program object. glAttachShader attaches the shader object 
 * specified by shader to the program object specified by program. This indicates that shader will be included 
 * in link operations that will be performed on program.
 *
 * All operations that can be performed on a shader object are valid whether or not the shader object is attached to a program 
 * object. It is permissible to attach a shader object to a program object before source code has been loaded into the shader 
 * object or before the shader object has been compiled. Multiple shader objects of the same type may not be attached to a 
 * single program object. However, a single shader object may be attached to more than one program object. If a shader object 
 * is deleted while it is attached to a program object, it will be flagged for deletion, and deletion will not occur until 
 * glDetachShader is called to detach it from all program objects to which it is attached.
 *
 * @param program Specifies the program object to which a shader object will be attached.
 * @param shader Specifies the shader object that is to be attached.
 */
extern pv_glAttachShaderFunction pv_glAttachShader;

/**
 * The OpenGL method "glAttachShader", to be grabbed as an OpenGL extension.
 *
 * Definition from http://www.khronos.org/ (http://www.khronos.org/opengles/sdk/docs/man/xhtml/glLinkProgram.xml):
 *
 * glLinkProgram links the program object specified by program. Shader objects of type GL_VERTEX_SHADER attached to program 
 * are used to create an executable that will run on the programmable vertex processor. Shader objects of type GL_FRAGMENT_SHADER 
 * attached to program are used to create an executable that will run on the programmable fragment processor.
 * 
 * The status of the link operation will be stored as part of the program object's state. This value will be set to GL_TRUE if 
 * the program object was linked without errors and is ready for use, and GL_FALSE otherwise. It can be queried by calling glGetProgramiv 
 * with arguments program and GL_LINK_STATUS.
 * 
 * As a result of a successful link operation, all active user-defined uniform variables belonging to program will be initialized to 0, 
 * and each of the program object's active uniform variables will be assigned a location that can be queried by calling 
 * glGetUniformLocation. Also, any active user-defined attribute variables that have not been bound to a generic vertex attribute 
 * index will be bound to one at this time.
 * 
 * Linking of a program object can fail for a number of reasons as specified in the OpenGL ES Shading Language Specification. The 
 * following lists some of the conditions that will cause a link error.
 * 
 * *A vertex shader and a fragment shader are not both present in the program object.
 * 
 * *The number of active attribute variables supported by the implementation has been exceeded.
 * 
 * The storage limit for uniform variables has been exceeded.
 *  
 * *The number of active uniform variables supported by the implementation has been exceeded.
 * 
 * *The main function is missing for the vertex shader or the fragment shader.
 * 
 * *A varying variable actually used in the fragment shader is not declared in the same way (or is not declared at all) in the vertex 
 * shader.
 * 
 * *A reference to a function or variable name is unresolved.
 * 
 * *A shared global is declared with two different types or two different initial values.
 * 
 * *One or more of the attached shader objects has not been successfully compiled (via glCompileShader) or loaded with a pre-compiled 
 * shader binary (via glShaderBinary).
 * 
 * *Binding a generic attribute matrix caused some rows of the matrix to fall outside the allowed maximum of GL_MAX_VERTEX_ATTRIBS.
 * 
 * *Not enough contiguous vertex attribute slots could be found to bind attribute matrices.
 * 
 * When a program object has been successfully linked, the program object can be made part of current state by calling 
 * glUseProgram. Whether or not the link operation was successful, the program object's information log will be overwritten. The 
 * information log can be retrieved by calling glGetProgramInfoLog.
 * 
 * glLinkProgram will also install the generated executables as part of the current rendering state if the link operation was 
 * successful and the specified program object is already currently in use as a result of a previous call to glUseProgram. If 
 * the program object currently in use is relinked unsuccessfully, its link status will be set to GL_FALSE , but the executables 
 * and associated state will remain part of the current state until a subsequent call to glUseProgram removes it from use. After 
 * it is removed from use, it cannot be made part of current state until it has been successfully relinked.
 * 
 * The program object's information log is updated and the program is generated at the time of the link operation. After the link 
 * operation, applications are free to modify attached shader objects, compile attached shader objects, detach shader objects, 
 * delete shader objects, and attach additional shader objects. None of these operations affects the information log or the program 
 * that is part of the program object.
 *
 * @param program Specifies the handle of the program object to be linked.
 */
extern pv_glLinkProgramFunction pv_glLinkProgram;

/**
 * Initializes the minimum required OpenGL functions for use with Project Virtua.  All of these methods are prefixed with pv_ in order
 * to prevent compiler issues with OpenGL extension libraries.  These methods are included so that only the necessary OpenGL methods
 * are added for smaller file sizes and such.
 */
void initMinGL();

#endif