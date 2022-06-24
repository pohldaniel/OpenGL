#ifndef __quadH__
#define __quadH__

#include <vector>
#include "Shader.h"

class Quad {

public:
	Quad();
	Quad(Vector2f& position, Vector2f size);
	Quad(bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f,  short x = 0, short y = 0);
	~Quad();

	void render(unsigned int texture, bool array = false);
	void render();
	void setFlipped(bool flipped);

	const Vector2f &getScale() const;
	void setPosition(const Vector2f &position);
	void setSize(const Vector2f &size);
	void setOrigin(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector2f &getOrigin() const;

private:

	void createBuffer(unsigned int& vao, bool flippable, float leftEdge = -1.0f, float rightEdge = 1.0f, float bottomEdge = -1.0f, float topEdge = 1.0f, float sizeX = 1.0f, float sizeY = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f, float sizeTexX = 1.0f, float sizeTexY = 1.0f, short x = 0, short y = 0);
	void createBuffer();
	void mapBuffer();

	unsigned int m_vao = 0;
	unsigned int m_vaoFlipped = 0;
	unsigned int m_vbo = 0;

	bool m_flippable = false;
	bool m_flipped = false;

	Vector2f m_scale;
	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;
};
#endif // __quadH__