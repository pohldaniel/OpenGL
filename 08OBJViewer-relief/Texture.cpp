#include <iostream>
#include <cassert>

#include "Texture.h"

Texture::Texture(const std::string & fileName) {
	setTexture(fileName);
}

void Texture::setTexture(const std::string & fileName) {

	int g_maxAnisotrophy;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotrophy);
	int width, height, numCompontents;

	Bitmap* bitmap = new Bitmap();
	if (bitmap->loadBitmap24(fileName.c_str())) {

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (g_maxAnisotrophy > 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_maxAnisotrophy);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}else {
		std::cout << "Unable to load texture: " << fileName << std::endl;
		return;

	}
	delete bitmap;
	bitmap = NULL;
}

Texture::~Texture() {
	glDeleteTextures(1, &m_texture);
}

void Texture::bind(unsigned int unit) {
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::bind(unsigned int unit, unsigned int texture) {
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}