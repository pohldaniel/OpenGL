#include "Model.h"

Model::Model() {

	m_hasMaterial = false;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_mltPath = "";
	
	modelMatrix = new ModelMatrix();
}

Model::~Model(){

	delete modelMatrix;
}

void Model::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz){

	modelMatrix->setRotPos(axis, degrees, dx, dy, dz);
}

void Model::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz){

	modelMatrix->setRotXYZPos(axisX, degreesX, 
							  axisY, degreesY,
						      axisZ, degreesZ,
							  dx, dy, dz);
}

void Model::rotate(const Vector3f &axis, float degrees){

	modelMatrix->rotate(axis, degrees);
}

void Model::translate(float dx, float dy, float dz){

	modelMatrix->translate(dx, dy, dz);
}

void Model::scale(float a, float b, float c){

	modelMatrix->scale(a, b, c);	
}

const Matrix4f &Model::getTransformationMatrix() const{

	return modelMatrix->getTransformationMatrix();
}

const Matrix4f &Model::getInvTransformationMatrix() const{

	return modelMatrix->getInvTransformationMatrix();
}

const Vector3f &Model::getCenter() const{

	return m_center;
}

std::vector<Mesh*> Model::getMesches(){

	return mesh;	
}

std::string Model::getMltPath(){

	return m_mltPath;
}

std::string Model::getModelDirectory(){

	return m_modelDirectory;
}

int Model::numberOfMeshes(){

	return m_numberOfMeshes;
}

bool Model::hasMaterial() const{

	return  m_hasMaterial;
}

bool Model::loadObject(const char* filename){

	return loadObject(filename, Vector3f(0.0, 0.0, 0.0), 1.0);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs){

	return i_lhs[9] < i_rhs[9];
}

