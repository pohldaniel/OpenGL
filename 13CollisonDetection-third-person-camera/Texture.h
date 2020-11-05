#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "Extension.h"

class Texture{

public:
	Texture(const std::string& fileName);
	
	void setTexture(const std::string& fileName);
	virtual ~Texture();

	void bind(unsigned int unit);


	GLuint m_texture;
};

#endif