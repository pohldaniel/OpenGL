#ifndef ANIMATION_SHADER_H
#define ANIMATION_SHADER_H

#include <string>
#include <vector>

#include "Shader2.h"
#include "..\Extension.h"
#include "..\Camera.h"



static const unsigned int MAX_JOINTS = 50;
class AnimatedModel;

class AnimationShader : public BaseShader{

public:
	AnimationShader(const std::string& filename);
	AnimationShader();
	virtual ~AnimationShader();

	void Update(const AnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector);
	void Bind();

private:
	static const unsigned int NUM_SHADERS = 2;

	enum{
		TRANSFORM_U,
		VIEW_PROJECTION_U,
		JOINT_TRANSFORMS_U,

		NUM_UNIFORMS
	};

	GLuint _program;
	GLuint _uniforms[NUM_UNIFORMS];
	GLuint _shaders[NUM_SHADERS];


};
#endif