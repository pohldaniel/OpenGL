#ifndef SHADER2_H
#define SHADER2_H

#include <string>
#include "..\Extension.h"
#include "..\Camera.h"

class BaseShader{

public:
	virtual void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	virtual std::string LoadShader(const std::string& filename);
	virtual GLuint CreateShader(const std::string& text, GLenum shaderType);
};

class AnimatedModel;
class Shader2 : public BaseShader{

public:
	Shader2(const std::string& filename);
	virtual ~Shader2();

	void Update(const AnimatedModel& model, const Camera& camera);
	void Bind();

private:
	static const unsigned int NUM_SHADERS = 2;

	enum{
		TRANSFORM_U,
		VIEW_PROJECTION_U,

		NUM_UNIFORMS
	};

	GLuint _program;
	GLuint _uniforms[NUM_UNIFORMS];
	GLuint _shaders[NUM_SHADERS];
};
#endif