#ifndef __shaderH__
#define __shaderH__

#include <iostream>
#include <fstream>
#include <sstream>

#include "Extension.h"
#include "Vector.h"
#include "Model.h"





struct LightSource{
	Vector3f ambient;
	Vector3f diffuse;
	Vector3f specular;
	Vector3f position;
};


class Shader{

	

public:
	
	Shader(std::string vertex, std::string fragment);
	Shader(Shader* shader);
	virtual ~Shader();

	
	GLuint positionID, texCoordID, normalID, tangentID, bitangentID;
	GLuint m_program;

	void Shader::loadSampler(const char* location, int sampler);
	void loadMatrix(const char* location, const Matrix4f matrix);
	void loadVector(const char* location, Vector3f vector);
	void loadFloat2(const char* location, float value[2]);
	void loadFloat(const char* location, float value);
	void loadBool(const char* location, bool value);
	
	virtual void bindAttributes(Mesh *a_mesh);
	virtual void unbindAttributes(Mesh *a_mesh);
	
	

	void loadMaterial(const Mesh::Material material);
	void loadLightSource(LightSource &lightsource, int index);
	void loadLightSources(std::vector<LightSource> lights);
	
	

protected:

	GLuint createProgram(std::string vertex, std::string fragment);
	void getAttributeGetAttribLocation();
	void loadSampler();

	void readTextFile(const char *pszFilename, std::string &buffer);
	GLuint loadShaderProgram(GLenum type, const char *pszFilename);
	GLuint compileShader(GLenum type, const char *pszSource);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);

	void cleanup();

};

class SkyboxShader : public Shader{

public:

	SkyboxShader(std::string vertex, std::string fragment, GLuint cubemap);
	~SkyboxShader();

	
	
	void bindAttributes();
	void unbindAttributes();

private:

	GLuint m_cubemap;
};

class EnvironmentMap : public Shader{

public:

	EnvironmentMap(std::string vertex, std::string fragment, GLuint cubemap);
	EnvironmentMap(EnvironmentMap* shader);
	~EnvironmentMap();

	

	void bindAttributes(Mesh *a_mesh);
	void unbindAttributes(Mesh *a_mesh);

private:

	GLuint m_cubemap;
};

#endif // __shaderH__