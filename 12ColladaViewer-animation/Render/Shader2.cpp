#include "Shader2.h"
#include <iostream>
#include <fstream>

#include "../AnimatedModel/AnimatedModel.h"

Shader2::Shader2(const std::string& filename) {

	_program = glCreateProgram();
	_shaders[0] = CreateShader(LoadShader(filename + ".glvs"), GL_VERTEX_SHADER);
	_shaders[1] = CreateShader(LoadShader(filename + ".glfs"), GL_FRAGMENT_SHADER);

	for (unsigned int i = 0; i < NUM_SHADERS; ++i){
		glAttachShader(_program, _shaders[i]);
	}

	glBindAttribLocation(_program, 0, "position");
	glBindAttribLocation(_program, 1, "texCoord");
	glBindAttribLocation(_program, 2, "normal");

	glLinkProgram(_program);
	CheckShaderError(_program, GL_LINK_STATUS, true, "Error: Program linking failed. ");

	glValidateProgram(_program);
	CheckShaderError(_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid. ");

	_uniforms[TRANSFORM_U] = glGetUniformLocation(_program, "transform");
	_uniforms[VIEW_PROJECTION_U] = glGetUniformLocation(_program, "viewProjection");
}

void Shader2::Update(const AnimatedModel& model, const Camera& camera){

	glUniformMatrix4fv(glGetUniformLocation(_program, "u_model"), 1, GL_TRUE, &model.getTransformationMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "u_view"), 1, GL_TRUE, &camera.getViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "u_projection"), 1, GL_TRUE, &camera.getProjectionMatrix()[0][0]);
}

void Shader2::Bind(){

	for (unsigned int i = 0; i < NUM_SHADERS; ++i){
		glDetachShader(_program, _shaders[i]);
		glDeleteShader(_shaders[i]);
	}
	glUseProgram(_program);
}

Shader2::~Shader2(){
	glDeleteProgram(_program);
}

GLuint BaseShader::CreateShader(const std::string& text, GLenum shaderType){
	GLuint shader = glCreateShader(shaderType);

	if (shader == 0) std::cerr << "Error: Shader creation failed!" << std::endl;
	
	const GLchar* shaderSourceStrings[1];
	GLint shaderSourceStringsLengths[1];
	
	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringsLengths[0] = text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation error. ");

	return shader;
}

std::string BaseShader::LoadShader(const std::string& filename){
	std::ifstream file;
	file.open((filename).c_str());

	std::string output;
	std::string line;

	if (file.is_open()){
		while (file.good()){
			getline(file, line);
			output.append(line + "\n");
		}
	}else{
		std::cerr << "Unable to load shader: " << filename << std::endl;
	}
	return output;
}

void BaseShader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage){
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram){
		glGetProgramiv(shader, flag, &success);

	}else{
		glGetShaderiv(shader, flag, &success);

	}

	if (success == GL_FALSE){
		if (isProgram){
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);

		}else{
			glGetShaderInfoLog(shader, flag, NULL, error);
		}
		std::cerr << errorMessage << ": " << error << std::endl;
	}
}
