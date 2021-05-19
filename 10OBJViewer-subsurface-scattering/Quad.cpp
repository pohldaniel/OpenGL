#include "Quad.h"

Quad::Quad() {
	m_quadShader = new Shader("shader/quad.vert", "shader/quad.frag");
	createBuffer();

	glUseProgram(m_quadShader->m_program);
	glUniform1i(glGetUniformLocation(m_quadShader->m_program, "u_texture"), 0);
	glUseProgram(0);
}

Quad::~Quad() {}

void Quad::createBuffer() {

	m_vertex.push_back(1.0 * m_size); m_vertex.push_back(1.0 * m_size); m_vertex.push_back(0.0 * m_size); m_vertex.push_back(1.0); m_vertex.push_back(1.0);
	m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(1.0 * m_size); m_vertex.push_back(0.0 * m_size); m_vertex.push_back(0.0); m_vertex.push_back(1.0);
	m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(0.0 * m_size); m_vertex.push_back(0.0); m_vertex.push_back(0.0);
	m_vertex.push_back(1.0 * m_size); m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(0.0 * m_size); m_vertex.push_back(1.0); m_vertex.push_back(0.0);

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	//////////////////////////////////////////////setup vertex buffer objects//////////////////////////////////////////////////
	glGenBuffers(1, &m_quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(float), &m_vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//////////////////////////////////////////////setup vertex array objects//////////////////////////////////////////////////
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);	
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexQuad);
	glBindVertexArray(0);
}

void Quad::render(unsigned int texture) {	

	glUseProgram(m_quadShader->m_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}