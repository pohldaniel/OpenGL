#ifndef __shaderH__
#define __shaderH__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Extension.h"
#include "Vector.h"

class Shader {

public:
	Shader(std::string vertex, std::string fragment);
	Shader(Shader* shader);
	virtual ~Shader();

	GLuint m_program;

	void loadSampler(const char* location, int sampler);
	void loadMatrix(const char* location, const Matrix4f matrix, bool trans = true);
	void loadMatrixArray(const char* location, const std::vector<Matrix4f> matrixArray, const short count);
	void loadVector(const char* location, Vector3f vector);
	void loadVector(const char* location, Vector2f vector);
	void loadFloat(const char* location, float value[2]);
	void loadFloat(const char* location, float value);
	void loadBool(const char* location, bool value);

protected:
	GLuint createProgram(std::string vertex, std::string fragment);

	void readTextFile(const char *pszFilename, std::string &buffer);
	GLuint loadShaderProgram(GLenum type, const char *pszFilename);
	GLuint compileShader(GLenum type, const char *pszSource);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);

	void cleanup();
};
#endif // __shaderH__