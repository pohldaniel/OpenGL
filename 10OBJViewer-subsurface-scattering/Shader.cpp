#include "Shader.h"

Shader::Shader(std::string vertex, std::string fragment){
	
	m_program = createProgram(vertex, fragment);

	getAttributeGetAttribLocation();
	//loadSampler();
}

Shader::Shader(Shader* shader){

	positionID = shader->positionID;
	texCoordID = shader->texCoordID;
	normalID = shader->normalID;
	tangentID = shader->tangentID;
	bitangentID = shader->bitangentID;
	m_program = shader->m_program;
}



Shader::~Shader(){
	cleanup();
}

void Shader::getAttributeGetAttribLocation(){

	positionID = glGetAttribLocation(m_program, "i_position");
	texCoordID = glGetAttribLocation(m_program, "i_texCoord");
	normalID = glGetAttribLocation(m_program, "i_normal");
	tangentID = glGetAttribLocation(m_program, "i_tangent");
	bitangentID = glGetAttribLocation(m_program, "i_bitangent");
}

void Shader::loadSampler(){
	std::cout << "-------------------------" << std::endl;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "u_textureColor"), 0);
	glUniform1i(glGetUniformLocation(m_program, "u_textureNormal"), 1);
	glUniform1i(glGetUniformLocation(m_program, "u_displace"), 2);
	glUniform1i(glGetUniformLocation(m_program, "u_texture"), 3);
	glUniform1i(glGetUniformLocation(m_program, "u_normal"), 4);
	glUniform1i(glGetUniformLocation(m_program, "u_depth"), 5);
	glUniform1i(glGetUniformLocation(m_program, "u_irradiance"), 6);
	glUseProgram(0);
}

void Shader::loadSampler(const char* location, int sampler){
	glUniform1i(glGetUniformLocation(m_program, location), sampler);
}

//OpenGL specifies matrices as column-major to get row-major just transpose it
void Shader::loadMatrix(const char* location, const Matrix4f matrix, bool trans) {
	glUniformMatrix4fv(glGetUniformLocation(m_program, location), 1, trans, &matrix[0][0]);
}

void Shader::loadVector(const char* location,  Vector3f vector){
	glUniform3fv(glGetUniformLocation(m_program, location), 1, &vector[0]);
}

void Shader::loadFloat2(const char* location, float value[2]){
	glUniform1fv(glGetUniformLocation(m_program, location), 2, value);
}

void Shader::loadFloat(const char* location, float value){
	glUniform1f(glGetUniformLocation(m_program, location), value);
}


void Shader::loadBool(const char* location, bool value){
	glUniform1i(glGetUniformLocation(m_program, location), value);
}

void Shader::loadLightSource(LightSource &lightsource, int index = -1){
	
	if (index < 0){
		glUniform3fv(glGetUniformLocation(m_program, "light.ambient"), 1, &lightsource.ambient[0]);
		glUniform3fv(glGetUniformLocation(m_program, "light.diffuse"), 1, &lightsource.diffuse[0]);
		glUniform3fv(glGetUniformLocation(m_program, "light.specular"), 1, &lightsource.specular[0]);
		glUniform3fv(glGetUniformLocation(m_program, "light.position"), 1, &lightsource.position[0]);

	}else{

		std::string tmp = "light[" + std::to_string(index) + "].ambient";
		glUniform3fv(glGetUniformLocation(m_program, tmp.c_str()), 1, &lightsource.ambient[0]);

		tmp = "light[" + std::to_string(index) + "].diffuse";
		glUniform3fv(glGetUniformLocation(m_program, tmp.c_str()), 1, &lightsource.diffuse[0]);

		tmp = "light[" + std::to_string(index) + "].specular";
		glUniform3fv(glGetUniformLocation(m_program, tmp.c_str()), 1, &lightsource.specular[0]);

		tmp = "u_lightPos[" + std::to_string(index) + "]";
		glUniform3fv(glGetUniformLocation(m_program, tmp.c_str()), 1, &lightsource.position[0]);

	}
	
}

/*void Shader::loadLightSources(std::vector<LightSource> lights){
	
	for (int i = 0; i < lights.size(); i++){

		loadLightSource(lights[i], i);

	}
	
}*/

/*void Shader::loadMaterial(const Mesh::Material material){
	
	

	glUseProgram(m_program);
	glUniform3fv(glGetUniformLocation(m_program, "material.ambient"), 1, &material.ambient[0]);
	glUniform3fv(glGetUniformLocation(m_program, "material.diffuse"), 1, &material.diffuse[0]);
	glUniform3fv(glGetUniformLocation(m_program, "material.specular"), 1, &material.specular[0]);
	glUniform1f(glGetUniformLocation(m_program, "material.specularShininesse"), material.shinies);
	glUseProgram(0);
}*/

