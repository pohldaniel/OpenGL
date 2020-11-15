#ifndef ANIMATION_SHADER_H
#define ANIMATION_SHADER_H

#include <string>
#include <vector>

#include "..\..\Camera.h"
#include "..\..\Shader.h"

static const unsigned int MAX_JOINTS = 50;
class AnimatedModel;

class AnimationShader : public Shader{

public:
	AnimationShader(const std::string& vertex, const std::string& fragment);
	virtual ~AnimationShader();

	void update(const AnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector);
	void bind();
};
#endif