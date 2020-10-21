#include <iostream>
#include <fstream>

#include "AnimationShader.h"
#include "../AnimatedModel/AnimatedModel.h"

AnimationShader::AnimationShader() {

}

AnimationShader::AnimationShader(const std::string& filename){

	_program = glCreateProgram();
	_shaders[0] = CreateShader(LoadShader(filename + ".glvs"), GL_VERTEX_SHADER);
	_shaders[1] = CreateShader(LoadShader(filename + ".glfs"), GL_FRAGMENT_SHADER);

	for (unsigned int i = 0; i < NUM_SHADERS; ++i) {
		glAttachShader(_program, _shaders[i]);
	}

	glBindAttribLocation(_program, 0, "position");
	glBindAttribLocation(_program, 1, "texCoord");
	glBindAttribLocation(_program, 2, "normal");
	glBindAttribLocation(_program, 3, "jointIds");
	glBindAttribLocation(_program, 4, "jointWeights");

	glLinkProgram(_program);
	CheckShaderError(_program, GL_LINK_STATUS, true, "Error: Program linking failed. ");

	glValidateProgram(_program);
	CheckShaderError(_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid. ");

	_uniforms[TRANSFORM_U] = glGetUniformLocation(_program, "transform");
	_uniforms[VIEW_PROJECTION_U] = glGetUniformLocation(_program, "viewProjection");
	_uniforms[JOINT_TRANSFORMS_U] = glGetUniformLocation(_program, "jointTransforms");
}

void AnimationShader::Update(const AnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector){

	Matrix4f jointTransforms[MAX_JOINTS];

	for (unsigned int i = 0; i < (jointVector.size() < MAX_JOINTS ? jointVector.size() : MAX_JOINTS); ++i) {
		jointTransforms[i] = jointVector[i];
	}
	glUniformMatrix4fv(_uniforms[JOINT_TRANSFORMS_U], MAX_JOINTS, GL_FALSE, jointTransforms[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(_program, "u_model"), 1, GL_TRUE, &model.getTransformationMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "u_view"), 1, GL_TRUE, &camera.getViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "u_projection"), 1, GL_TRUE, &camera.getProjectionMatrix()[0][0]);

	

}

void AnimationShader::Bind(){

	for (unsigned int i = 0; i < NUM_SHADERS; ++i){
		glDetachShader(_program, _shaders[i]);
		glDeleteShader(_shaders[i]);
	}
	glUseProgram(_program);
}

AnimationShader::~AnimationShader(){
	glDeleteProgram(_program);
}