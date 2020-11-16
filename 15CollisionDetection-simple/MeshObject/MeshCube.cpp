#include "MeshCube.h"

MeshCube::MeshCube(const Vector3f &position, float width, float height, float depth, bool generateTexels, const std::string &texture) {

	m_numBuffers = 4;

	m_width = width;
	m_height = height;
	m_depth = depth;
	m_position = position;

	m_generateTexels = generateTexels;

	m_hasTexels = false;

	m_uResolution = 49;
	m_vResolution = 49;

	m_texture = std::make_shared<Texture>(texture);
	m_shader = std::make_shared<Shader>("shader/texture.vert", "shader/texture.frag");

	m_modelMatrix = ModelMatrix();
}

MeshCube::MeshCube(const Vector3f &position, float width, float height, float depth, const std::string &texture) : MeshCube(position, width, height, depth, true, texture) {}

MeshCube::MeshCube(float width, float height, float depth, const std::string &texture) : MeshCube(Vector3f(0.0f, 0.0f, 0.0f), width, height, depth, true, texture) {}

MeshCube::~MeshCube() {}

void MeshCube::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshCube::buildMesh() {

	std::vector<Vector2f> texels;
	std::vector<Vector3f> normals;

	float vStep = (1.0f / m_vResolution) * m_height;
	float uStep = (1.0f / m_uResolution) * m_width;

	//front
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j < m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;
				Vector2f textureCoordinate = Vector2f(u, 1 - v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_depth;

	//right
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = m_uResolution; j > 0; j--) {

			// Calculate vertex position on the surface of a quad
			float x = m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = j * uStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;				
				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_width;

	//back
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = m_uResolution; j > 0; j--) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = -m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);
			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
				m_hasTexels = true;
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_depth;

	//left
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j < m_uResolution; j++) {
			// Calculate vertex position on the surface of a quad
			float x = -m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = j * uStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}
	
	vStep = (1.0f / m_vResolution) * m_depth;
	uStep = (1.0f / m_uResolution) * m_width;

	//bottom
	for (unsigned int i = 0; i <= m_vResolution ; i++) {
		for (unsigned int j = 0; j <= m_uResolution ; j++) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = -m_height * 0.5f;
			float z = i * vStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u,v);			
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_depth;
	uStep = (1.0f / m_uResolution) * m_width;

	//top
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = m_height * 0.5f;
			float z = i * vStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	int face = 0;
	for (unsigned int v = 0; v < m_vResolution; v++) {

		for (unsigned int u = 0; u < 4 * m_uResolution * (m_vResolution + 1); u++) {

			//Connect the penultimate vertex to the first
			if (u == ((m_uResolution - 1) + face * (m_uResolution * (1 + m_vResolution)))) {
				face++;

				m_indexBuffer.push_back(u + (1 + v) * m_uResolution);
				m_indexBuffer.push_back(u + v * (m_uResolution));
				m_indexBuffer.push_back((m_uResolution * (v + 1 + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));

				m_indexBuffer.push_back(u + v * (m_uResolution));
				m_indexBuffer.push_back((m_uResolution * (v + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));
				m_indexBuffer.push_back((m_uResolution * (v + 1 + m_vResolution) + u + 1) % (4 * m_uResolution * (m_vResolution + 1)));

			}
			else if (v == 0 && u < m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) <= u && u < 2 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) * 2 <= u && u < 3 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);

			}
			else if (v == 0 && m_uResolution * (m_vResolution + 1) * 3 <= u && u < 4 * m_uResolution * (m_vResolution + 1) - m_uResolution) {

				if ((u + 1) % (m_uResolution) == 0) continue;

				m_indexBuffer.push_back(u);
				m_indexBuffer.push_back(u + 1);
				m_indexBuffer.push_back(u + m_uResolution + 1);

				m_indexBuffer.push_back(u + m_uResolution + 1);
				m_indexBuffer.push_back(u + m_uResolution);
				m_indexBuffer.push_back(u);
			}
		}
		face = 0;
	}

	for (unsigned short face = 0; face < 2; face++) {

		int k = (4 + face) * m_uResolution * (m_vResolution + 1) + face * (m_uResolution + 1);
		for (int i = k; i < k + m_vResolution; i++) {
			for (int j = k; j < k + m_uResolution; j++) {
				if (face % 2 == 0) {
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);

					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j + 1);
				}
				else {
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);

					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j);
					m_indexBuffer.push_back((i - k + 1) * (m_uResolution + 1) + j + 1);
					m_indexBuffer.push_back((i - k) * (m_uResolution + 1) + j + 1);
				}
			}
		}
	}

	m_drawCount = m_indexBuffer.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(m_numBuffers, m_vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(texels[0]), &texels[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	/*positions.clear();
	positions.shrink_to_fit();*/
	texels.clear();
	texels.shrink_to_fit();
	normals.clear();
	normals.shrink_to_fit();
	/*indexBuffer.clear();
	indexBuffer.shrink_to_fit();*/

	m_isInitialized = true;
}

void MeshCube::buildMesh4Q() {

	std::vector<Vector2f> texels;
	std::vector<Vector3f> normals;

	float vStep = (1.0f / m_vResolution) * m_height;
	float uStep = (1.0f / m_uResolution) * m_width;
	//front
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_depth;

	//right
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = j * uStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 -u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_depth;

	//left
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = -m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = j * uStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_height;
	uStep = (1.0f / m_uResolution) * m_width;
	//back
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = i * vStep - m_height * 0.5f;
			float z = -m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
				m_hasTexels = true;
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_depth;
	uStep = (1.0f / m_uResolution) * m_width;
	//bottom
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = -m_height * 0.5f;
			float z = i * vStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	vStep = (1.0f / m_vResolution) * m_depth;
	uStep = (1.0f / m_uResolution) * m_width;
	//top
	for (unsigned int i = 0; i <= m_vResolution; i++) {
		for (unsigned int j = 0; j <= m_uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep - m_width * 0.5f;
			float y = m_height * 0.5f;
			float z = i * vStep - m_depth * 0.5f;

			Vector3f position = Vector3f(x, y, z) + m_position;
			m_positions.push_back(position);

			if (m_generateTexels) {
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
	}

	//calculate the indices
	for (short face = 0; face < 6; face++) {
		int k = (m_vResolution + 1) * (m_uResolution + 1) * face;
		for (int v = 0; v < m_vResolution; v++) {
			for (int u = 0; u < m_uResolution; u++) {

				if (face % 2 == 0) {
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));

					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u));
				}else {
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u + 1));
					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));

					m_indexBuffer.push_back(k + (v * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u));
					m_indexBuffer.push_back(k + ((v + 1) * (m_uResolution + 1) + u + 1));				
				}
			}
		}
	}

	m_drawCount = m_indexBuffer.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(m_numBuffers, m_vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(texels[0]), &texels[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	/*positions.clear();
	positions.shrink_to_fit();*/
	texels.clear();
	texels.shrink_to_fit();
	normals.clear();
	normals.shrink_to_fit();
	/*indexBuffer.clear();
	indexBuffer.shrink_to_fit();*/

	m_isInitialized = true;
}

void MeshCube::draw(const Camera camera) {
	glUseProgram(m_shader->m_program);

	m_texture->bind(0);
	m_shader->loadMatrix("u_modelView", camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix());

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}