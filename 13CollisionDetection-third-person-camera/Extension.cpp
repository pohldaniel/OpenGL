#include <cassert>

#include "Extension.h"


void glUseProgram(GLuint program)
{
	//-------------------------------------------------------------------
	//#define APIENTRY WINAPI
	//#define WINAPI __stdcall
	//__stdcall is a calling convention for Win32-API-Functioncs:
	//a way of determining how parameters are passed to a 
	//function(on the stack or in registers) and who is responsible
	//for cleaning up after the function returns(the caller or the callee)
	//Syntax: return-type __stdcall function-name[(argument-list)] 
	//--------------------------------------------------------------------

	// Declare typedef 
	//PFNGLUSEPROGRAMPROC: Pointer to the FunctoiN glUseProgram, wich is a PROCedur

	typedef void (APIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
	//typedef void(__stdcall *PFNGLUSEPROGRAMPROC) (GLuint program);

	// Define function-pointer variable, and initialise
	static PFNGLUSEPROGRAMPROC pfnUseProgram = 0;

	//Load Entrypoint 

	// The wglGetProcAddress function returns 
	// the address of an OpenGL extension 
	// function for use with the current OpenGL rendering context.

	// reinterpret_cast :convert integer types to pointer types
	//					 and vice versa or to convert one pointer 
	//					 type to another
	//					 reinterpret_cast only guarantees that if 
	//					 you cast a pointer to a different type, 
	//					 and then reinterpret_cast it back to the
	//				     original type, you get the original value. 
	//					 So in the following :

	//					 int* a = new int();
	//					 void* b = reinterpret_cast<void*>(a);
	//					 int* c = reinterpret_cast<int*>(b);
	//					 a and c contain the same value, but the 
	//					 value of b is unspecified.

	// static_casting: a pointer to and from void* preserves the address
	//				   That is, in the following, a, b and c all point to the same address:

	//				   int* a = new int();
	//				   void* b = static_cast<void*>(a);
	//				   int* c = static_cast<int*>(b);

	//				   static_casting also internally used by C++ for implicit casts

	pfnUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(wglGetProcAddress("glUseProgram"));

	//terminate the programm in case the pointer is not zero

	//process the parameter
	pfnUseProgram(program);
}


//Use a macro to load the entrypoint
#define LOAD_ENTRYPOINT(name, var, type) \
if (!var) \
{ \
	var = reinterpret_cast<type>(wglGetProcAddress(name)); \
	assert(var != 0); \
}


//
// OpenGL 1.5
//
void glBindBuffer(GLenum target, GLuint buffer)
{
	typedef void (APIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
	static PFNGLBINDBUFFERPROC pfnBindBuffer = 0;
	LOAD_ENTRYPOINT("glBindBuffer", pfnBindBuffer, PFNGLBINDBUFFERPROC);
	pfnBindBuffer(target, buffer);
}
void glDeleteBuffers(GLsizei n, const GLuint *buffers)
{
	typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
	static PFNGLDELETEBUFFERSPROC pfnDeleteBuffers = 0;
	LOAD_ENTRYPOINT("glDeleteBuffers", pfnDeleteBuffers, PFNGLDELETEBUFFERSPROC);
	pfnDeleteBuffers(n, buffers);
}
void glGenBuffers(GLsizei n, GLuint *buffers)
{
	typedef void (APIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
	static PFNGLGENBUFFERSPROC pfnGenBuffers = 0;
	LOAD_ENTRYPOINT("glGenBuffers", pfnGenBuffers, PFNGLGENBUFFERSPROC);
	pfnGenBuffers(n, buffers);
}

void glGenVertexArrays(GLsizei n, GLuint *arrays)
{
	typedef void (APIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
	static PFNGLGENVERTEXARRAYSPROC pfnGenVertexArrays = 0;
	LOAD_ENTRYPOINT("glGenVertexArrays", pfnGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
	pfnGenVertexArrays(n, arrays);
}


void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	typedef void (APIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	static PFNGLBUFFERDATAPROC pfnBufferData = 0;
	LOAD_ENTRYPOINT("glBufferData", pfnBufferData, PFNGLBUFFERDATAPROC);
	pfnBufferData(target, size, data, usage);
}

void glActiveTexture(GLenum texture)
{
	typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
	static PFNGLACTIVETEXTUREPROC pfnActiveTexture = 0;
	LOAD_ENTRYPOINT("glActiveTexture", pfnActiveTexture, PFNGLACTIVETEXTUREPROC);
	pfnActiveTexture(texture);
}
//
// OpenGL 2.0
//
void glAttachShader(GLuint program, GLuint shader)
{
	typedef void (APIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
	static PFNGLATTACHSHADERPROC pfnAttachShader = 0;
	LOAD_ENTRYPOINT("glAttachShader", pfnAttachShader, PFNGLATTACHSHADERPROC);
	pfnAttachShader(program, shader);
}
void glCompileShader(GLuint shader)
{
	typedef void (APIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
	static PFNGLCOMPILESHADERPROC pfnCompileShader = 0;
	LOAD_ENTRYPOINT("glCompileShader", pfnCompileShader, PFNGLCOMPILESHADERPROC);
	pfnCompileShader(shader);
}
GLuint glCreateProgram()
{
	typedef GLuint(APIENTRY * PFNGLCREATEPROGRAMPROC) ();
	static PFNGLCREATEPROGRAMPROC pfnCreateProgram = 0;
	LOAD_ENTRYPOINT("glCreateProgram", pfnCreateProgram, PFNGLCREATEPROGRAMPROC);
	return pfnCreateProgram();
}
void glValidateProgram(GLuint program) 
{
	typedef GLuint(APIENTRY * PFNGLVALIDATEPROGRAMPROC) (GLuint program);
	static PFNGLVALIDATEPROGRAMPROC pfnValidateProgram = 0;
	LOAD_ENTRYPOINT("glValidateProgram", pfnValidateProgram, PFNGLVALIDATEPROGRAMPROC);
	pfnValidateProgram(program);

}
GLuint glCreateShader(GLenum type)
{
	typedef GLuint(APIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
	static PFNGLCREATESHADERPROC pfnCreateShader = 0;
	LOAD_ENTRYPOINT("glCreateShader", pfnCreateShader, PFNGLCREATESHADERPROC);
	return pfnCreateShader(type);
}
void glDeleteProgram(GLuint program)
{
	typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
	static PFNGLDELETEPROGRAMPROC pfnDeleteProgram = 0;
	LOAD_ENTRYPOINT("glDeleteProgram", pfnDeleteProgram, PFNGLDELETEPROGRAMPROC);
	pfnDeleteProgram(program);
}
void glDeleteShader(GLuint shader)
{
	typedef void (APIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
	static PFNGLDELETESHADERPROC pfnDeleteShader = 0;
	LOAD_ENTRYPOINT("glDeleteShader", pfnDeleteShader, PFNGLDELETESHADERPROC);
	pfnDeleteShader(shader);
}

void glDetachShader(GLuint program, GLuint shader) 
{
	typedef void (APIENTRY * PFNGLDETACHHADERPROC) (GLuint program, GLuint shader);
	static PFNGLDETACHHADERPROC pfnDetachShader = 0;
	LOAD_ENTRYPOINT("glDetachShader", pfnDetachShader, PFNGLDETACHHADERPROC);
	pfnDetachShader(program, shader);

}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
	typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
	static PFNGLGETPROGRAMIVPROC pfnGetProgramiv = 0;
	LOAD_ENTRYPOINT("glGetProgramiv", pfnGetProgramiv, PFNGLGETPROGRAMIVPROC);
	pfnGetProgramiv(program, pname, params);
}
void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog)
{
	typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog);
	static PFNGLGETPROGRAMINFOLOGPROC pfnGetProgramInfoLog = 0;
	LOAD_ENTRYPOINT("glGetProgramInfoLog", pfnGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
	pfnGetProgramInfoLog(program, bufSize, length, infoLog);
}
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
	typedef void (APIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
	static PFNGLGETSHADERIVPROC pfnGetShaderiv = 0;
	LOAD_ENTRYPOINT("glGetShaderiv", pfnGetShaderiv, PFNGLGETSHADERIVPROC);
	pfnGetShaderiv(shader, pname, params);
}
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog)
{
	typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog);
	static PFNGLGETSHADERINFOLOGPROC pfnGetShaderInfoLog = 0;
	LOAD_ENTRYPOINT("glGetShaderInfoLog", pfnGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
	pfnGetShaderInfoLog(shader, bufSize, length, infoLog);
}
GLint glGetUniformLocation(GLuint program, const char *name)
{
	typedef GLint(APIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const char *name);
	static PFNGLGETUNIFORMLOCATIONPROC pfnGetUniformLocation = 0;
	LOAD_ENTRYPOINT("glGetUniformLocation", pfnGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
	return pfnGetUniformLocation(program, name);
}
void glLinkProgram(GLuint program)
{
	typedef void (APIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
	static PFNGLLINKPROGRAMPROC pfnLinkProgram = 0;
	LOAD_ENTRYPOINT("glLinkProgram", pfnLinkProgram, PFNGLLINKPROGRAMPROC);
	pfnLinkProgram(program);
}
void glShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length)
{
	typedef void (APIENTRY * PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const char* *string, const GLint *length);
	static PFNGLSHADERSOURCEPROC pfnShaderSource = 0;
	LOAD_ENTRYPOINT("glShaderSource", pfnShaderSource, PFNGLSHADERSOURCEPROC);
	pfnShaderSource(shader, count, string, length);
}
void glUniform1i(GLint location, GLint v0)
{
	typedef void (APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
	static PFNGLUNIFORM1IPROC pfnUniform1i = 0;
	LOAD_ENTRYPOINT("glUniform1i", pfnUniform1i, PFNGLUNIFORM1IPROC);
	pfnUniform1i(location, v0);
}

void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	typedef void (APIENTRY * PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	static PFNGLUNIFORM3FPROC pfnUniform3f = 0;
	LOAD_ENTRYPOINT("glUniform3f", pfnUniform3f, PFNGLUNIFORM3FPROC);
	pfnUniform3f(location, v0, v1, v2);
}

void glUniform1f(GLint location, GLfloat v0)
{
	typedef void (APIENTRY * PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
	static PFNGLUNIFORM1FPROC pfnUniform1f = 0;
	LOAD_ENTRYPOINT("glUniform1f", pfnUniform1f, PFNGLUNIFORM1FPROC);
	pfnUniform1f(location, v0);
}

void glUniform2f(GLint location, GLfloat v0, GLfloat v1){

	typedef void (APIENTRY * PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
	static PFNGLUNIFORM2FPROC pfnUniform2f = 0;
	LOAD_ENTRYPOINT("glUniform2f", pfnUniform2f, PFNGLUNIFORM2FPROC);
	pfnUniform2f(location, v0, v1);
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){

	typedef void (APIENTRY * PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	static PFNGLUNIFORM4FPROC pfnUniform4f = 0;
	LOAD_ENTRYPOINT("glUniform4f", pfnUniform4f, PFNGLUNIFORM4FPROC);
	pfnUniform4f(location, v0, v1, v2, v3);
}


void glUniform1fv(GLint location, GLsizei count, const GLfloat *value)
{
	typedef void (APIENTRY * PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	static PFNGLUNIFORM1FVPROC pfnUniform1fv = 0;
	LOAD_ENTRYPOINT("glUniform1fv", pfnUniform1fv, PFNGLUNIFORM1FVPROC);
	pfnUniform1fv(location, count, value);

}

void glEnableVertexAttribArray(GLuint index)
{
	typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
	static PFNGLENABLEVERTEXATTRIBARRAYPROC pfnEnableVertexAttribArray = 0;
	LOAD_ENTRYPOINT("glEnableVertexAttribArray", pfnEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
	pfnEnableVertexAttribArray(index);
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
	typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
	static PFNGLVERTEXATTRIBPOINTERPROC pfnVertexAttribPointer = 0;
	LOAD_ENTRYPOINT("glVertexAttribPointer", pfnVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
	pfnVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{

	typedef void (APIENTRY * PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	static PFNGLVERTEXATTRIBIPOINTERPROC pfnVertexAttribIPointer = 0;
	LOAD_ENTRYPOINT("glVertexAttribIPointer", pfnVertexAttribIPointer, PFNGLVERTEXATTRIBIPOINTERPROC);
	pfnVertexAttribIPointer(index, size, type, stride, pointer);
}


void glDisableVertexAttribArray(GLuint index)
{
	typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC pfnDisableVertexAttribArray = 0;
	LOAD_ENTRYPOINT("glDisableVertexAttribArray", pfnDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);
	pfnDisableVertexAttribArray(index);
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) 
{
	typedef void (APIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
	static PFNGLDELETEVERTEXARRAYSPROC pfnDeleteVertexArrays = 0;
	LOAD_ENTRYPOINT("glDeleteVertexArrays", pfnDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
	pfnDeleteVertexArrays(n, arrays);

}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	static PFNGLUNIFORMMATRIX4FVPROC pfnUniformMatrix4fv = 0;
	LOAD_ENTRYPOINT("glUniformMatrix4fv", pfnUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
	pfnUniformMatrix4fv(location, count, transpose, value);
}

void glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
{
	typedef void (APIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	static PFNGLUNIFORM3FVPROC pfnUniform3fv = 0;
	LOAD_ENTRYPOINT("glUniform3fv", pfnUniform3fv, PFNGLUNIFORM3FVPROC);
	pfnUniform3fv(location, count, value);
}

void glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
	typedef void (APIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
	static PFNGLUNIFORM4FVPROC pfnUniform4fv = 0;
	LOAD_ENTRYPOINT("glUniform4fv", pfnUniform4fv, PFNGLUNIFORM4FVPROC);
	pfnUniform4fv(location, count, value);
}

void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name){

	typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
	static PFNGLBINDATTRIBLOCATIONPROC pfnBindAttribLocation = 0;
	LOAD_ENTRYPOINT("glBindAttribLocation", pfnBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC);
	pfnBindAttribLocation(program, index, name);
}

GLint glGetAttribLocation(GLuint program, const GLchar *name){

	typedef GLint(APIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
	static PFNGLGETATTRIBLOCATIONPROC pfnGetAttribLocation = 0;
	LOAD_ENTRYPOINT("glGetAttribLocation", pfnGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);
	return pfnGetAttribLocation(program,  name);
}

void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params){

	typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
	static PFNGLGETBUFFERPARAMETERIVPROC pfnGetBufferParameteriv = 0;
	LOAD_ENTRYPOINT("GetBufferParameteriv", pfnGetBufferParameteriv, PFNGLGETBUFFERPARAMETERIVPROC);
	pfnGetBufferParameteriv(target, pname, params);

}

//
// OpenGL 4.0
//

void glGenFramebuffers(GLsizei n, GLuint *ids){

	typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint *ids);
	static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = 0;
	LOAD_ENTRYPOINT("glGenFramebuffers", glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
	glGenFramebuffers(n, ids);

}

void glBindVertexArray(GLuint array){

	typedef void (APIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
	static PFNGLBINDVERTEXARRAYPROC glBindVertexArray = 0;
	LOAD_ENTRYPOINT("glBindVertexArray", glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
	glBindVertexArray(array);
}

void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){

	typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = 0;
	LOAD_ENTRYPOINT("glFramebufferTexture2D", glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glBindFramebuffer(GLenum target, GLuint framebuffer){

	typedef void (APIENTRY * PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
	static PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = 0;
	LOAD_ENTRYPOINT("glBindFramebuffer", glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
	glBindFramebuffer(target, framebuffer);
}

void glBindRenderbuffer(GLenum target, GLuint renderbuffer){

	typedef void (APIENTRY * PFNGLBINDRENDERBUFFERPROC) (GLenum target, GLuint renderbuffer);
	static PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = 0;
	LOAD_ENTRYPOINT("glBindRenderbuffer", glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC);
	glBindRenderbuffer(target, renderbuffer);

}

void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height){

	typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	static PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = 0;
	LOAD_ENTRYPOINT("glRenderbufferStorage", glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC);
	glRenderbufferStorage(target, internalformat, width, height);
}

void glGenRenderbuffers(GLsizei n, GLuint * renderbuffers){

	typedef void (APIENTRY * PFNGLGENRENDERBUFFERSTPROC) (GLsizei n, GLuint * renderbuffers);
	static PFNGLGENRENDERBUFFERSTPROC glGenRenderbuffers = 0;
	LOAD_ENTRYPOINT("glGenRenderbuffers", glGenRenderbuffers, PFNGLGENRENDERBUFFERSTPROC);
	glGenRenderbuffers(n, renderbuffers);
}

void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height){

	typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	static PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = 0;
	LOAD_ENTRYPOINT("glRenderbufferStorageMultisample", glRenderbufferStorageMultisample, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC);
	glRenderbufferStorageMultisample(target, samples, internalformat, width , height);
}

void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){

	typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFERPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	static PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = 0;
	LOAD_ENTRYPOINT("glFramebufferRenderbuffer", glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC);
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

GLenum glCheckFramebufferStatus(GLenum target){

	typedef GLenum (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSPROC) (GLenum target);
	static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = 0;
	LOAD_ENTRYPOINT("glCheckFramebufferStatus", glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC);
	return glCheckFramebufferStatus(target);
}

void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){

	typedef void(APIENTRY * PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	static PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = 0;
	LOAD_ENTRYPOINT("glBlitFramebuffer", glBlitFramebuffer, PFNGLBLITFRAMEBUFFERPROC);
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void glGenerateMipmap(GLenum target){

	typedef void(APIENTRY * PFNGLGENERATEMIPMAPPROC) (GLenum target);
	static PFNGLGENERATEMIPMAPPROC glGenerateMipmap = 0;
	LOAD_ENTRYPOINT("glGenerateMipmap", glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);
	glGenerateMipmap(target);
}

void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data){

	typedef void(APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);
	static PFNGLCOMPRESSEDTEXIMAGE2DPPROC glCompressedTexImage2D = 0;
	LOAD_ENTRYPOINT("glCompressedTexImage2D", glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPPROC);
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data){

	typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data);
	static PFNGLTEXIMAGE3DPROC glTexImage3D = 0;
	LOAD_ENTRYPOINT("glTexImage3D", glTexImage3D, PFNGLTEXIMAGE3DPROC);
	glTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, data);
}

void glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer){

	typedef void (APIENTRY * PFNGLGLFRAMEBUFFER3DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
	static PFNGLGLFRAMEBUFFER3DPROC glFramebufferTexture3D = 0;
	LOAD_ENTRYPOINT("glFramebufferTexture3D", glFramebufferTexture3D, PFNGLGLFRAMEBUFFER3DPROC);
	glFramebufferTexture3D(target, attachment, textarget, texture, level, layer);
}

void glDrawBuffers(GLsizei n, const GLenum *bufs){

	typedef void (APIENTRY * PFNGLGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);
	static PFNGLGLDRAWBUFFERSPROC glDrawBuffers = 0;
	LOAD_ENTRYPOINT("glDrawBuffers", glDrawBuffers, PFNGLGLDRAWBUFFERSPROC);
	glDrawBuffers(n, bufs);
}


void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){

	typedef void(APIENTRY * PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	static PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = 0;
	LOAD_ENTRYPOINT("glFramebufferTextureLayer", glFramebufferTextureLayer, PFNGLFRAMEBUFFERTEXTURELAYERPROC);
	glFramebufferTextureLayer(target, attachment, texture, level, layer);
}



void glActiveTextureARB(GLenum texture)
{
	typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
	static PFNGLACTIVETEXTUREARBPROC pfnActiveTextureARB = 0;
	LOAD_ENTRYPOINT("glActiveTextureARB", pfnActiveTextureARB, PFNGLACTIVETEXTUREARBPROC);
	pfnActiveTextureARB(texture);
}

void glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat s, GLfloat t);
	static PFNGLMULTITEXCOORD2FARBPROC pfnMultiTexCoord2fARB = 0;
	LOAD_ENTRYPOINT("glMultiTexCoord2fARB", pfnMultiTexCoord2fARB, PFNGLMULTITEXCOORD2FARBPROC);
	pfnMultiTexCoord2fARB(target, s, t);
}