#include "MeshSpiral.h"

MeshSpiral::MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture) {

	m_radius = radius;
	m_tubeRadius = tubeRadius;
	m_numRotations = numRotations;
	m_length = length;
	m_repeatTexture = repeatTexture;
	m_position = position;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;

	m_hasTexels = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasNormalDerivatives = false;

	m_isInitialized = false;
	m_mainSegments = 49;
	m_tubeSegments = 49;

	m_numBuffers = 2 + generateTexels + generateNormals + 2 * generateTangents + 2 * generateNormalDerivatives;

	m_texture = std::make_shared<Texture>(texture);
	m_shader = std::make_shared<Shader>("shader/textureN.vert", "shader/textureN.frag");

	m_modelMatrix = ModelMatrix();
}

MeshSpiral::MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, const std::string &texture) : MeshSpiral(position, radius, tubeRadius, numRotations, length, false, true, true, false, false, texture) {}

MeshSpiral::MeshSpiral(float radius, float tubeRadius, int numRotations, float length, const std::string &texture) : MeshSpiral(Vector3f(0.0f, 0.0f, 0.0f), radius, tubeRadius, numRotations, length, false, true, true, false, false, texture) { }

MeshSpiral::~MeshSpiral() {}

void MeshSpiral::setPrecision(int uResolution, int vResolution) {

	m_mainSegments = uResolution;
	m_tubeSegments = vResolution;
}

