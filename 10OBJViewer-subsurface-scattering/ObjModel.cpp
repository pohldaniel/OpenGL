#include "ObjModel.h"

Model::Model() : m_size(0), m_stride(0), m_offset(0), m_numberOfBytes(0) {
	m_numberOfMeshes = 0;
	m_numberOfTriangles = 0;
	m_mltPath = "";
	m_modelDirectory = "";
	m_hasMaterial = false;
	m_center;
	m_modelMatrix = new ModelMatrix();

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
}

Model::~Model() {
	m_vertexBuffer.clear();
	m_indexBuffer.clear();
	m_vertexCache.clear();
}

void Model::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	m_modelMatrix->setRotPos(axis, degrees, dx, dy, dz);
}

void Model::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz) {

	m_modelMatrix->setRotXYZPos(axisX, degreesX,
		axisY, degreesY,
		axisZ, degreesZ,
		dx, dy, dz);
}

void Model::rotate(const Vector3f &axis, float degrees) {
	m_modelMatrix->rotate(axis, degrees);
}

void Model::translate(float dx, float dy, float dz) {
	m_modelMatrix->translate(dx, dy, dz);
}

void Model::scale(float a, float b, float c) {
	m_modelMatrix->scale(a, b, c);
}

const Matrix4f &Model::getTransformationMatrix() const {
	return m_modelMatrix->getTransformationMatrix();
}

const Matrix4f &Model::getInvTransformationMatrix() const {
	return m_modelMatrix->getInvTransformationMatrix();
}

const Vector3f &Model::getCenter() const {
	return m_center;
}

std::string Model::getMltPath() {
	return m_mltPath;
}

std::string Model::getModelDirectory() {
	return m_modelDirectory;
}