bool Model::loadObject(const char* a_filename, Vector3f& translate, float scale){

	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index){

		m_modelDirectory = filename.substr(0, index);
	}

	//std::vector<std::string*>coord;
	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;

	std::map<std::string, int> name;

	int countMesh = 0;
	int assign = 0;
	int countFacesWithTexture = 0;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	char buffer[250];

	FILE * pFile = fopen(a_filename, "r");
	if (pFile == NULL) {
		std::cout << "File not found" << std::endl;
		return false;
	}

	while (fscanf(pFile, "%s", buffer) != EOF) {

		switch (buffer[0]) {

		case '#': {

			fgets(buffer, sizeof(buffer), pFile);
			break;

		}case 'm': {

			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s %s", buffer, buffer);
			m_mltPath = buffer;

			m_hasMaterial = true;
			break;

		}case 'v': {

			switch (buffer[1]) {

			case '\0': {

				float tmpx, tmpy, tmpz;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

				tmpx = tmpx * scale + translate[0];
				tmpy = tmpy * scale + translate[1];
				tmpz = tmpz * scale + translate[2];

				vertexCoords.push_back(tmpx);
				vertexCoords.push_back(tmpy);
				vertexCoords.push_back(tmpz);

				xmin = (std::min)(tmpx, xmin);
				ymin = (std::min)(tmpy, ymin);
				zmin = (std::min)(tmpz, zmin);

				xmax = (std::max)(tmpx, xmax);
				ymax = (std::max)(tmpy, ymax);
				zmax = (std::max)(tmpz, zmax);
				break;

			}case 't': {

				float tmpu, tmpv;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f", &tmpu, &tmpv);

				textureCoords.push_back(tmpu);
				textureCoords.push_back(tmpv);
				break;

			}case 'n': {

				float tmpx, tmpy, tmpz;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

				normalCoords.push_back(tmpx);
				normalCoords.push_back(tmpy);
				normalCoords.push_back(tmpz);
				break;

			}default: {

				break;
			}
			}
			break;

		}case 'u': {

			if (m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);

				std::map<std::string, int >::const_iterator iter = name.find(buffer);

				if (iter == name.end()) {
					// mlt name not found
					countMesh++;
					assign = countMesh;

					name[buffer] = countMesh;

				}
				else {
					// mlt name found
					assign = iter->second;
				}
			}
			break;

		}case 'g': {

			if (!m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s", buffer);

				countMesh++;
				assign = countMesh;
				name[buffer] = countMesh;
			}
			break;

		}case 'f': {

			int a, b, c, n1, n2, n3, t1, t2, t3;
			fgets(buffer, sizeof(buffer), pFile);

			if (!textureCoords.empty() && !normalCoords.empty()) {
				sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d ", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);
				face.push_back({ { a, b, c, t1, t2, t3, n1, n2, n3, assign } });

			}
			else if (!normalCoords.empty()) {
				sscanf(buffer, "%d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);
				face.push_back({ { a, b, c, 0, 0, 0, n1, n2, n3, assign } });

			}
			else if (!textureCoords.empty()) {
				sscanf(buffer, "%d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3);
				face.push_back({ { a, b, c, t1, t2, t3, 0, 0, 0, assign } });

			}
			else {
				sscanf(buffer, "%d %d %d", &a, &b, &c);
				face.push_back({ { a, b, c, 0, 0, 0, 0, 0, 0, assign } });
			}
			break;

		}default: {

			break;
		}

		}//end switch
	}// end while
	fclose(pFile);
	
	m_center = Vector3f((xmin + xmax) / 2.0f, (ymin + ymax) / 2.0f, (zmin + zmax) / 2.0f);

	std::sort(face.begin(), face.end(), compare);
	std::map<int, int> dup;

	for (int i = 0; i < face.size(); i++){
		dup[face[i][9]]++;
	}

	std::map<int, int>::const_iterator iterDup = dup.begin();

	for (iterDup; iterDup != dup.end(); iterDup++){
		
		if (name.empty()){

			mesh.push_back(new Mesh( iterDup->second));

		}else{

			std::map<std::string, int >::const_iterator iterName = name.begin();
			for (iterName; iterName != name.end(); iterName++){

				if (iterDup->first == iterName->second){

					mesh.push_back(new Mesh("newmtl " + iterName->first, iterDup->second));
				}
			}

		}
	}
		
	dup.clear();
	name.clear();

	m_numberOfMeshes = mesh.size();
	int start = 0;
	int end = mesh[0]->m_numberTriangles;

	if (!textureCoords.empty() && !normalCoords.empty()){
	
		m_hasTextureCoords = true;
		m_hasNormals = true;

		
		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasTextureCoords = true;
			mesh[j]->m_hasNormals = true;
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			mesh[j]->m_numberOfBytes = 8;

			int numberOfTriangle = 0;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}
	
			for (int i = start; i < end; i++){

				float vertex1[] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					textureCoords[((face[i])[3] - 1) * 2], textureCoords[((face[i])[3] - 1) * 2 + 1],
					normalCoords[((face[i])[6] - 1) * 3], normalCoords[((face[i])[6] - 1) * 3 + 1], normalCoords[((face[i])[6] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3] = mesh[j]->addVertex(((face[i])[0] - 1), &vertex1[0], 8);

				float vertex2[] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					textureCoords[((face[i])[4] - 1) * 2], textureCoords[((face[i])[4] - 1) * 2 + 1],
					normalCoords[((face[i])[7] - 1) * 3], normalCoords[((face[i])[7] - 1) * 3 + 1], normalCoords[((face[i])[7] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 1] = mesh[j]->addVertex(((face[i])[1] - 1), &vertex2[0], 8);

				float vertex3[] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					textureCoords[((face[i])[5] - 1) * 2], textureCoords[((face[i])[5] - 1) * 2 + 1],
					normalCoords[((face[i])[8] - 1) * 3], normalCoords[((face[i])[8] - 1) * 3 + 1], normalCoords[((face[i])[8] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 2] = mesh[j]->addVertex(((face[i])[2] - 1), &vertex3[0], 8);

				numberOfTriangle++;
				
			}
		}	
	}else if (!normalCoords.empty()){
	
		m_hasNormals = true;

		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasNormals = true;
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			mesh[j]->m_numberOfBytes = 6;

			int numberOfTriangle = 0;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start; i < end; i++){
			
				float vertex1[] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					normalCoords[((face[i])[6] - 1) * 3], normalCoords[((face[i])[6] - 1) * 3 + 1], normalCoords[((face[i])[6] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3] = mesh[j]->addVertex(((face[i])[0] - 1), &vertex1[0], 6);

				float vertex2[] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					normalCoords[((face[i])[7] - 1) * 3], normalCoords[((face[i])[7] - 1) * 3 + 1], normalCoords[((face[i])[7] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 1] = mesh[j]->addVertex(((face[i])[1] - 1), &vertex2[0], 6);

				float vertex3[] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					normalCoords[((face[i])[8] - 1) * 3], normalCoords[((face[i])[8] - 1) * 3 + 1], normalCoords[((face[i])[8] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 2] = mesh[j]->addVertex(((face[i])[2] - 1), &vertex3[0], 6);

				numberOfTriangle++;			
			}		
		}
	}else if (!textureCoords.empty()){

		m_hasTextureCoords = true;
		
		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasTextureCoords = true;
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			mesh[j]->m_numberOfBytes = 5;
			int numberOfTriangle = 0;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start; i < end; i++){
			
				float vertex1[] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					textureCoords[((face[i])[3] - 1) * 2], textureCoords[((face[i])[3] - 1) * 2 + 1] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3] = mesh[j]->addVertex(((face[i])[0] - 1), &vertex1[0], 5);

				float vertex2[] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					textureCoords[((face[i])[4] - 1) * 2], textureCoords[((face[i])[4] - 1) * 2 + 1] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 1] = mesh[j]->addVertex(((face[i])[1] - 1), &vertex2[0], 5);

				float vertex3[] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					textureCoords[((face[i])[5] - 1) * 2], textureCoords[((face[i])[5] - 1) * 2 + 1] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 2] = mesh[j]->addVertex(((face[i])[2] - 1), &vertex3[0], 5);

				numberOfTriangle++;
			
			}
		}
	}else {

		for (int j = 0; j < m_numberOfMeshes; j++){
			
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			mesh[j]->m_numberOfBytes = 3;
			int numberOfTriangle = 0;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start; i < end; i++){
				
				float vertex1[] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3] = mesh[j]->addVertex(((face[i])[0] - 1), &vertex1[0], 3);

				float vertex2[] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 1] = mesh[j]->addVertex(((face[i])[1] - 1), &vertex2[0], 3);

				float vertex3[] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2] };
				mesh[j]->m_indexBuffer[numberOfTriangle * 3 + 2] = mesh[j]->addVertex(((face[i])[2] - 1), &vertex3[0], 3);

				numberOfTriangle++;			
			}			
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mesh::generateNormals(){

	if (m_hasNormals) {  return; }
	
	std::vector<float> tmpVertex;

	const int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;
	int modulo = m_hasTextureCoords ? 5 : 3;
	int vertexLength = m_hasTextureCoords ? 8 : 6;
	int vertexOffset = m_hasTextureCoords ? 2 : 0;

		int totalTriangles = getNumberOfTriangles();

		for (int i = 0; i < m_vertexBuffer.size(); i++){


			tmpVertex.push_back(m_vertexBuffer[i]);


			if ((i + 1) % modulo == 0){

				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
			}
		}


		for (int i = 0; i < totalTriangles; i++){

			pTriangle = &m_indexBuffer[i * 3];

			pVertex0 = &m_vertexBuffer[pTriangle[0] * modulo];
			pVertex1 = &m_vertexBuffer[pTriangle[1] * modulo];
			pVertex2 = &m_vertexBuffer[pTriangle[2] * modulo];

			// Calculate triangle face normal.
			edge1[0] = pVertex1[0] - pVertex0[0];
			edge1[1] = pVertex1[1] - pVertex0[1];
			edge1[2] = pVertex1[2] - pVertex0[2];

			edge2[0] = pVertex2[0] - pVertex0[0];
			edge2[1] = pVertex2[1] - pVertex0[1];
			edge2[2] = pVertex2[2] - pVertex0[2];

			normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
			normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
			normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

			// Accumulate the normals.
			tmpVertex[pTriangle[0] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[pTriangle[0] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[pTriangle[0] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[pTriangle[1] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[pTriangle[1] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[pTriangle[1] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[pTriangle[2] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[pTriangle[2] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[pTriangle[2] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 5 + vertexOffset] + normal[2];
		}



		// Normalize the vertex normals.
		for (int i = 0; i < tmpVertex.size(); i = i + vertexLength){

			float length = 1.0f / sqrtf(tmpVertex[i + 3 + vertexOffset] * tmpVertex[i + 3 + vertexOffset] +
				tmpVertex[i + 4 + vertexOffset] * tmpVertex[i + 4 + vertexOffset] +
				tmpVertex[i + 5 + vertexOffset] * tmpVertex[i + 5 + vertexOffset]);

			tmpVertex[i + 3 + vertexOffset] *= length;
			tmpVertex[i + 4 + vertexOffset] *= length;
			tmpVertex[i + 5 + vertexOffset] *= length;
		}


		m_vertexBuffer.clear();
		copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(m_vertexBuffer));
		tmpVertex.clear();

		m_hasNormals = true;
}

void Mesh::generateTangents(){

	if (m_hasTangents){ return; }
	if (!m_hasTextureCoords){ std::cout << "TextureCoords needed!" << std::endl; return; }
	if (!m_hasNormals){
		generateNormals();
	}

	std::vector<float> tmpVertex;
	const int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;
	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;



	int totalVertices = getNumberOfVertices();
	int totalTriangles = getNumberOfTriangles();

		for (int i = 0; i < m_vertexBuffer.size(); i++){

			tmpVertex.push_back(m_vertexBuffer[i]);

			if ((i + 1) % 8 == 0){

				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
				tmpVertex.push_back(0.0);
			}
		}



		// Calculate the vertex tangents and bitangents.
		for (int i = 0; i < totalTriangles; ++i){

			pTriangle = &m_indexBuffer[i * 3];

			pVertex0 = &m_vertexBuffer[pTriangle[0] * 8];
			pVertex1 = &m_vertexBuffer[pTriangle[1] * 8];
			pVertex2 = &m_vertexBuffer[pTriangle[2] * 8];

			// Calculate the triangle face tangent and bitangent.

			edge1[0] = pVertex1[0] - pVertex0[0];
			edge1[1] = pVertex1[1] - pVertex0[1];
			edge1[2] = pVertex1[2] - pVertex0[2];

			edge2[0] = pVertex2[0] - pVertex0[0];
			edge2[1] = pVertex2[1] - pVertex0[1];
			edge2[2] = pVertex2[2] - pVertex0[2];

			texEdge1[0] = pVertex1[3] - pVertex0[3];
			texEdge1[1] = pVertex1[4] - pVertex0[4];

			texEdge2[0] = pVertex2[3] - pVertex0[3];
			texEdge2[1] = pVertex2[4] - pVertex0[4];

			det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

			if (fabs(det) < 1e-6f){

				tangent[0] = 1.0f;
				tangent[1] = 0.0f;
				tangent[2] = 0.0f;

				bitangent[0] = 0.0f;
				bitangent[1] = 1.0f;
				bitangent[2] = 0.0f;

			}else{

				det = 1.0f / det;

				tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
				tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
				tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

				bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
				bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
				bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
			}

	
			// Accumulate the tangents and bitangents.
			tmpVertex[pTriangle[0] * 14 + 8] = tmpVertex[pTriangle[0] * 14 + 8] + tangent[0];
			tmpVertex[pTriangle[0] * 14 + 9] = tmpVertex[pTriangle[0] * 14 + 9] + tangent[1];
			tmpVertex[pTriangle[0] * 14 + 10] = tmpVertex[pTriangle[0] * 14 + 10] + tangent[2];

			tmpVertex[pTriangle[0] * 14 + 11] = tmpVertex[pTriangle[0] * 14 + 11] + bitangent[0];
			tmpVertex[pTriangle[0] * 14 + 12] = tmpVertex[pTriangle[0] * 14 + 12] + bitangent[1];
			tmpVertex[pTriangle[0] * 14 + 13] = tmpVertex[pTriangle[0] * 14 + 13] + bitangent[2];

			tmpVertex[pTriangle[1] * 14 + 8] = tmpVertex[pTriangle[1] * 14 + 8] + tangent[0];
			tmpVertex[pTriangle[1] * 14 + 9] = tmpVertex[pTriangle[1] * 14 + 9] + tangent[1];
			tmpVertex[pTriangle[1] * 14 + 10] = tmpVertex[pTriangle[1] * 14 + 10] + tangent[2];

			tmpVertex[pTriangle[1] * 14 + 11] = tmpVertex[pTriangle[1] * 14 + 11] + bitangent[0];
			tmpVertex[pTriangle[1] * 14 + 12] = tmpVertex[pTriangle[1] * 14 + 12] + bitangent[1];
			tmpVertex[pTriangle[1] * 14 + 13] = tmpVertex[pTriangle[1] * 14 + 13] + bitangent[2];

			tmpVertex[pTriangle[2] * 14 + 8] = tmpVertex[pTriangle[2] * 14 + 8] + tangent[0];
			tmpVertex[pTriangle[2] * 14 + 9] = tmpVertex[pTriangle[2] * 14 + 9] + tangent[1];
			tmpVertex[pTriangle[2] * 14 + 10] = tmpVertex[pTriangle[2] * 14 + 10] + tangent[2];

			tmpVertex[pTriangle[2] * 14 + 11] = tmpVertex[pTriangle[2] * 14 + 11] + bitangent[0];
			tmpVertex[pTriangle[2] * 14 + 12] = tmpVertex[pTriangle[2] * 14 + 12] + bitangent[1];
			tmpVertex[pTriangle[2] * 14 + 13] = tmpVertex[pTriangle[2] * 14 + 13] + bitangent[2];

			
		}

		

		// Orthogonalize and normalize the vertex tangents.
		for (int i = 0; i < tmpVertex.size(); i = i + 14){

			pVertex0 = &tmpVertex[i];

			// Gram-Schmidt orthogonalize tangent with normal.

			nDotT = pVertex0[5] * pVertex0[8] +
				pVertex0[6] * pVertex0[9] +
				pVertex0[7] * pVertex0[10];

			pVertex0[8] -= pVertex0[5] * nDotT;
			pVertex0[9] -= pVertex0[6] * nDotT;
			pVertex0[10] -= pVertex0[7] * nDotT;

			// Normalize the tangent.

			length = 1.0f / sqrtf(pVertex0[8] * pVertex0[8] +
				pVertex0[9] * pVertex0[9] +
				pVertex0[10] * pVertex0[10]);

			pVertex0[8] *= length;
			pVertex0[9] *= length;
			pVertex0[10] *= length;

			bitangent[0] = (pVertex0[6] * pVertex0[10]) -
				(pVertex0[7] * pVertex0[9]);
			bitangent[1] = (pVertex0[7] * pVertex0[8]) -
				(pVertex0[5] * pVertex0[10]);
			bitangent[2] = (pVertex0[5] * pVertex0[9]) -
				(pVertex0[6] * pVertex0[8]);

			bDotB = bitangent[0] * pVertex0[11] +
				bitangent[1] * pVertex0[12] +
				bitangent[2] * pVertex0[13];

			// Calculate handedness
			if (bDotB < 0.0f){
				pVertex0[11] = -bitangent[0];
				pVertex0[12] = -bitangent[1];
				pVertex0[13] = -bitangent[2];

			}else{

				pVertex0[11] = bitangent[0];
				pVertex0[12] = bitangent[1];
				pVertex0[13] = bitangent[2];
			}

			/*pVertex0[11] = bitangent[0];
			pVertex0[12] = bitangent[1];
			pVertex0[13] = bitangent[2];*/
		}


		m_vertexBuffer.clear();
		copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(m_vertexBuffer));
		tmpVertex.clear();

		m_hasTangents = true;
}


bool Mesh::mltCompare(std::string* mltName){
	return *mltName == m_mltName;
}

bool Mesh::readMaterial(const char* filename){
	

	std::vector<std::string*>lines;
	int start = -1;
	int end = -1;

	std::ifstream in(filename);


	if (!in.is_open()){

		std::cout << "mlt file not found" << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)){
		lines.push_back(new std::string(line));
		
	}
	in.close();


	for (int i = 0; i < lines.size(); i++){

		if (strcmp((*lines[i]).c_str(), m_mltName.c_str()) == 0){

			start = i;
			continue;
		}

		
		if ((*lines[i]).find("newmtl") != std::string::npos && start > 0){
			
			end = i;
			break;
		}else {

			end = lines.size();
		}

	}
	
	if (start < 0 || end < 0) return false;

	for (int i = start; i < end; i++){

		if ((*lines[i])[0] == '#'){

			continue;

		}else if ((*lines[i])[0] == 'N' && (*lines[i])[1] == 's'){

			int tmp;
			sscanf(lines[i]->c_str(), "Ns %i", &tmp);
			//static_cast<std::shared_ptr<Phong>>(m_material.get())->m_shinies = tmp;
			m_material.shinies = tmp;


		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'a'){
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ka %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.ambient[0] = tmpx;
			m_material.ambient[1] = tmpy;
			m_material.ambient[2] = tmpz;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'd'){
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Kd %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.diffuse[0] = tmpx;
			m_material.diffuse[1] = tmpy;
			m_material.diffuse[2] = tmpz;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 's'){
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ks %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.specular[0] = tmpx;
			m_material.specular[1] = tmpy;
			m_material.specular[2] = tmpz;

		}else if ((*lines[i])[0] == 'm'){

			
			char identifierBuffer[20], valueBuffer[250];;
			sscanf(lines[i]->c_str(), "%s %s", identifierBuffer, valueBuffer);
			
			if (strstr(identifierBuffer, "map_Kd") != 0){
				
				m_material.colorMapPath = valueBuffer;

				
			}else if (strstr(identifierBuffer, "map_bump") != 0){

				m_material.bumpMapPath = valueBuffer;

			}else if (strstr(identifierBuffer, "map_disp") != 0){

				m_material.displacementMapPath = valueBuffer;

			}
			
		}
	}



	for (int i = 0; i < lines.size(); i++){

		delete lines[i];
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh() {

	Mesh::xmin = FLT_MAX;
	Mesh::ymin = FLT_MAX;
	Mesh::zmin = FLT_MAX;
	Mesh::xmax = -FLT_MAX;
	Mesh::ymax = -FLT_MAX;
	Mesh::zmax = -FLT_MAX;

	Mesh::m_numberTriangles = 0;

	Mesh::m_material.colorMapPath = "";
	Mesh::m_material.bumpMapPath = "";
	Mesh::m_material.displacementMapPath = "";

	Mesh::m_hasNormals = false;
	Mesh::m_hasTextureCoords = false;
	Mesh::m_hasTangents = false;
}

Mesh::Mesh(std::string mltName, int numberTriangles){

	m_color = Vector3f(1.0, 1.0, 1.0);
	m_numberTriangles = numberTriangles;
	m_mltName = mltName;

	Mesh::m_material.colorMapPath = "";
	Mesh::m_material.bumpMapPath = "";
	Mesh::m_material.displacementMapPath = "";

	Mesh::m_hasNormals = false;
	Mesh::m_hasTextureCoords = false;
	Mesh::m_hasTangents = false;
}

Mesh::Mesh( int numberTriangles){

	m_color = Vector3f(1.0, 1.0, 1.0);
	m_numberTriangles = numberTriangles;

	Mesh::m_material.colorMapPath = "";
	Mesh::m_material.bumpMapPath = "";
	Mesh::m_material.displacementMapPath = "";

	Mesh::m_hasNormals = false;
	Mesh::m_hasTextureCoords = false;
	Mesh::m_hasTangents = false;
}


Mesh::~Mesh(){


}

const int *Mesh::getIndexBuffer() const{

	return &m_indexBuffer[0];
}

const float *Mesh::getVertexBuffer() const{

	return &m_vertexBuffer[0];
}

int Mesh::getNumberOfIndices() const{

	return static_cast<int>(m_indexBuffer.size());
}

int Mesh::getNumberOfVertices() const{

	return static_cast<int>(m_vertexBuffer.size());
}

int Mesh::getNumberOfTriangles() const{

	return  m_numberTriangles;
}

std::string Mesh::getMltName(){

	return m_mltName;
}




Mesh::Material Mesh::getMaterial(){

	return m_material;
}

unsigned int Mesh::getVertexName(){

	return m_vertexName;
}

unsigned int Mesh::getIndexName(){

	return m_indexName;
}

unsigned int Mesh::getTextureName(){

	return m_textureName;
}

unsigned int Mesh::getNormalMap(){

	return m_normalMap;
}

unsigned int Mesh::getDisplacementMap(){

	return m_displacementMap;
}

const Vector3f &Mesh::getColor() const {
	return m_color;
}



void Mesh::setVertexName(unsigned int a_vertexName){

	m_vertexName = a_vertexName;
}

void Mesh::setIndexName(unsigned int a_indexName){

	m_indexName = a_indexName;
}


void Mesh::setTextureName(unsigned int a_textureName){

	m_textureName = a_textureName;
}

void Mesh::setNormalMap(unsigned int a_normalMap){

	m_normalMap = a_normalMap;
}

void Mesh::setColor(const Vector3f &color){
	m_color = color;

}

void Mesh::setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies){

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

int Mesh::addVertex(int hash, const float *pVertex, int numberOfBytes){

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(m_vertexBuffer.size() / numberOfBytes);
		m_vertexBuffer.insert(m_vertexBuffer.end(), pVertex, pVertex + numberOfBytes);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));

	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &m_vertexBuffer[index * 3];



			if (memcmp(pCachedVertex, pVertex, sizeof(float)* numberOfBytes) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(m_vertexBuffer.size() / numberOfBytes);
			m_vertexBuffer.insert(m_vertexBuffer.end(), pVertex, pVertex + numberOfBytes);
			m_vertexCache[hash].push_back(index);
		}
	}

	return index;
}
