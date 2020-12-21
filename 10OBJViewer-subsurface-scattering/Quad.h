#ifndef __quadH__
#define __quadH__

#include <vector>

#include "Shader.h"


class Quad {

public:

	Quad();
	~Quad();

	void render(unsigned int texture);

private:
	void createBuffer();

	unsigned int m_quadVBO, m_indexQuad;
	int m_size = 1;
	std::vector<float> m_vertex;

	TextureShader *m_quadShader;
};


#endif // __quadH__