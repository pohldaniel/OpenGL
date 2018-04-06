#if !defined(EXTENSION_H)
#define EXTENSION_H

#include <windows.h>
#include <GL/gl.h>


extern "C" {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.2
//
#define GL_BGRA                           0x80E1
#define GL_CLAMP_TO_EDGE                  0x812F
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.4
//
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_GENERATE_MIPMAP                0x8191
#define GL_TEXTURE0                       0x84C0
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.5
//
#define GL_ELEMENT_ARRAY_BUFFER                   0x8893
#define GL_BUFFER_SIZE                            0x8764
	// GL types for handling large vertex buffer objects.
	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;

	extern void glGenBuffers(GLsizei n, GLuint *buffers);
	extern void glBindBuffer(GLenum target, GLuint buffer);
	extern void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	extern void glDeleteBuffers(GLsizei n, const GLuint *buffers);
	extern void glGenVertexArrays(GLsizei n, GLuint *arrays);
	extern void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params);
	extern void glActiveTexture(GLenum texture);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 2.0
//
typedef char GLchar;

#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_RENDERBUFFER                   0x8D41
	
	extern void glAttachShader(GLuint program, GLuint shader);
	extern void glCompileShader(GLuint shader);
	extern GLuint glCreateProgram();
	extern GLuint glCreateShader(GLenum type);
	extern void glDeleteProgram(GLuint program);
	extern void glDeleteShader(GLuint shader);
	extern void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
	extern void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog);
	extern void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
	extern void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog);
	extern GLint glGetUniformLocation(GLuint program, const char *name);
	extern void glLinkProgram(GLuint program);
	extern void glShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length);
	extern void glUseProgram(GLuint program);
	extern void glUniform1i(GLint location, GLint v0);
	extern void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	extern void glUniform1f(GLint location, GLfloat v0);
	extern void glEnableVertexAttribArray(GLuint index);
	extern void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
	extern void glDisableVertexAttribArray(GLuint index);
	extern void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	extern void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);
	extern GLint glGetAttribLocation(GLuint program, const GLchar *name);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 4.0
//
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RGBA32F                        0x8814
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9

	extern void glBindFramebuffer(GLenum target, GLuint framebuffer);
	extern void glBindVertexArray(GLuint array);
	extern void glGenFramebuffers(GLsizei n, GLuint *ids);
	extern void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	extern void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
	extern void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	extern void glGenRenderbuffers(GLsizei n, GLuint * renderbuffers);
	extern void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	extern void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	extern GLenum glCheckFramebufferStatus(GLenum target);
	extern void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	extern void glGenerateMipmap(GLenum target);
	
} // extern "C"
#endif