bool Model::loadObject(const char* filename) {
	return loadObject(filename, Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs) {
	return i_lhs[9] < i_rhs[9];
}

bool Model::loadObject(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale) {

	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	std::vector<std::string*>coord;
	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;

	std::ifstream in(a_filename);

	std::map<std::string, int> name;


	int countMesh = 0;
	int assign = 0;
	char buffer[250];

	if (!in.is_open()) {

		std::cout << "File not found" << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)) {
		coord.push_back(new std::string(line));

	}
	in.close();

	for (int i = 0; i < coord.size(); i++) {

		if ((*coord[i])[0] == '#') {

			continue;

		}else if ((*coord[i])[0] == 'm') {

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);
			m_mltPath = buffer;

			m_hasMaterial = true;

		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == ' ') {

			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);

			vertexCoords.push_back((translate.getVec()[0] + tmpx) * scale);
			vertexCoords.push_back((translate.getVec()[1] + tmpy) * scale);
			vertexCoords.push_back((translate.getVec()[2] + tmpz) * scale);

			/*vertexCoords.push_back(tmpx);
			vertexCoords.push_back(tmpy);
			vertexCoords.push_back(tmpz);*/

		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == 't') {

			float tmpu, tmpv;
			sscanf(coord[i]->c_str(), "vt %f %f", &tmpu, &tmpv);

			textureCoords.push_back(tmpu);
			textureCoords.push_back(1.0 - tmpv);

		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == 'n') {
			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);

			normalCoords.push_back(tmpx);
			normalCoords.push_back(tmpy);
			normalCoords.push_back(tmpz);

		}else if ((*coord[i])[0] == 'u' &&  m_hasMaterial) {

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);
			std::map<std::string, int >::const_iterator iter = name.find(buffer);

			if (iter == name.end()) {
				// mlt name not found
				countMesh++;
				assign = countMesh;

				name[buffer] = countMesh;

			}else {
				// mlt name found
				assign = iter->second;

			}
		}else if ((*coord[i])[0] == 'g' && !m_hasMaterial) {

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);

			countMesh++;
			assign = countMesh;
			name[buffer] = countMesh;

		}else if ((*coord[i])[0] == 'f') {

			int a, b, c, n1, n2, n3, t1, t2, t3;


			if (!textureCoords.empty() && !normalCoords.empty()) {
				sscanf(coord[i]->c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d ", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);
				face.push_back({ { a, b, c, t1, t2, t3, n1, n2, n3, assign } });
			}else if (!normalCoords.empty()) {
				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);
				face.push_back({ { a, b, c, 0, 0, 0, n1, n2, n3, assign } });
			}else if (!textureCoords.empty()) {
				sscanf(coord[i]->c_str(), "f %d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3);
				face.push_back({ { a, b, c, t1, t2, t3, 0, 0, 0, assign } });
			}else {
				sscanf(coord[i]->c_str(), "f %d %d %d", &a, &b, &c);
				face.push_back({ { a, b, c, 0, 0, 0, 0, 0, 0, assign } });
			}
		}
	}

	std::sort(face.begin(), face.end(), compare);
	std::map<int, int> dup;

	for (int i = 0; i < face.size(); i++) {
		dup[face[i][9]]++;
	}

	std::map<int, int>::const_iterator iterDup = dup.begin();
	for (iterDup; iterDup != dup.end(); iterDup++) {

		if (name.empty()) {
			m_mesh.push_back(new Mesh(iterDup->second, this));
		}else {
		
			std::map<std::string, int >::const_iterator iterName = name.begin();
			for (iterName; iterName != name.end(); iterName++) {

				if (iterDup->first == iterName->second) {
					m_mesh.push_back(new Mesh("newmtl " + iterName->first, iterDup->second, this));
					if (m_mesh.size() > 1) {
						m_mesh[m_mesh.size() - 1]->m_triangleOffset = m_mesh[m_mesh.size() - 2]->m_numberOfTriangles + m_mesh[m_mesh.size() - 2]->m_triangleOffset;
					}
				}
			}
		}
	}

	m_numberOfMeshes = m_mesh.size();
	dup.clear();
	name.clear();

	IndexBufferCreator indexBufferCreator;
	indexBufferCreator.positionCoordsIn = vertexCoords;
	indexBufferCreator.normalCoordsIn = normalCoords;
	indexBufferCreator.textureCoordsIn = textureCoords;

	for (int j = 0; j < m_numberOfMeshes; j++) {

		std::vector<std::array<int, 10>>::const_iterator first = face.begin() + m_mesh[j]->m_triangleOffset;
		std::vector<std::array<int, 10>>::const_iterator last = face.begin() + (m_mesh[j]->m_triangleOffset + m_mesh[j]->m_numberOfTriangles);
		std::vector<std::array<int, 10>> subFace(first, last);

		indexBufferCreator.face = subFace;
		indexBufferCreator.createIndexBuffer();

		m_mesh[j]->m_indexBuffer = indexBufferCreator.indexBufferOut;
		m_mesh[j]->m_vertexBuffer = indexBufferCreator.vertexBufferOut;

		
		if (!textureCoords.empty() && !normalCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true;
			m_mesh[j]->m_stride = 8;
		}
		else if (!normalCoords.empty()) {
			m_hasNormals = true;
			m_mesh[j]->m_stride = 6;
		}
		else if (!textureCoords.empty()) {
			m_hasTextureCoords = true;
			m_mesh[j]->m_stride = 5;
		}else {
			m_mesh[j]->m_stride = 3;
		}

		if (m_hasMaterial) {
			m_mesh[j]->readMaterial();
		}
		m_mesh[j]->createTexture();
		m_mesh[j]->createBuffer();

		indexBufferCreator.indexBufferOut.clear();
		indexBufferCreator.indexBufferOut.shrink_to_fit();
		indexBufferCreator.vertexBufferOut.clear();
		indexBufferCreator.vertexBufferOut.shrink_to_fit();
	}

	indexBufferCreator.positionCoordsIn.clear();
	indexBufferCreator.positionCoordsIn.shrink_to_fit();
	indexBufferCreator.normalCoordsIn.clear();
	indexBufferCreator.normalCoordsIn.shrink_to_fit();
	indexBufferCreator.textureCoordsIn.clear();
	indexBufferCreator.textureCoordsIn.shrink_to_fit();
}

