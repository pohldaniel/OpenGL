#if !defined(EXTENSION_H)
#define EXTENSION_H

#include <windows.h>
#include <GL/gl.h>


extern "C" {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.2
//
#define GL_BGR							  0x80E0
#define GL_BGRA                           0x80E1
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_R8                             0x8229
#define GL_RGB32F_ARB					  0x8815
#define GL_RG							  0x8227
#define GL_R16F							  0x822D
#define GL_R32F							  0x822E
#define GL_HALF_FLOAT					  0x140B
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.4
//
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_GENERATE_MIPMAP                0x8191
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1						  0x84C1
#define GL_TEXTURE2						  0x84C2
#define	GL_TEXTURE3						  0x84C3
#define GL_TEXTURE4						  0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_DEPTH_COMPONENT24			  0x81A6
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_TEXTURE_COMPARE_FUNC           0x884D
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

	extern void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 2.0
//
typedef char GLchar;

#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_GEOMETRY_SHADER				  0x8DD9
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

	extern void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
	extern void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

	extern void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);
	extern GLint glGetAttribLocation(GLuint program, const GLchar *name);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 4.0
//
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RGBA32F                        0x8814
#define GL_RGB32F						  0x8815
#define GL_RGBA16F						  0x881A
#define GL_RGB16F						  0x881B
#define GL_RG16F						  0x822F
#define GL_RG32F						  0x8230
#define GL_DEPTH24_STENCIL8				  0x88F0
#define GL_DEPTH_STENCIL				  0x84F9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1			  0x8CE1
#define GL_COLOR_ATTACHMENT2			  0x8CE2
#define GL_COLOR_ATTACHMENT3			  0x8CE3
#define GL_COLOR_ATTACHMENT4			  0x8CE4
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_DEPTH_STENCIL_ATTACHMENT		  0x821A
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
	extern void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3D Texture
//
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_READ_COLOR 0x891C

	extern void glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data);
	extern void glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
	extern void glDrawBuffers(GLsizei n, const GLenum *bufs);
	extern GLenum glCheckFramebufferStatus(GLenum target);
	extern void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	extern void glClampColor(GLenum target, GLenum clamp);
	extern void glDepthRangef(GLfloat nearVal, GLfloat farVal);
	
} // extern "C"
#endif