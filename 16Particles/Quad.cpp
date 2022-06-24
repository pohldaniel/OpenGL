#include "Quad.h"
#include <iostream>

Quad::Quad() {
	createBuffer();
}

Quad::Quad(bool flippable, float leftEdge, float rightEdge, float bottomEdge, float topEdge, float sizeX, float sizeY, float offsetX, float offsetY, float sizeTexX, float sizeTexY, short x, short y) {
	
	m_flippable = flippable;

	if (flippable) {
		createBuffer(m_vao, true, leftEdge, rightEdge, bottomEdge, topEdge, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  x, y);
		//createBuffer(m_vaoFlipped, false, shiftX_1, shiftY_1, shiftX_2, shiftY_2, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  1, 0);
		//createBuffer(m_vao, false, shiftX_1, shiftY_1, shiftX_2, shiftY_2, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  0, 0);
	}else {
		createBuffer(m_vao, false, leftEdge, rightEdge, bottomEdge, topEdge, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  x, y);
	}
}

Quad::Quad(Vector2f& position, Vector2f size) {
	m_position = position;
	m_size = size;
	createBuffer();
}

Quad::~Quad() {
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
	}

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
	}

	if (m_vaoFlipped) {
		glDeleteVertexArrays(1, &m_vaoFlipped);
		m_vaoFlipped = 0;
	}
}

void Quad::createBuffer(unsigned int& vao, bool flippable, float leftEdge, float rightEdge, float bottomEdge, float topEdge, float sizeX, float sizeY, float offsetX, float offsetY, float sizeTexX, float sizeTexY, short x, short y) {
	std::vector<float> vertex;

	if (flippable) {
		vertex.push_back(leftEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
		vertex.push_back(leftEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
	}else {
		
		vertex.push_back(leftEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
		vertex.push_back(leftEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
	}

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	short stride = flippable ? 7 : 5;
	short offset = 3;

	m_scale[0] = (vertex[stride * 2] - vertex[0]) / sizeX;
	m_scale[1] = (vertex[stride + 1] - vertex[1]) / sizeY;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (flippable) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
	}else {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
	}
	//Indices
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::createBuffer() {
	std::vector<float> vertex;

	Vector2f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	
	vertex.push_back(pos[0]); vertex.push_back(pos[1]); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + h); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f);

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	short stride = 5;
	short offset = 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::mapBuffer() {
	float data[] = {
		m_position[0] - m_origin[0], m_position[1] - m_origin[1],						  0.0f, 0.0f, 0.0f,
		m_position[0] - m_origin[0], m_position[1] - m_origin[1] + m_size[1],			  0.0f, 0.0f, 1.0f,
		m_position[0] - m_origin[0] + m_size[0], m_position[1] - m_origin[1] + m_size[1], 0.0f, 1.0f, 1.0f,
		m_position[0] - m_origin[0] + m_size[0], m_position[1] - m_origin[1],			  0.0f, 1.0f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), NULL, GL_STATIC_DRAW);

	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr){
		memcpy(ptr, data, sizeof(data));
		glUnmapBuffer(GL_ARRAY_BUFFER); 
	}
}

void Quad::render(unsigned int texture, bool array) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Quad::render() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Quad::setFlipped(bool flipped) { 
	if (m_flipped == flipped || !m_flippable) return;
	m_flipped = flipped; 

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (m_flippable ? 7 : 5) * sizeof(float), (void*)((m_flipped ? 5 : 3) * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/*void  Quad::render(unsigned int texture, bool array) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
	glBindVertexArray(m_flipped ? m_vaoFlipped : m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Quad::setFlipped(bool flipped) {
	if (m_flipped == flipped) return;
	m_flipped = flipped;
}*/

const Vector2f &Quad::getScale() const {
	return m_scale;
}

void Quad::setPosition(const Vector2f &position) {
	m_position = position;
	mapBuffer();
}

void Quad::setSize(const Vector2f &size) {
	m_size = size;
	mapBuffer();
}

void Quad::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	mapBuffer();
}

const Vector2f &Quad::getPosition() const {
	return m_position;
}

const Vector2f &Quad::getSize() const {
	return m_size;
}

const Vector2f &Quad::getOrigin() const {
	return m_origin;
}