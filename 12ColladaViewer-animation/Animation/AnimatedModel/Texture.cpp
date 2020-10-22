#include <iostream>
#include <cassert>
#include "..\..\Third\deps\stb_image.h"

#include "Texture.h"

Texture::Texture(const std::string & fileName){

	setTexture(fileName);
}

Texture::Texture(){

	setTexture(".\\res\\default.png");
}

void Texture::setTexture(const std::string & fileName){

	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numCompontents, 4);

	if (imageData == NULL) {
		std::cerr << "Unable to load texture: " << fileName << std::endl;
		return;
	}
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

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
