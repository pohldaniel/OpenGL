#include "Shader.h"

Shader::Shader(std::string vertex, std::string fragment){
	
	m_program = createProgram(vertex, fragment);

	getAttributeGetAttribLocation();
	loadSampler();
}

Shader::Shader(std::string vertex, std::string fragment, std::string geometry) {

	m_program = createProgram(vertex, fragment, geometry);
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


void Shader::bindAttributes(Mesh *a_mesh, GLuint texture){
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, normal);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, depth);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, irradiance);

	if (a_mesh->m_hasTangents){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(
			positionID,					// attribute
			3,						     // size
			GL_FLOAT,					 // type
			GL_FALSE,					 // normalized?
			14 * sizeof(float),			 // stride
			(void*)0      // array buffer offset
			);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getNormalMap());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getDisplacementMap());

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(
			texCoordID,                       // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			14 * sizeof(float),	              // stride
			(void*)(3 * sizeof(float))        // array buffer offset
			);

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(
			normalID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			14 * sizeof(float),			// stride
			(void*)(5 * sizeof(float))	// array buffer offset
			);


		glEnableVertexAttribArray(tangentID);
		glVertexAttribPointer(
			tangentID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			14 * sizeof(float),			// stride
			(void*)(8 * sizeof(float))	// array buffer offset
			);


		glEnableVertexAttribArray(bitangentID);
		glVertexAttribPointer(
			bitangentID,				// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			14 * sizeof(float),			// stride
			(void*)(11 * sizeof(float))	// array buffer offset
			);

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){


		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(
			positionID,					// attribute
			3,						     // size
			GL_FLOAT,					 // type
			GL_FALSE,					 // normalized?
			8 * sizeof(float),			 // stride
			(void*)0      // array buffer offset
			);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(
			texCoordID,                       // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			8 * sizeof(float),	              // stride
			(void*)(3 * sizeof(float))        // array buffer offset
			);

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(
			normalID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			8 * sizeof(float),			// stride
			(void*)(5 * sizeof(float))	// array buffer offset
			);

	}else if (a_mesh->m_hasNormals) { 
		
		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(
			positionID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			6 * sizeof(float),			// stride
			(void*)0					// array buffer offset
			);

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(
			normalID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			6 * sizeof(float),			// stride
			(void*)(3 * sizeof(float))	// array buffer offset
			);

	}else if (a_mesh->m_hasTextureCoords){


		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(
			positionID,					// attribute
			3,						    // size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			5 * sizeof(float),			// stride
			(void*)0					// array buffer offset
			);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(
			texCoordID,                       // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			5 * sizeof(float),	              // stride
			(void*)(3 * sizeof(float))        // array buffer offset
			);
	}else{

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(
			positionID,					// attribute
			3,						     // size
			GL_FLOAT,					 // type
			GL_FALSE,					 // normalized?
			3 * sizeof(float),			 // stride
			(void*)0      // array buffer offset
			);
	}
}

void Shader::unbindAttributes(Mesh *a_mesh){

	if (a_mesh->m_hasTangents){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(tangentID);
		glDisableVertexAttribArray(bitangentID);
		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}else if (a_mesh->m_hasTextureCoords){

		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}else if (a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);

	}
	
	glDisableVertexAttribArray(positionID);	
}


void Shader::loadSampler(const char* location, int sampler){

	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, location), sampler);
	glUseProgram(0);
}

//OpenGL specifies matrices as column-major to get row-major just transpose it