GLuint Shader::createProgram(std::string vertex, std::string fragment) {


	GLuint vshader = loadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = loadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());

	return linkShaders(vshader, fshader);

}

void Shader::readTextFile(const char *pszFilename, std::string &buffer){

	std::ifstream file(pszFilename, std::ios::binary);

	if (file.is_open())
	{
		file.seekg(0, std::ios::end);

		std::ifstream::pos_type fileSize = file.tellg();

		buffer.resize(static_cast<unsigned int>(fileSize));
		file.seekg(0, std::ios::beg);
		file.read(&buffer[0], fileSize);
	}
}

GLuint Shader::loadShaderProgram(GLenum type, const char *pszFilename){

	GLuint shader = 0;
	std::string buffer;
	readTextFile(pszFilename, buffer);

	if (buffer.length() > 0)
	{


		shader = compileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}

	return shader;
}

GLuint Shader::compileShader(GLenum type, const char *pszSource){

	GLuint shader = glCreateShader(type);

	if (shader)
	{
		GLint compiled = 0;

		glShaderSource(shader, 1, &pszSource, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled){

			GLsizei infoLogSize = 0;
			std::string infoLog;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
			infoLog.resize(infoLogSize);
			glGetShaderInfoLog(shader, infoLogSize, &infoLogSize, &infoLog[0]);
			std::cout << "Compile status: \n" << &infoLog << std::endl;
		}
	}
	return shader;
}

GLuint Shader::linkShaders(GLuint vertShader, GLuint fragShader){

	GLuint program = glCreateProgram();

	if (program){
		GLint linked = 0;

		if (vertShader)
			glAttachShader(program, vertShader);

		if (fragShader)
			glAttachShader(program, fragShader);

		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked){
			GLsizei infoLogSize = 0;
			std::string infoLog;

			glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
			infoLog.resize(infoLogSize);
			glGetShaderInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);
			std::cout << "Compile status: \n" << &infoLog << std::endl;
		}

		// Mark the two attached shaders for deletion. These two shaders aren't
		// deleted right now because both are already attached to a shader
		// program. When the shader program is deleted these two shaders will
		// be automatically detached and deleted.

		if (vertShader)
			glDeleteShader(vertShader);

		if (fragShader)
			glDeleteShader(fragShader);

	}
	return program;
}

void Shader::cleanup(){
	if (m_program){
		glDeleteProgram(m_program);
		m_program = 0;
	}
}
//////////////////////////////////////////SkyboxShader/////////////////////////////////////
SkyboxShader::SkyboxShader(std::string vertex, std::string fragment, GLuint cubemap) : Shader(vertex, fragment){
	m_cubemap = cubemap;
}

SkyboxShader::~SkyboxShader(){}

void SkyboxShader::bindAttributes(){

	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
}

void SkyboxShader::unbindAttributes(){

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(positionID);
}

////////////////////////////////////////////////////////////////////////////////

EnvironmentMap::EnvironmentMap(std::string vertex, std::string fragment, GLuint cubemap) : Shader(vertex, fragment){	
	m_cubemap = cubemap;
	loadSampler("u_cubeMap", 2);
}

EnvironmentMap::EnvironmentMap(EnvironmentMap* shader) : Shader(shader){	
	m_cubemap = shader->m_cubemap;
}

EnvironmentMap::~EnvironmentMap(){}
///////////////////////////////DepthShader////////////////////////////
DepthShader::DepthShader(std::string vertex, std::string fragment) : Shader(vertex, fragment){}

DepthShader::~DepthShader(){}
//////////////////////////TextureShader/////////////////////////////////////
TextureShader::TextureShader(std::string vertex, std::string fragment) : Shader(vertex, fragment){
	loadSampler("u_texture", 0);
}

TextureShader::~TextureShader(){}

void TextureShader::bindAttributes(GLuint texture){
	
	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnableVertexAttribArray(texCoordID);
	glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void TextureShader::unbindAttributes(){

	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(texCoordID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

}
///////////////////////////////Subsurface////////////////////////////
Subsurfacehader::Subsurfacehader(std::string vertex, std::string fragment) : Shader(vertex, fragment){

}

Subsurfacehader::~Subsurfacehader(){

}




/*void Subsurfacehader::bindAttributes(Mesh *a_mesh, GLuint texture){

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(normalID);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}


void Subsurfacehader::unbindAttributes(Mesh *a_mesh){

	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(normalID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

}*/