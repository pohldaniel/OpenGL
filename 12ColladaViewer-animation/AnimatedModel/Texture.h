#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "../Extension.h"

class Texture{

public:
	Texture(const std::string& fileName);
	Texture();
	void setTexture(const std::string& fileName);
	virtual ~Texture();

	void Bind(unsigned int unit);

private:
	GLuint _texture;
};

#endif