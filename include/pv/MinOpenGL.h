#ifndef _MIN_OPEN_GL_H_
#define _MIN_OPEN_GL_H_

#include <GL/GL.h>
#include <GL/GLU.h>

/**
 * The define for getting the major version (X.0) in OpenGL.
 */
#define __GL_MAJOR_VERSION 0x821B
/**
 * The define for getting the minor version (0.X) in OpenGL.
 */
#define __GL_MINOR_VERSION 0x821C

typedef GLuint (WINAPI* _glCreateShaderFunction) (GLenum shaderType);

extern _glCreateShaderFunction glCreateShader;

#endif