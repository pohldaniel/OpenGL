#ifndef __shaderH__
#define __shaderH__

#include <iostream>
#include <fstream>
#include <sstream>

#include "Extension.h"
#include "Vector.h"
#include "Model.h"



struct Material{
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	float specularShininesse;
};

struct LightSource{
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	Vector3f position;
};

enum SelectedShader{ TEXTURE, PHONG, NORMAL };

class Shader{

	

public:
	Shader();
	Shader(std::string vertex, std::string fragment);
	~Shader();

	GLuint u_projection, u_modelView, u_modelViewLight, u_view, u_normalMatrix, u_active;
	GLuint positionID, texCoordID, normalID, tangentID, bitangentID;
	GLuint m_program;

	

	void loadMatrix(GLuint location, const Matrix4f matrix);
	void loadVector(GLuint location, const Vector3f vector);
	void loadFloat2d(GLuint location, float value[2]);

	void loadProjectionMatrix(const Matrix4f projectionMatrix);
	void loadModelViewMatrix(const Matrix4f modelViewMatrix);
	void loadViewMatrix(const Matrix4f normalMatrix);
	void loadNormalMatrix(const Matrix4f normalMatrix);
	void loadModelViewLightMatrix(const Matrix4f normalMatrix);
	
	void bindAttributes(Mesh *a_mesh);
	void unbindAttributes(Mesh *a_mesh);
	
	

	void loadMaterial(Material &material);
	void loadLightSource(LightSource &lightsource, int index);
	void loadLightSources(std::vector<LightSource> lights);

protected:

	GLuint createProgram(std::string vertex, std::string fragment);
	void getAttributeGetAttribLocation();
	void getUniformLocation();

	void readTextFile(const char *pszFilename, std::string &buffer);
	GLuint loadShaderProgram(GLenum type, const char *pszFilename);
	GLuint compileShader(GLenum type, const char *pszSource);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);

	void cleanup();

};

class PhongShader : public Shader{

public:
	
	PhongShader();
	PhongShader(std::string vertex, std::string fragment);
	~PhongShader();

	
	


	
};

class NormalShader : public PhongShader{

public:

	NormalShader();
	NormalShader(std::string vertex, std::string fragment);
	~NormalShader();
	
	

private:
	void getAttributeGetAttribLocation();
	void loadSampler();
};

#endif // __shaderH__