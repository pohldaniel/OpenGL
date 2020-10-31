#include "Skybox.h"

SkyBox::SkyBox(const char* filename, int size, bool flipVertical, bool flipHorizontal, const Vector3f &position){
	m_size = size;
	createCubeMap(filename, flipVertical, flipHorizontal);
	createBuffer(position);

	m_shader = new SkyboxShader("shader/skybox.vert", "shader/skybox.frag", m_cubemap);
	m_shader->loadSampler("u_skybox", 0);	
}

void SkyBox::setProjectionMatrix(const Matrix4f &projection) {
	glUseProgram(m_shader->m_program);
		m_shader->loadMatrix("u_projection", projection);
	glUseProgram(0);
}

SkyBox::~SkyBox(){

	if (m_shader){
		delete m_shader;
		m_shader = 0;
	}
}


void SkyBox::createCubeMap(const char* filename, bool flipVertical, bool flipHorizontal){

	std::string pattern(filename);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;

	glGenTextures(1, &m_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

	int i = 0;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {

			if (std::string(data.cFileName) == "." || std::string(data.cFileName) == "..") continue;

			loadSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (std::string(filename) + "/" + std::string(data.cFileName)).c_str(), flipVertical, flipHorizontal);
				
			i++;
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}


	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}


void SkyBox::loadSide(unsigned int side_target, const char* file_name, bool flipVertical, bool flipHorizontal){


	Bitmap *bitmap = new Bitmap();
	if (bitmap->loadBitmap24(file_name)){

		if (flipVertical){
			bitmap->flipVertical();
		}

		if (flipHorizontal){
			bitmap->flipHorizontal();
		}

		glTexImage2D(side_target, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

	}else{

		std::cout << "Cubemap texture failed to load at path: " << file_name << std::endl;
		
	}
	delete bitmap;
}


void SkyBox::createBuffer(const Vector3f &position){

	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);

	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);

	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);

	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);

	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);

	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((-1.0 + position[2]) * m_size);
	m_postions.push_back((-1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);
	m_postions.push_back((1.0 + position[0]) * m_size); m_postions.push_back((-1.0 + position[1]) * m_size); m_postions.push_back((1.0 + position[2]) * m_size);

	glGenBuffers(1, &m_skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 *sizeof(float), &m_postions[0], GL_STATIC_DRAW);
}

void SkyBox::render(const Camera camera){

	glDepthFunc(GL_LEQUAL);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", camera.getViewMatrix());
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	m_shader->bindAttributes();

	glDrawArrays(GL_TRIANGLES, 0, 36);
	m_shader->unbindAttributes();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glDepthFunc(GL_LESS);
}