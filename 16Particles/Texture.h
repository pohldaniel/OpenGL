#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include "Extension.h"

class Texture {

public:
	Texture(const std::string& fileName);

	void setTexture(const std::string& fileName);
	virtual ~Texture();

	void bind(unsigned int unit);


	GLuint m_texture;

private:
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};

#endif