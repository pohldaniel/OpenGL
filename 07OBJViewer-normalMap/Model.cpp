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

/*bool Model::hasTextureCoords() const{

	return m_hasTextureCoords;
}

bool Model::hasNormals() const{

	return m_hasNormals;
}

bool Model::hasTangents() const{

	return m_hasTangents;
}*/

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

	if (!in.is_open()){

		std::cout << "File not found" << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)){
		coord.push_back(new std::string(line));

	}
	in.close();

	

	for (int i = 0; i < coord.size(); i++){

		if ((*coord[i])[0] == '#'){

			continue;

		}else if ((*coord[i])[0] == 'm'){

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);
			m_mltPath = buffer;
		
			m_hasMaterial = true;
			
		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == ' '){


			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);

			/*vertexCoords.push_back((translate.getVec()[0] + tmpx) * scale);
			vertexCoords.push_back((translate.getVec()[1] + tmpy) * scale);
			vertexCoords.push_back((translate.getVec()[2] + tmpz) * scale);*/

			vertexCoords.push_back(tmpx);
			vertexCoords.push_back(tmpy);
			vertexCoords.push_back(tmpz);

		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == 't'){

			float tmpu, tmpv;
			sscanf(coord[i]->c_str(), "vt %f %f", &tmpu, &tmpv);

			textureCoords.push_back(tmpu);
			textureCoords.push_back(tmpv);

		}else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == 'n'){
			float tmpx, tmpy, tmpz;
			sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);

			normalCoords.push_back(tmpx);
			normalCoords.push_back(tmpy);
			normalCoords.push_back(tmpz);

		}else if ((*coord[i])[0] == 'u' &&  m_hasMaterial){

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);

			

			std::map<std::string, int >::const_iterator iter = name.find(buffer);

			if (iter == name.end()){
				// mlt name not found
				countMesh++;
				assign = countMesh;

				name[buffer] = countMesh;

				
			}
			else{
				// mlt name found
				assign = iter->second;

			}
			//std::cout << assign << "  " << buffer << std::endl;
			

		}else if ((*coord[i])[0] == 'g' && !m_hasMaterial){

			

			sscanf(coord[i]->c_str(), "%s %s", buffer, buffer);

			countMesh++;
			assign = countMesh;
			name[buffer] = countMesh;

		}else if ((*coord[i])[0] == 'f'){

			int a, b, c, n1, n2, n3, t1, t2, t3;


			if (!textureCoords.empty() && !normalCoords.empty()){
				sscanf(coord[i]->c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d ", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);

				face.push_back({ { a, b, c, n1, n2, n3, t1, t2, t3, assign } });

			}else if (!normalCoords.empty()){

				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);

				face.push_back({ { a, b, c, n1, n2, n3, 0, 0, 0, assign } });


			}else if (!textureCoords.empty()){

				sscanf(coord[i]->c_str(), "f %d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3);

				face.push_back({ { a, b, c, 0, 0, 0, t1, t2, t3, assign } });


			}else {

				sscanf(coord[i]->c_str(), "f %d %d %d", &a, &b, &c);

				face.push_back({ { a, b, c, 0, 0, 0, 0, 0, 0, assign } });
			}


		}
	}
	

	

	std::sort(face.begin(), face.end(), compare);

	std::map<int, int> dup;

	for (int i = 0; i < face.size(); i++){
		dup[face[i][9]]++;
	}

	m_numberOfMeshes = dup.size();
	
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

	int start = 0;
	int end = mesh[0]->m_numberTriangles;


	if (!textureCoords.empty() && !normalCoords.empty()){
	
		m_hasTextureCoords = true;
		m_hasNormals = true;

		

		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasTextureCoords = true;
			mesh[j]->m_hasNormals = true;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}
	
			

			for (int i = start, k = 0; i < end; i++, k++){


				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[6] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[6] - 1) * 2 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3 + 2]);
				
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[7] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[7] - 1) * 2 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3 + 2]);
				
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[8] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[8] - 1) * 2 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3 + 2]);
				
			}

			indexVBO_PTN(mesh[j]->m_vertexBuffer, mesh[j]->m_indexBuffer, mesh[j]->m_vertexBuffer);


		}
	
	
	}else if (!normalCoords.empty()){
	
		m_hasNormals = true;

		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasNormals = true;

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
			
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[3] - 1) * 3 + 2]);


				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[4] - 1) * 3 + 2]);

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(normalCoords[((face[i])[5] - 1) * 3 + 2]);
			
			}
			indexVBO_PN(mesh[j]->m_vertexBuffer, mesh[j]->m_indexBuffer, mesh[j]->m_vertexBuffer);
		}

	}else if (!textureCoords.empty()){

		m_hasTextureCoords = true;
		
		for (int j = 0; j < m_numberOfMeshes; j++){

			mesh[j]->m_hasTextureCoords = true;

			//mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
			

			/*	const float vert1[5] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					textureCoords[((face[i])[6] - 1) * 2], textureCoords[((face[i])[6] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 5);

				const float vert2[5] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					textureCoords[((face[i])[7] - 1) * 2], textureCoords[((face[i])[7] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 5);

				const float vert3[5] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					textureCoords[((face[i])[8] - 1) * 2], textureCoords[((face[i])[8] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 5);*/

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[6] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[6] - 1) * 2 + 1]);

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[7] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[7] - 1) * 2 + 1]);

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[8] - 1) * 2]);
				mesh[j]->m_vertexBuffer.push_back(textureCoords[((face[i])[8] - 1) * 2 + 1]);
			
			}

			indexVBO_PT(mesh[j]->m_vertexBuffer, mesh[j]->m_indexBuffer, mesh[j]->m_vertexBuffer);
		}

	}else {

		for (int j = 0; j < m_numberOfMeshes; j++){
			//mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
				

				/*const float vert1[3] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 3);

				const float vert2[3] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 3);

				const float vert3[3] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 3);*/

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[0] - 1) * 3 + 2]);

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[1] - 1) * 3 + 2]);

				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 1]);
				mesh[j]->m_vertexBuffer.push_back(vertexCoords[((face[i])[2] - 1) * 3 + 2]);
			
			}

			indexVBO_P(mesh[j]->m_vertexBuffer, mesh[j]->m_indexBuffer, mesh[j]->m_vertexBuffer);
		}

	}

	
	


	for (int i = 0; i < coord.size(); i++){

		delete coord[i];
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

	if (m_hasTangents){ std::cout << "Tangents already generate!" << std::endl; return; }
	if (!m_hasTextureCoords){ std::cout << "TextureCoords needed!" << std::endl; return; }
	if (!m_hasNormals){
		generateNormals();
		std::cout << "Normals generated, they needed to calculate the bitangents and setup the TNB-matrix!" << std::endl;

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
			tmpVertex[pTriangle[0] * 15 + 8] = tmpVertex[pTriangle[0] * 15 + 8] + tangent[0];
			tmpVertex[pTriangle[0] * 15 + 9] = tmpVertex[pTriangle[0] * 15 + 9] + tangent[1];
			tmpVertex[pTriangle[0] * 15 + 10] = tmpVertex[pTriangle[0] * 15 + 10] + tangent[2];

			tmpVertex[pTriangle[0] * 15 + 12] = tmpVertex[pTriangle[0] * 15 + 12] + bitangent[0];
			tmpVertex[pTriangle[0] * 15 + 13] = tmpVertex[pTriangle[0] * 15 + 13] + bitangent[1];
			tmpVertex[pTriangle[0] * 15 + 14] = tmpVertex[pTriangle[0] * 15 + 14] + bitangent[2];

			tmpVertex[pTriangle[1] * 15 + 8] = tmpVertex[pTriangle[1] * 15 + 8] + tangent[0];
			tmpVertex[pTriangle[1] * 15 + 9] = tmpVertex[pTriangle[1] * 15 + 9] + tangent[1];
			tmpVertex[pTriangle[1] * 15 + 10] = tmpVertex[pTriangle[1] * 15 + 10] + tangent[2];

			tmpVertex[pTriangle[1] * 15 + 12] = tmpVertex[pTriangle[1] * 15 + 12] + bitangent[0];
			tmpVertex[pTriangle[1] * 15 + 13] = tmpVertex[pTriangle[1] * 15 + 13] + bitangent[1];
			tmpVertex[pTriangle[1] * 15 + 14] = tmpVertex[pTriangle[1] * 15 + 14] + bitangent[2];

			tmpVertex[pTriangle[2] * 15 + 8] = tmpVertex[pTriangle[2] * 15 + 8] + tangent[0];
			tmpVertex[pTriangle[2] * 15 + 9] = tmpVertex[pTriangle[2] * 15 + 9] + tangent[1];
			tmpVertex[pTriangle[2] * 15 + 10] = tmpVertex[pTriangle[2] * 15 + 10] + tangent[2];

			tmpVertex[pTriangle[2] * 15 + 12] = tmpVertex[pTriangle[2] * 15 + 12] + bitangent[0];
			tmpVertex[pTriangle[2] * 15 + 13] = tmpVertex[pTriangle[2] * 15 + 13] + bitangent[1];
			tmpVertex[pTriangle[2] * 15 + 14] = tmpVertex[pTriangle[2] * 15 + 14] + bitangent[2];


		}

		// Orthogonalize and normalize the vertex tangents.
		for (int i = 0; i < tmpVertex.size(); i = i + 15){

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

			// Calculate the handedness of the local tangent space.
			// The bitangent vector is the cross product between the triangle face
			// normal vector and the calculated tangent vector. The resulting
			// bitangent vector should be the same as the bitangent vector
			// calculated from the set of linear equations above. If they point in
			// different directions then we need to invert the cross product
			// calculated bitangent vector. We store this scalar multiplier in the
			// tangent vector's 'w' component so that the correct bitangent vector
			// can be generated in the normal mapping shader's vertex shader.
			//
			// Normal maps have a left handed coordinate system with the origin
			// located at the top left of the normal map texture. The x coordinates
			// run horizontally from left to right. The y coordinates run
			// vertically from top to bottom. The z coordinates run out of the
			// normal map texture towards the viewer. Our handedness calculations
			// must take this fact into account as well so that the normal mapping
			// shader's vertex shader will generate the correct bitangent vectors.
			// Some normal map authoring tools such as Crazybump
			// (http://www.crazybump.com/) includes options to allow you to control
			// the orientation of the normal map normal's y-axis.

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
			pVertex0[11] = (bDotB < 0.0f) ? 1.0f : -1.0f;

			pVertex0[12] = bitangent[0];
			pVertex0[13] = bitangent[1];
			pVertex0[14] = bitangent[2];
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

		std::cout << "File not found" << std::endl;
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

		}
		else if ((*lines[i])[0] == 'm'){

			
			char identifierBuffer[20], valueBuffer[250];;
			sscanf(lines[i]->c_str(), "%s %s", identifierBuffer, valueBuffer);
			
			if (strstr(identifierBuffer, "map_Kd") != 0){
				
				m_material.colorMapPath = valueBuffer;

				
			}else if (strstr(identifierBuffer, "map_bump") != 0){

				m_material.bumpMapPath = valueBuffer;

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

	Mesh::m_hasNormals = false;
	Mesh::m_hasTextureCoords = false;
	Mesh::m_hasTangents = false;
}

Mesh::Mesh(std::string mltName, int numberTriangles){

	m_color = Vector3f(1.0, 1.0, 1.0);
	m_numberTriangles = numberTriangles;
	m_mltName = mltName;


	Mesh::m_hasNormals = false;
	Mesh::m_hasTextureCoords = false;
	Mesh::m_hasTangents = false;
}

Mesh::Mesh( int numberTriangles){

	m_color = Vector3f(1.0, 1.0, 1.0);
	m_numberTriangles = numberTriangles;

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



/////////////////////////////////////////////////////////////////

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2){
	return v1 == v2;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool getSimilarVertexIndex(float p1, float p2, float p3, float n1, float n2, float n3, std::vector<float>& out_vertices, int &index)

{
	// Lame linear search
	for (unsigned int i = 0; i<out_vertices.size(); i = i + 6){
		if (
			is_near(p1, out_vertices[i]) &&
			is_near(p2, out_vertices[i + 1]) &&
			is_near(p3, out_vertices[i + 2]) &&
			is_near(n1, out_vertices[i + 3]) &&
			is_near(n2, out_vertices[i + 4]) &&
			is_near(n3, out_vertices[i + 5])
			){
			index = i / 6;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

bool getSimilarVertexIndex(float p1, float p2, float p3, float t1, float t2, float n1, float n2, float n3, std::vector<float>& out_vertices, int &index)

{
	// Lame linear search
	for (unsigned int i = 0; i<out_vertices.size(); i = i + 8){
		if (
			is_near(p1, out_vertices[i]) &&
			is_near(p2, out_vertices[i + 1]) &&
			is_near(p3, out_vertices[i + 2]) &&
			is_near(t1, out_vertices[i + 3]) &&
			is_near(t2, out_vertices[i + 4]) &&
			is_near(n1, out_vertices[i + 5]) &&
			is_near(n2, out_vertices[i + 6]) &&
			is_near(n3, out_vertices[i + 7])
			){
			index = i / 8;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

bool getSimilarVertexIndex(float p1, float p2, float p3, float t1, float t2, std::vector<float>& out_vertices, int &index)

{
	// Lame linear search
	for (unsigned int i = 0; i<out_vertices.size(); i = i + 5){
		if (
			is_near(p1, out_vertices[i]) &&
			is_near(p2, out_vertices[i + 1]) &&
			is_near(p3, out_vertices[i + 2]) &&
			is_near(t1, out_vertices[i + 3]) &&
			is_near(t2, out_vertices[i + 4])
			){
			index = i / 5;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}


bool getSimilarVertexIndex(float p1, float p2, float p3, std::vector<float>& out_vertices, int &index){
	// Lame linear search
	for (unsigned int i = 0; i<out_vertices.size(); i = i + 3){
		if (
			is_near(p1, out_vertices[i]) &&
			is_near(p2, out_vertices[i + 1]) &&
			is_near(p3, out_vertices[i + 2])

			){
			index = i / 3;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}


void Model::indexVBO_P(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices){
	// For each input vertex
	for (int i = 0; i<in_vertices.size(); i = i + 3){

		// Try to find a similar vertex in out_XXXX
		int index;
		bool found = getSimilarVertexIndex(in_vertices[i],
			in_vertices[i + 1],
			in_vertices[i + 2],
			out_vertices, index);

		if (found){ // A similar vertex is already in the VBO, use it instead !



			out_indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_vertices.push_back(in_vertices[i + 1]);
			out_vertices.push_back(in_vertices[i + 2]);
			out_indices.push_back((int)out_vertices.size() / 3 - 1);
		}
	}
}

void Model::indexVBO_PN(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices){
	// For each input vertex
	for (int i = 0; i<in_vertices.size(); i = i + 6){

		// Try to find a similar vertex in out_XXXX
		int index;
		bool found = getSimilarVertexIndex(in_vertices[i],
			in_vertices[i + 1],
			in_vertices[i + 2],
			in_vertices[i + 3],
			in_vertices[i + 4],
			in_vertices[i + 5],
			out_vertices, index);

		if (found){ // A similar vertex is already in the VBO, use it instead !

			out_indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_vertices.push_back(in_vertices[i + 1]);
			out_vertices.push_back(in_vertices[i + 2]);
			out_vertices.push_back(in_vertices[i + 3]);
			out_vertices.push_back(in_vertices[i + 4]);
			out_vertices.push_back(in_vertices[i + 5]);

			out_indices.push_back((int)out_vertices.size() / 6 - 1);
		}
	}
}



void Model::indexVBO_PT(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices){
	// For each input vertex
	for (int i = 0; i<in_vertices.size(); i = i + 5){

		// Try to find a similar vertex in out_XXXX
		int index;
		bool found = getSimilarVertexIndex(in_vertices[i],
			in_vertices[i + 1],
			in_vertices[i + 2],
			in_vertices[i + 3],
			in_vertices[i + 4],
			out_vertices, index);

		if (found){ // A similar vertex is already in the VBO, use it instead !

			out_indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_vertices.push_back(in_vertices[i + 1]);
			out_vertices.push_back(in_vertices[i + 2]);
			out_vertices.push_back(in_vertices[i + 3]);
			out_vertices.push_back(in_vertices[i + 4]);

			out_indices.push_back((int)out_vertices.size() / 5 - 1);
		}
	}
}


void Model::indexVBO_PTN(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices){
	// For each input vertex
	for (int i = 0; i<in_vertices.size(); i = i + 8){

		// Try to find a similar vertex in out_XXXX
		int index;
		bool found = getSimilarVertexIndex(in_vertices[i],
			in_vertices[i + 1],
			in_vertices[i + 2],
			in_vertices[i + 3],
			in_vertices[i + 4],
			in_vertices[i + 5],
			in_vertices[i + 6],
			in_vertices[i + 7],
			out_vertices, index);

		if (found){ // A similar vertex is already in the VBO, use it instead !

			out_indices.push_back(index);
		}
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_vertices.push_back(in_vertices[i + 1]);
			out_vertices.push_back(in_vertices[i + 2]);
			out_vertices.push_back(in_vertices[i + 3]);
			out_vertices.push_back(in_vertices[i + 4]);
			out_vertices.push_back(in_vertices[i + 5]);
			out_vertices.push_back(in_vertices[i + 6]);
			out_vertices.push_back(in_vertices[i + 7]);

			out_indices.push_back((int)out_vertices.size() / 8 - 1);
		}
	}
}



int Mesh::addVertex(int hash, const float *pVertex, int n){

	int index = -1;
	std::map<int, int >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()){

		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(m_vertexBuffer.size() / n);

		for (int i = 0; i < n; i++){


			m_vertexBuffer.push_back(pVertex[i]);
		}


		m_vertexCache[hash] = index;

	}
	else {


		// One or more vertices have been hashed to this entry in the cache.
		const float *pCachedVertex = 0;
		bool found = false;

		for (iter; iter != m_vertexCache.end(); iter++){

			index = iter->second;
			pCachedVertex = &m_vertexBuffer[(index)* n];


			if (memcmp(pCachedVertex, pVertex, 4 * n) == 0)
			{

				found = true;
				break;
			}

		}

		if (!found)
		{
			index = static_cast<int>(m_vertexBuffer.size() / n);


			for (int i = 0; i < n; i++){
				m_vertexBuffer.push_back(pVertex[i]);
			}

			m_vertexCache[hash] = index;
		}


	}

	return index;
}
