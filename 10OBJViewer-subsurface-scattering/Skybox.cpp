#include "Skybox.h"

SkyBox::SkyBox(const char* filename, int size, bool flipVertical, bool flipHorizontal, const Vector3f &position){
	m_size = size;
	createCubeMap(filename, flipVertical, flipHorizontal);
	createBuffer(position);
	m_shader = std::make_shared<Shader>("shader/skybox.vert", "shader/skybox.frag");
}

void SkyBox::setProjectionMatrix(const Matrix4f &projection) {
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_projection", projection);
	glUseProgram(0);
}

SkyBox::~SkyBox() {}

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


void SkyBox::createBuffer(const Vector3f &position) {

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

	//////////////////////////////////////////////setup vertex buffer objects//////////////////////////////////////////////////
	glGenBuffers(1, &m_skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &m_postions[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////setup vertex array objects//////////////////////////////////////////////////
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void SkyBox::draw(const Camera camera) {

	glDepthFunc(GL_LEQUAL);
	glUseProgram(m_shader->m_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

	m_shader->loadMatrix("u_modelView", camera.getViewMatrix());

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glUseProgram(0);

	glDepthFunc(GL_LESS);
}

void SkyBox::FlipVertical(unsigned int width, unsigned int height, unsigned short channels, unsigned char(*&image)) {
	int padWidth = channels * width;
	while (padWidth % 4 != 0) {
		padWidth++;
	}

	unsigned char *pFront = 0;
	unsigned char *pBack = 0;
	unsigned char pixel[3] = { 0 };

	for (int i = 0; i < height; ++i) {

		pFront = &image[i * padWidth];
		pBack = &pFront[padWidth - 3];

		while (pFront < pBack) {

			// Save current pixel at position pFront.
			pixel[0] = pFront[0];
			pixel[1] = pFront[1];
			pixel[2] = pFront[2];

			// Copy new pixel from position pBack into pFront.
			pFront[0] = pBack[0];
			pFront[1] = pBack[1];
			pFront[2] = pBack[2];

			// Copy old pixel at position pFront into pBack.
			pBack[0] = pixel[0];
			pBack[1] = pixel[1];
			pBack[2] = pixel[2];

			pFront += 3;
			pBack -= 3;
		}
	}
}

void SkyBox::FlipHorizontal(unsigned int width, unsigned int height, unsigned short channels, unsigned char(*&image)) {

	int padWidth = channels * width;
	while (padWidth % 4 != 0) {
		padWidth++;
	}
	unsigned char* srcPixels = new unsigned char[padWidth * height];
	memcpy(&srcPixels[0], image, padWidth * height);

	unsigned char *pSrcRow = 0;
	unsigned char *pDestRow = 0;

	for (int i = 0; i < height; ++i) {
		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &image[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
	delete[] srcPixels;
}