int Model::addVertex(int hash, float *pVertex, int stride) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(m_vertexBuffer.size() / stride);
		m_vertexBuffer.insert(m_vertexBuffer.end(), pVertex, pVertex + stride);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &m_vertexBuffer[index * 3];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(m_vertexBuffer.size() / stride);
			m_vertexBuffer.insert(m_vertexBuffer.end(), pVertex, pVertex + stride);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}

int Model::getNumberOfIndices() const {
	return static_cast<int>(m_indexBuffer.size());
}

void Model::draw() const {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->draw();
	}
}

void Model::draw(const Camera camera) const {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->draw(camera);
	}
}

void Model::setShader(const char* vertex, const char* fragment) {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->m_shader.reset(new Shader(vertex, fragment));
	}
}

void  Model::setShader(Shader* shader) {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->m_shader.reset(shader);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(std::string mltName, int numberTriangles, Model* model) : m_stride(0), m_triangleOffset(0) {
	m_numberOfTriangles = numberTriangles;
	m_mltName = mltName;
	m_model = model;

	m_material.diffuseTexPath = "";
	m_material.bumpMapPath = "";
	m_material.displacementMapPath = "";

	m_numBuffers = 4;

	m_positions.clear();
	m_vertexBuffer.clear();
}

Mesh::Mesh(int numberTriangles, Model* model) : m_stride(0), m_triangleOffset(0) {
	m_numberOfTriangles = numberTriangles;
	m_model = model;

	m_material.diffuseTexPath = "";
	m_material.bumpMapPath = "";
	m_material.displacementMapPath = "";

	m_numBuffers = 4;

	m_positions.clear();
	m_vertexBuffer.clear();
}

Mesh::Material Mesh::getMaterial() {
	return m_material;
}

void Mesh::setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies) {
	m_material.ambient[0] = ambient[0];
	m_material.ambient[1] = ambient[1];
	m_material.ambient[2] = ambient[2];

	m_material.diffuse[0] = diffuse[0];
	m_material.diffuse[1] = diffuse[1];
	m_material.diffuse[2] = diffuse[2];

	m_material.specular[0] = specular[0];
	m_material.specular[1] = specular[1];
	m_material.specular[2] = specular[2];

	m_material.shinies = shinies;
}

bool Mesh::readMaterial() {
	std::vector<std::string*>lines;
	int start = -1;
	int end = -1;

	std::ifstream in((m_model->getModelDirectory() + "/" + m_model->getMltPath()).c_str());

	if (!in.is_open()) {
		std::cout << "mlt file not found" << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)) {
		lines.push_back(new std::string(line));

	}
	in.close();

	for (int i = 0; i < lines.size(); i++) {

		if (strcmp((*lines[i]).c_str(), m_mltName.c_str()) == 0) {
			start = i;
			continue;
		}

		if ((*lines[i]).find("newmtl") != std::string::npos && start > 0) {
			end = i;
			break;

		}else {
			end = lines.size();
		}

	}

	if (start < 0 || end < 0) return false;

	for (int i = start; i < end; i++) {

		if ((*lines[i])[0] == '#') {
			continue;
		}else if ((*lines[i])[0] == 'N' && (*lines[i])[1] == 's') {
			int tmp;
			sscanf(lines[i]->c_str(), "Ns %i", &tmp);
			m_material.shinies = tmp;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'a') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ka %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.ambient[0] = tmpx;
			m_material.ambient[1] = tmpy;
			m_material.ambient[2] = tmpz;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'd') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Kd %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.diffuse[0] = tmpx;
			m_material.diffuse[1] = tmpy;
			m_material.diffuse[2] = tmpz;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 's') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ks %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.specular[0] = tmpx;
			m_material.specular[1] = tmpy;
			m_material.specular[2] = tmpz;

		}else if ((*lines[i])[0] == 'm') {

			char identifierBuffer[20], valueBuffer[250];;
			sscanf(lines[i]->c_str(), "%s %s", identifierBuffer, valueBuffer);

			if (strstr(identifierBuffer, "map_Kd") != 0) {
				m_material.diffuseTexPath = valueBuffer;

			}else if (strstr(identifierBuffer, "map_bump") != 0) {
				m_material.bumpMapPath = valueBuffer;

			}else if (strstr(identifierBuffer, "map_disp") != 0) {
				m_material.displacementMapPath = valueBuffer;
			}
		}
	}

	for (int i = 0; i < lines.size(); i++) {
		delete lines[i];
	}
}