void Shader::loadMatrix(const char* location, const Matrix4f matrix, bool transpose){

	glUniformMatrix4fv(glGetUniformLocation(m_program, location), 1, transpose, &matrix[0][0]);

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

void Shader::loadLightSources(std::vector<LightSource> lights){
	
	for (int i = 0; i < lights.size(); i++){

		loadLightSource(lights[i], i);

	}
	
}

void Shader::loadMaterial(const Mesh::Material material){
	
	

	glUseProgram(m_program);
	glUniform3fv(glGetUniformLocation(m_program, "material.ambient"), 1, &material.ambient[0]);
	glUniform3fv(glGetUniformLocation(m_program, "material.diffuse"), 1, &material.diffuse[0]);
	glUniform3fv(glGetUniformLocation(m_program, "material.specular"), 1, &material.specular[0]);
	glUniform1f(glGetUniformLocation(m_program, "material.specularShininesse"), material.shinies);
	glUseProgram(0);
}

void Shader::readTextFile(const char *pszFilename, std::string &buffer){

	std::ifstream file(pszFilename, std::ios::binary);

	if (file.is_open()){
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

	if (buffer.length() > 0){
		shader = compileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}

	return shader;
}

GLuint Shader::compileShader(GLenum type, const char *pszSource){

	GLuint shader = glCreateShader(type);

	if (shader){
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

GLuint Shader::createProgram(std::string vertex, std::string fragment) {


	GLuint vshader = loadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = loadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());

	return linkShaders(vshader, fshader);

}

GLuint Shader::createProgram(std::string vertex, std::string fragment, std::string geometry) {

	GLuint vshader = loadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = loadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());
	GLuint gshader = loadShaderProgram(GL_GEOMETRY_SHADER, geometry.c_str());

	return linkShaders(vshader, fshader, gshader);
}


GLuint Shader::linkShaders(GLuint vertShader, GLuint fragShader){

	GLuint program = glCreateProgram();

	if (program)
	{
		GLint linked = 0;

		if (vertShader)
			glAttachShader(program, vertShader);

		if (fragShader)
			glAttachShader(program, fragShader);

		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked)
		{
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

GLuint Shader::linkShaders(GLuint vertShader, GLuint fragShader, GLuint geoShader) {
	GLuint program = glCreateProgram();

	if (program){

		GLint linked = 0;

		if (vertShader)
			glAttachShader(program, vertShader);

		if (fragShader)
			glAttachShader(program, fragShader);

		if (geoShader)
			glAttachShader(program, geoShader);

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

		if (geoShader)
			glDeleteShader(geoShader);

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

SkyboxShader::~SkyboxShader(){

}


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

EnvironmentMap::~EnvironmentMap(){

}


void EnvironmentMap::bindAttributes(Mesh *a_mesh, GLuint texture){
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (a_mesh->m_hasTangents){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)0 );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getNormalMap());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(3*sizeof(float)) );

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID,	3, GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(5*sizeof(float)));

		glEnableVertexAttribArray(tangentID);
		glVertexAttribPointer(tangentID, 3,	GL_FLOAT, GL_FALSE, 14*sizeof(float), (void*)(8*sizeof(float)));

		glEnableVertexAttribArray(bitangentID);
		glVertexAttribPointer(bitangentID, 3, GL_FLOAT,	GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),	(void*)(5*sizeof(float)));

	}else if (a_mesh->m_hasNormals) {

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

	}else if (a_mesh->m_hasTextureCoords){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),	(void*)0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, a_mesh->getTextureName());

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),	(void*)(3*sizeof(float)));

	}else{

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),	(void*)0);
	}
}

void EnvironmentMap::unbindAttributes(Mesh *a_mesh){

	if (a_mesh->m_hasTangents){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(tangentID);
		glDisableVertexAttribArray(bitangentID);
		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}else if (a_mesh->m_hasTextureCoords){

		glDisableVertexAttribArray(texCoordID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}else if (a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	glDisableVertexAttribArray(positionID);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////DepthShader////////////////////////////

DepthShader::DepthShader(std::string vertex, std::string fragment) : Shader(vertex, fragment){
}

DepthShader::~DepthShader(){

}

void DepthShader::bindAttributes(Mesh *a_mesh, GLuint texture){
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (a_mesh->m_hasTangents){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));

		glEnableVertexAttribArray(tangentID);
		glVertexAttribPointer(tangentID, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));

		glEnableVertexAttribArray(bitangentID);
		glVertexAttribPointer(bitangentID, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	}else if (a_mesh->m_hasNormals) {

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	}else if (a_mesh->m_hasTextureCoords){

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(texCoordID);
		glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	}else{

		glEnableVertexAttribArray(positionID);
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
}

void DepthShader::unbindAttributes(Mesh *a_mesh){


	if (a_mesh->m_hasTangents){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(tangentID);
		glDisableVertexAttribArray(bitangentID);
		glDisableVertexAttribArray(texCoordID);

	}else if (a_mesh->m_hasTextureCoords && a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);
		glDisableVertexAttribArray(texCoordID);

	}else if (a_mesh->m_hasTextureCoords){

		glDisableVertexAttribArray(texCoordID);

	}else if (a_mesh->m_hasNormals){

		glDisableVertexAttribArray(normalID);
	}

	glDisableVertexAttribArray(positionID);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

}

//////////////////////////TextureShader/////////////////////////////////////


TextureShader::TextureShader(std::string vertex, std::string fragment) : Shader(vertex, fragment){

	loadSampler("u_texture", 0);
}

TextureShader::~TextureShader(){

}


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