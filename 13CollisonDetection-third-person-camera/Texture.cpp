#include <iostream>
#include <cassert>
#include "Third\deps\stb_image.h"

#include "Texture.h"

Texture::Texture(const std::string & fileName){
	setTexture(fileName);
}


void Texture::setTexture(const std::string & fileName){

	int g_maxAnisotrophy;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotrophy);
	//std::cout << fileName.c_str() << std::endl;
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numCompontents, 4);

	if (imageData == NULL) {
		//std::cout << "----------------" << std::endl;
		std::cout << "Unable to load texture: " << fileName << std::endl;
		return;
	}
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

	stbi_image_free(imageData);
}

Texture::~Texture(){
	glDeleteTextures(1, &m_texture);
}

void Texture::bind(unsigned int unit){
	assert(unit >= 0 && unit <= 31);		
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