void Mesh::createTexture() {
	m_texture = std::make_shared<Texture>(m_model->getModelDirectory() + "/" + m_material.diffuseTexPath);
}

void Mesh::createBuffer() {

	if (!m_indexBuffer.empty()) {
		for (int i = 0; i < m_vertexBuffer.size(); i = i + m_stride) {
			if (m_stride == 8) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector2f texCoord = Vector2f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4]);
				Vector3f normal = Vector3f(m_vertexBuffer[i + 5], m_vertexBuffer[i + 6], m_vertexBuffer[i + 7]);

				m_positions.push_back(position);
				m_texels.push_back(texCoord);
				m_normals.push_back(normal);

			}else if (m_stride == 6) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector3f normal = Vector3f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4], m_vertexBuffer[i + 5]);

				m_positions.push_back(position);
				m_normals.push_back(normal);

			}else if (m_stride == 5) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector2f texCoord = Vector2f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4]);

				m_positions.push_back(position);
				m_texels.push_back(texCoord);

			}else if (m_stride == 3) {

				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				m_positions.push_back(position);
			}
		}
	}

	m_drawCount = m_indexBuffer.size();

	//////////////////////////////////////////////setup vertex buffer objects//////////////////////////////////////////////////
	glGenBuffers(m_numBuffers, m_vbo);

	//positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);
	//texture coordinates
	if (m_model->m_hasTextureCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);
	}
	//texture coordinates
	if (m_model->m_hasNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);
	}
	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////setup vertex array objects//////////////////////////////////////////////////
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	//positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//texture coordinates
	if (m_model->m_hasTextureCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//texture coordinates
	if (m_model->m_hasNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[3]);
	glBindVertexArray(0);
}

void Mesh::draw() {	
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::draw(const Camera camera) {
	glUseProgram(m_shader->m_program);

	//m_texture->bind(0);
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix(), true);
	m_shader->loadMatrix("u_modelView", m_model->m_modelMatrix->getTransformationMatrix() * camera.getViewMatrix() , true);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

Mesh::~Mesh() {}
////////////////////////////////////////////////////////////////////////////////
void IndexBufferCreator::createIndexBuffer() {

	indexBufferOut.resize(face.size() * 3);
	if (!textureCoordsIn.empty() && !normalCoordsIn.empty()) {
		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1],
				normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 8);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1],
				normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 8);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
				normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 8);
		}

	}else if (!normalCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 6);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 6);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 6);
		}

	}else if (!textureCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 5);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3],positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 5);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 5);
		}

	}else {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 3);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 3);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 3);
		}
	}
	std::map<int, std::vector<int>>().swap(m_vertexCache);
	face.clear();
	face.shrink_to_fit();
}

int IndexBufferCreator::addVertex(int hash, const float *pVertex, int stride) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBufferOut.size() / stride);
		vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));

	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBufferOut[index * stride];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBufferOut.size() / stride);
			vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}