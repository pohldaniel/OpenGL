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
	Shader() = default;
	Shader(std::string vertex, std::string fragment, bool fromFile = true);
	Shader(Shader* shader);
	~Shader();

	void loadFromFile(std::string vertex, std::string fragment);
	void loadFromResource(std::string vertex, std::string fragment);
	Shader& get();

	void loadMatrix(const char* location, const Matrix4f matrix, bool trans = true);
	void loadMatrixArray(const char* location, const std::vector<Matrix4f> matrixArray, const short count);
	void loadVector(const char* location, Vector4f vector);
	void loadVector(const char* location, Vector3f vector);
	void loadVector(const char* location, Vector2f vector);
	void loadFloat4(const char* location, float value[4]);
	void loadFloat3(const char* location, float value[3]);
	void loadFloat2(const char* location, float value[2]);
	void loadFloat1(const char* location, float value[1]);
	void loadFloat(const char* location, float value);
	//void loadFloat(const char* location, float value[2]);
	//void loadFloat(const char* location, float value[3]);
	void loadFloat(const char* location, float value[4]);

	void loadBool(const char* location, bool value);
	void loadInt(const char* location, int value);

	GLuint m_program;

protected:

	GLuint createProgramFromFile(std::string vertex, std::string fragment);
	GLuint createProgram(std::string vertex, std::string fragment);

	void readTextFile(const char *pszFilename, std::string &buffer);
	GLuint loadShaderProgram(GLenum type, const char *pszFilename);
	GLuint loadShaderProgram(GLenum type, std::string buffer);
	GLuint compileShader(GLenum type, const char *pszSource);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);

	void cleanup();


};
#endif // __shaderH__