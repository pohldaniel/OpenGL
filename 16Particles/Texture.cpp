#include <iostream>
#include <cassert>
#include "soil2\SOIL2.h"

#include "Texture.h"

Texture::Texture(const std::string & fileName) {
	setTexture(fileName);
}


void Texture::setTexture(const std::string & fileName) {

	int g_maxAnisotrophy;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotrophy);
	//std::cout << fileName.c_str() << std::endl;
	int width, height, numCompontents;

	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	if (imageData == NULL) {
		//std::cout << "----------------" << std::endl;
		std::cout << "Unable to load texture: " << fileName << std::endl;
		return;
	}

	
	//flip vertically
	flipVertical(imageData, numCompontents * width, height);
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	//std::cout << "----------------" << std::endl;
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (g_maxAnisotrophy > 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_maxAnisotrophy);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_texture);
}

void Texture::flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<BYTE> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	BYTE *pSrcRow = 0;
	BYTE *pDestRow = 0;

	for (int i = 0; i < height; ++i) {

		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &data[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
}

void Texture::bind(unsigned int unit) {
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