void MeshSpiral::buildMesh() {

	if (m_isInitialized) return;

	
	std::vector<Vector3f> tangents;
	std::vector<Vector3f> bitangents;
	
	float mainSegmentAngleStep = (2.0f * PI) / float(m_mainSegments);
	float tubeSegmentAngleStep = (2.0f * PI) / float(m_tubeSegments);
	float pitch = m_length / m_numRotations;

	float currentMainSegmentAngle = 0.0f;

	for (unsigned int i = 0; i <= m_mainSegments * m_numRotations; i++) {

		// Calculate sine and cosine of main segment angle
		float sinMainSegment = sinf(currentMainSegmentAngle);
		float cosMainSegment = cosf(currentMainSegmentAngle);

		float currentTubeSegmentAngle = 0.0f;

		if (i > 0 && (m_mainSegments + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

		for (unsigned int j = 0; j <= m_tubeSegments; j++) {

			// Calculate sine and cosine of tube segment angle
			float sinTubeSegment = sinf(currentTubeSegmentAngle);
			float cosTubeSegment = cosf(currentTubeSegmentAngle);

			//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius

			//	[x] = [(R + r cos(u)) cos(v))
			//	[y] = [h * v/(2PI) + rsin(u)]
			//	[z] = [(R + r cos(u)) sin(v)]
			float x = (m_radius + m_tubeRadius * cosTubeSegment)*cosMainSegment;
			float z = (m_radius + m_tubeRadius * cosTubeSegment)*sinMainSegment;
			float y = pitch * (currentMainSegmentAngle * invTWO_PI) + m_tubeRadius * sinTubeSegment;

			Vector3f surfacePosition = Vector3f(x, y, z) + m_position;
			m_positions.push_back(surfacePosition);

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	if (m_generateNormals) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= m_mainSegments * m_numRotations; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			if (i > 0 && (m_mainSegments + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= m_tubeSegments; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/du x dp/dv

				//	[n1] = [(Rr + r^2 cos(u)) cos(v) cos(u) + r sin(u)sin(v)h/(2PI) )
				//	[n2] = [(Rr + r^2 cos(u)) sin(u)]
				//	[n3] = [(Rr + r^2 cos(u)) sin(v) cos(u) - r sin(u)cos(v)h/(2PI)]

				float tmp = m_radius * m_tubeRadius + m_tubeRadius * m_tubeRadius * cosTubeSegment;
				float n1 = tmp * cosMainSegment * cosTubeSegment + m_tubeRadius * pitch * invTWO_PI * sinMainSegment * sinTubeSegment;
				float n2 = tmp * sinTubeSegment;
				float n3 = tmp * sinMainSegment * cosTubeSegment - m_tubeRadius * pitch * invTWO_PI * cosMainSegment * sinTubeSegment;

				Vector3f normal = Vector3f(n1, n2, n3).normalize();
				m_normals.push_back(normal);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
		m_hasNormals = true;
	}

	if (m_generateTexels && m_numRotations > 0) {

		int foo = (m_repeatTexture) ? m_mainSegments : m_mainSegments * m_numRotations;
		int offset = (m_repeatTexture) ? m_numRotations - 1 : 0;

		float mainSegmentTextureStep = 1.0 / (float(foo));
		float tubeSegmentTextureStep = 1.0f / float(m_tubeSegments);

		//rotate the texture to like the meshTorus
		float currentMainSegmentTexCoordU = 0.0;

		for (unsigned int i = 0; i <= m_mainSegments * m_numRotations + offset; i++) {

			if (m_repeatTexture && i > 0 && i % (m_mainSegments + 1) == 0) {

				currentMainSegmentTexCoordU = 0.0;
			}
			//rotate the texture to like the meshTorus
			float currentTubeSegmentTexCoordV = 0.0 - 0.5f;
			for (unsigned int j = 0; j <= m_tubeSegments; j++) {

				Vector2f textureCoordinate = Vector2f((1.0 - currentMainSegmentTexCoordU), currentTubeSegmentTexCoordV);
				m_texels.push_back(textureCoordinate);

				currentTubeSegmentTexCoordV += tubeSegmentTextureStep;
			}
			// Update texture coordinate of main segment
			currentMainSegmentTexCoordU += mainSegmentTextureStep;
		}
		m_hasTexels = true;
	}



	if (m_generateTangents) {


		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= m_mainSegments * m_numRotations; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			if (i > 0 && (m_mainSegments + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= m_tubeSegments; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/du 

				//	[t1] = [-rsin(u)cos(v) )
				//	[t2] = [rcos(u)]
				//	[t3] = [-rsin(u)sins(v)]
				Vector3f tangent = Vector3f(-sinTubeSegment*cosMainSegment, cosTubeSegment, -sinTubeSegment*sinMainSegment);
				tangents.push_back(tangent);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/dv 

				//	[bt1] = [-(R + rcos(u))sin(v)]
				//	[bt2] = [h/(2PI)]
				//	[bt3] = [(R + rcos(u))cos(v)]

				float bt1 = -(m_radius + m_tubeRadius*cosTubeSegment)*sinMainSegment;
				float bt2 = pitch * invTWO_PI;
				float bt3 = (m_radius + m_tubeRadius*cosTubeSegment)*cosMainSegment;

				Vector3f bitangent = Vector3f(bt1, bt2, bt3).normalize();
				bitangents.push_back(bitangent);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}

			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}

		m_hasTangents = true;
	}

	//calculate the indices
	unsigned int currentVertexOffset = 0;
	for (unsigned int i = 0; i < m_mainSegments * m_numRotations; i++) {

		for (unsigned int j = 0; j < m_tubeSegments; j++) {

			unsigned int vertexIndexA, vertexIndexB, vertexIndexC, vertexIndexD, vertexIndexE, vertexIndexF;

			if ((j > 0) && ((j + 1) % (m_tubeSegments + 1)) == 0) {
				currentVertexOffset = ((i + 1) * (m_tubeSegments + 1));
			}else {

				vertexIndexA = currentVertexOffset;
				vertexIndexB = currentVertexOffset + m_tubeSegments + 1;
				vertexIndexC = currentVertexOffset + 1;

				vertexIndexD = currentVertexOffset + m_tubeSegments + 1;
				vertexIndexF = currentVertexOffset + m_tubeSegments + 2;
				vertexIndexE = currentVertexOffset + 1;

				m_indexBuffer.push_back(vertexIndexA); m_indexBuffer.push_back(vertexIndexB); m_indexBuffer.push_back(vertexIndexC);
				m_indexBuffer.push_back(vertexIndexD); m_indexBuffer.push_back(vertexIndexF); m_indexBuffer.push_back(vertexIndexE);
				currentVertexOffset++;
			}
		}
		currentVertexOffset++;
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
	glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	/*m_positions.clear();
	m_positions.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();*/

	tangents.clear();
	tangents.shrink_to_fit();
	bitangents.clear();
	bitangents.shrink_to_fit();
	
	m_isInitialized = true;
}


void MeshSpiral::draw(const Camera camera) {
	glDisable(GL_CULL_FACE);
	glUseProgram(m_shader->m_program);

	m_texture->bind(0);
	m_shader->loadMatrix("u_modelView", camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix());

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	glEnable(GL_CULL_FACE);
}