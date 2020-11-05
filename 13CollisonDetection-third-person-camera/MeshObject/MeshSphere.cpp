#include "MeshSphere.h"

MeshSphere::MeshSphere(float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool  generateNormalDerivatives, const std::string &texture) {

	m_radius = radius;
	m_invRadius = 1.0 / radius;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;

	m_hasTexels = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasNormalDerivatives = false;

	m_isInitialized = false;
	m_uResolution = 49;
	m_vResolution = 49;

	m_numBuffers = 2 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_texture = std::make_shared<Texture>(texture);
	m_shader = std::make_shared<Shader>("shader/texture.vert", "shader/texture.frag");

	m_modelMatrix = ModelMatrix();
}

MeshSphere::MeshSphere(float radius, const std::string &texture) : MeshSphere(radius, true, true, false, false, texture) { }

MeshSphere::~MeshSphere() {}

void MeshSphere::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshSphere::buildMesh() {

	if (m_isInitialized) return;

	std::vector<unsigned int> indexBuffer;
	std::vector<Vector3f> positions;
	std::vector<Vector2f> texels;
	std::vector<Vector3f> normals;
	std::vector<Vector3f> tangents;
	std::vector<Vector3f> bitangents;
	std::vector<Vector3f> normalsDu;
	std::vector<Vector3f> normalsDv;

	float uAngleStep = (2.0f * PI) / float(m_uResolution);
	float vAngleStep = PI / float(m_vResolution);

	float vSegmentAngle;
	for (unsigned int i = 0; i <= m_vResolution; i++) {

		vSegmentAngle = i * vAngleStep;
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= m_uResolution; j++) {

			float uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			// Calculate vertex position on the surface of a sphere
			float x = m_radius * sinVSegment * cosUSegment;
			float z = m_radius * sinVSegment * sinUSegment;
			float y = m_radius * cosVSegment;

			Vector3f surfacePosition = Vector3f(x, y, z);
			positions.push_back(surfacePosition);
		}
	}

	if (m_generateNormals) {

		for (unsigned int i = 0; i <= m_vResolution; i++) {

			// starting from pi/2 to -pi/2
			vSegmentAngle = i * vAngleStep;

			float cosVSegment = cosf(vSegmentAngle);
			float sinVSegment = sinf(vSegmentAngle);

			for (int j = 0; j <= m_uResolution; j++) {

				float uSegmentAngle = j * uAngleStep;

				float cosUSegment = cosf(uSegmentAngle);
				float sinUSegment = sinf(uSegmentAngle);

				// Calculate vertex position on the surface of the sphere
				float n1 = sinVSegment * cosUSegment;
				float n2 = cosVSegment;
				float n3 = sinVSegment * sinUSegment;

				Vector3f normal = Vector3f(n1, n2, n3);
				normals.push_back(normal);

				/*float n1 = cosVSegment * cosVSegment * cosUSegment;
				float n2 = sinVSegment;
				float n3 = cosVSegment * cosVSegment * sinUSegment;

				Vector3f normal = Vector3f(n1 , n2, n3).normalize();
				m_normals.push_back(normal);*/
			}
		}
		m_hasNormals = true;
	}

	if (m_generateTexels) {

		for (unsigned int i = 0; i <= m_vResolution; i++) {

			for (int j = 0; j <= m_uResolution; j++) {

				// Calculate vertex position on the surface of torus
				float u = (float)j / m_uResolution;
				float v = (float)i / m_vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}
		}
		m_hasTexels = true;
	}


	if (m_generateTangents) {

		for (unsigned int i = 0; i <= m_vResolution; i++) {

			vSegmentAngle = i * vAngleStep;

			float cosVSegment = cosf(vSegmentAngle);
			float sinVSegment = sinf(vSegmentAngle);

			for (int j = 0; j <= m_uResolution; j++) {

				float uSegmentAngle = j * uAngleStep;

				float cosUSegment = cosf(uSegmentAngle);
				float sinUSegment = sinf(uSegmentAngle);

				tangents.push_back(Vector3f(-sinUSegment * sinVSegment, 0.0, cosUSegment* sinVSegment).normalize());
				bitangents.push_back(Vector3f(cosVSegment*cosUSegment, -sinVSegment, cosVSegment*sinUSegment).normalize());
			}
		}

		m_hasTangents = true;
	}

	if (m_generateNormalDerivatives) {

		for (unsigned int i = 0; i <= m_vResolution; i++) {

			vSegmentAngle = i * vAngleStep;

			float cosVSegment = cosf(vSegmentAngle);
			float sinVSegment = sinf(vSegmentAngle);

			for (int j = 0; j <= m_uResolution; j++) {

				float uSegmentAngle = j * uAngleStep;

				float cosUSegment = cosf(uSegmentAngle);
				float sinUSegment = sinf(uSegmentAngle);


				/*Vector3f tangent = Vector3f(-sinUSegment * sinVSegment, 0.0, cosUSegment* sinVSegment).normalize();
				Vector3f bitangent = Vector3f(cosVSegment*cosUSegment, -sinVSegment, cosVSegment*sinUSegment).normalize();
				Vector3f normal = Vector3f::cross(tangent, bitangent);

				Vector3f d2Pduu = -Vector3f(sinVSegment * cosUSegment, 0.0, sinVSegment * sinUSegment);
				Vector3f d2Pdvv = -Vector3f(sinVSegment * cosUSegment, cosVSegment, sinVSegment * sinUSegment);
				Vector3f d2Pduv = Vector3f(-cosVSegment * sinUSegment, 0.0, cosVSegment * cosUSegment);

				float E = Vector3f::dot(tangent, tangent);
				float F = Vector3f::dot(tangent, bitangent);
				float G = Vector3f::dot(bitangent, bitangent);

				float e = Vector3f::dot(normal, d2Pduu);
				float f = Vector3f::dot(normal, d2Pduv);
				float g = Vector3f::dot(normal, d2Pdvv);

				float invEGF2 = 1.0 / (E * G - F * F);
				Vector3f dndu = ((f * F - e * G) * invEGF2 * tangent + (e * F - f * E) * invEGF2 * bitangent).normalize();
				Vector3f dndv = ((g * F - f * G) * invEGF2 * tangent + (f * F - g * E) * invEGF2 * bitangent).normalize();

				normalsDu.push_back(dndu);
				normalsDv.push_back(dndv);*/

				float n1u = -sinVSegment * sinUSegment;
				float n3u = sinVSegment * cosUSegment;

				Vector3f dndu = Vector3f(n1u, 0.0, n3u).normalize();
				normalsDu.push_back(dndu);


				float n1v = cosVSegment  * cosUSegment;
				float n2v = -sinVSegment;
				float n3v = cosVSegment  * sinUSegment;

				Vector3f dndv = Vector3f(n1v, n2v, n3v).normalize();
				normalsDv.push_back(dndv);

			}
		}

		m_hasNormalDerivatives = true;
	}

	//calculate the indices
	//north pole
	for (unsigned int j = 0; j < m_uResolution; j++) {

		indexBuffer.push_back(0);
		indexBuffer.push_back((m_uResolution + 1) + j + 1);
		indexBuffer.push_back((m_uResolution + 1) + j);

		//Connect the penultimate vertex to the first
		//if (j == (m_uResolution - 1)) {
		//
		//	m_indexBuffer.push_back(0);			
		//	m_indexBuffer.push_back((m_uResolution + 1));
		//	m_indexBuffer.push_back((m_uResolution + 1) + j);
		//
		//}else {
		//	m_indexBuffer.push_back(0);		
		//	m_indexBuffer.push_back((m_uResolution + 1) + j + 1);
		//	m_indexBuffer.push_back((m_uResolution + 1) + j);
		//}
	}

	for (unsigned int i = 1; i < m_vResolution - 1; i++) {

		int k1 = i * (m_uResolution + 1);
		int k2 = k1 + (m_uResolution + 1);

		for (unsigned int j = 0; j < m_uResolution; j++) {

			indexBuffer.push_back(k1 + j);
			indexBuffer.push_back(k1 + j + 1);
			indexBuffer.push_back(k2 + j);

			indexBuffer.push_back(k1 + j + 1);
			indexBuffer.push_back(k2 + j + 1);
			indexBuffer.push_back(k2 + j);

			//Connect the penultimate vertex to the first
			//if (j == (m_uResolution - 1)) {
			//
			//	m_indexBuffer.push_back(k1 + j);				
			//	m_indexBuffer.push_back(k1);
			//	m_indexBuffer.push_back(k2 + j);
			//
			//	m_indexBuffer.push_back(k1);				
			//	m_indexBuffer.push_back(k2);
			//	m_indexBuffer.push_back(k2 + j);
			//
			//}else {
			//
			//	m_indexBuffer.push_back(k1 + j);
			//	m_indexBuffer.push_back(k1 + j + 1);
			//	m_indexBuffer.push_back(k2 + j);
			//	
			//	m_indexBuffer.push_back(k1 + j + 1);
			//	m_indexBuffer.push_back(k2 + j + 1);
			//	m_indexBuffer.push_back(k2 + j);
			//	
			//}
		}
	}

	//south pole
	for (unsigned int j = 0; j < m_uResolution; j++) {

		indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j);
		indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j + 1);
		indexBuffer.push_back(m_vResolution * (m_uResolution + 1));

		//Connect the penultimate vertex to the first
		//if (j == (m_uResolution - 1)) {
		//	
		//	m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j);
		//	m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1));			
		//	m_indexBuffer.push_back(m_vResolution * (m_uResolution + 1));
		//
		//}else {
		//	
		//	m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j);
		//	m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j + 1);			
		//	m_indexBuffer.push_back(m_vResolution * (m_uResolution + 1));
		//}
	}
	
	m_drawCount = indexBuffer.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(m_numBuffers, m_vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(indexBuffer[0]), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	positions.clear();
	positions.shrink_to_fit();
	texels.clear();
	texels.shrink_to_fit();
	normals.clear();
	normals.shrink_to_fit();
	tangents.clear();
	tangents.shrink_to_fit();
	bitangents.clear();
	bitangents.shrink_to_fit();
	normalsDu.clear();
	normalsDu.shrink_to_fit();
	normalsDv.clear();
	normalsDv.shrink_to_fit();
	indexBuffer.clear();
	indexBuffer.shrink_to_fit();

	m_isInitialized = true;
}


void MeshSphere::draw(const Camera camera, Entity3D entity) {

	glUseProgram(m_shader->m_program);

	m_texture->bind(0);
	m_shader->loadMatrix("u_modelView", entity.getWorldMatrix() * camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix());

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}