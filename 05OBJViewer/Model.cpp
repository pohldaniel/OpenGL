#include "Model.h"

Model::Model() {

	m_hasTextureCoords = false;
	m_hasNormals = false;
	T.identity();
	invT.identity();
}

Model::~Model(){


}

void Model::rotate(const Vector3f &axis, float degrees){

	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
								  rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
								  rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
								  rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

	T = rotMtx * T;
	invT = invT * invRotMtx;

}

void Model::translate(float dx, float dy, float dz){

	T[0][3] = T[0][3] + dx*T[0][0] + dz*T[0][2] - dy*T[0][1];
	T[1][3] = T[1][3] + dx*T[1][0] + dz*T[1][2] - dy*T[1][1];
	T[2][3] = T[2][3] + dx*T[2][0] + dz*T[2][2] - dy*T[2][1];

	invT[0][0] = invT[0][0] - invT[3][0] * dx; invT[1][0] = invT[1][0] - invT[3][0] * dz; invT[2][0] = invT[2][0] + invT[3][0] * dy;
	invT[0][1] = invT[0][1] - invT[3][1] * dx; invT[1][1] = invT[1][1] - invT[3][1] * dz; invT[2][1] = invT[2][1] + invT[3][1] * dy;
	invT[0][2] = invT[0][2] - invT[3][2] * dx; invT[1][2] = invT[1][2] - invT[3][2] * dz; invT[2][2] = invT[2][2] + invT[3][2] * dy;
	invT[0][3] = invT[0][3] -			   dx; invT[1][3] = invT[1][3] +			  dy; invT[2][3] = invT[2][3] -				 dz;

	
}

void Model::scale(float a, float b, float c){

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	T[0][0] = T[0][0] * a;  T[0][1] = T[0][1] * b; T[0][2] = T[0][2] * c; 
	T[1][0] = T[1][0] * a;  T[1][1] = T[1][1] * b; T[1][2] = T[1][2] * c; 
	T[2][0] = T[2][0] * a;  T[2][1] = T[2][1] * b; T[2][2] = T[2][2] * c; 


	invT[0][0] = invT[0][0] * (1.0 / a); invT[1][0] = invT[1][0] * (1.0 / b); invT[2][0] = invT[2][0] * (1.0 / c);
	invT[0][1] = invT[0][1] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[2][1] = invT[2][1] * (1.0 / c);
	invT[0][2] = invT[0][2] * (1.0 / a); invT[1][2] = invT[1][2] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	invT[0][3] = invT[0][3] * (1.0 / a); invT[1][3] = invT[1][3] * (1.0 / b); invT[2][3] = invT[2][3] * (1.0 / c);
}

const Matrix4f &Model::getTransformationMatrix() const{

	return T;
}

const Matrix4f &Model::getInvTransformationMatrix() const{

	return invT;

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

bool Model::hasTextureCoords() const{

	return m_hasTextureCoords;
}

bool Model::hasNormals() const{

	return m_hasNormals;
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

		}else if ((*coord[i])[0] == 'u'){

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

			

		}else if ((*coord[i])[0] == 'f'){

			int a, b, c, n1, n2, n3, t1, t2, t3;


			if (!textureCoords.empty() && !normalCoords.empty()){
				sscanf(coord[i]->c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d ", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);

				face.push_back({ { a, b, c, n1, n2, n3, t1, t2, t3, assign } });

			}else if (!normalCoords.empty()){

				sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);

				face.push_back({ { a, b, c, n1, n2, n2, 0, 0, 0, assign } });


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

	std::map<std::string, int >::const_iterator iterName = name.begin();
	std::map<int, int>::const_iterator iterDup = dup.begin();
	
	for (iterDup; iterDup != dup.end(); iterDup++, iterName++) {
		
		mesh.push_back(new Mesh("newmtl " + iterName->first,  iterDup->second));

	}

	
	dup.clear();
	name.clear();

	int start = 0;
	int end = mesh[0]->m_numberTriangles;


	if (!textureCoords.empty() && !normalCoords.empty()){
	
		m_hasTextureCoords = true;
		m_hasNormals = true;

		

		for (int j = 0; j < m_numberOfMeshes; j++){

			// Allocate memory for the OBJ model data
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
			
			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}
	
	

			for (int i = start, k = 0; i < end; i++, k++){

				

				const float vert1[8] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					normalCoords[((face[i])[3] - 1) * 3], normalCoords[((face[i])[3] - 1) * 3 + 1], normalCoords[((face[i])[3] - 1) * 3 + 2],
					textureCoords[((face[i])[6] - 1) * 2], textureCoords[((face[i])[6] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 8);

				const float vert2[8] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					normalCoords[((face[i])[4] - 1) * 3], normalCoords[((face[i])[4] - 1) * 3 + 1], normalCoords[((face[i])[4] - 1) * 3 + 2],
					textureCoords[((face[i])[7] - 1) * 2], textureCoords[((face[i])[7] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 8);

				const float vert3[8] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					normalCoords[((face[i])[5] - 1) * 3], normalCoords[((face[i])[5] - 1) * 3 + 1], normalCoords[((face[i])[5] - 1) * 3 + 2],
					textureCoords[((face[i])[8] - 1) * 2], textureCoords[((face[i])[8] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 8);

			}
		}
	
	
	}else if (!normalCoords.empty()){

		m_hasNormals = true;

		for (int j = 0; j < m_numberOfMeshes; j++){

			// Allocate memory for the OBJ model data
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
		

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
			
				const float vert1[6] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					normalCoords[((face[i])[3] - 1) * 3], normalCoords[((face[i])[3] - 1) * 3 + 1], normalCoords[((face[i])[3] - 1) * 3 + 2] };


				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 6);

				const float vert2[6] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					normalCoords[((face[i])[4] - 1) * 3], normalCoords[((face[i])[4] - 1) * 3 + 1], normalCoords[((face[i])[4] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 6);

				const float vert3[6] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					normalCoords[((face[i])[5] - 1) * 3], normalCoords[((face[i])[5] - 1) * 3 + 1], normalCoords[((face[i])[5] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 6);
			
			}
		}

	}else if (!textureCoords.empty()){

		m_hasTextureCoords = true;

		for (int j = 0; j < m_numberOfMeshes; j++){

			// Allocate memory for the OBJ model data
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);
		
			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
			
				const float vert1[5] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2],
					textureCoords[((face[i])[6] - 1) * 2], textureCoords[((face[i])[6] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 5);

				const float vert2[5] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2],
					textureCoords[((face[i])[7] - 1) * 2], textureCoords[((face[i])[7] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 5);

				const float vert3[5] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2],
					textureCoords[((face[i])[8] - 1) * 2], textureCoords[((face[i])[8] - 1) * 2 + 1] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 5);
			
			}
		}

	}else {

		for (int j = 0; j < m_numberOfMeshes; j++){

			// Allocate memory for the OBJ model data
			mesh[j]->m_indexBuffer.resize(mesh[j]->m_numberTriangles * 3);

			if (j > 0){

				start = end;
				end = end + mesh[j]->m_numberTriangles;
			}

			for (int i = start, k = 0; i < end; i++, k++){
			
				const float vert1[3] = { vertexCoords[((face[i])[0] - 1) * 3], vertexCoords[((face[i])[0] - 1) * 3 + 1], vertexCoords[((face[i])[0] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3] = mesh[j]->addVertex((face[i])[0] - 1, vert1, 3);

				const float vert2[3] = { vertexCoords[((face[i])[1] - 1) * 3], vertexCoords[((face[i])[1] - 1) * 3 + 1], vertexCoords[((face[i])[1] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 1] = mesh[j]->addVertex((face[i])[1] - 1, vert2, 3);

				const float vert3[3] = { vertexCoords[((face[i])[2] - 1) * 3], vertexCoords[((face[i])[2] - 1) * 3 + 1], vertexCoords[((face[i])[2] - 1) * 3 + 2] };

				mesh[j]->m_indexBuffer[k * 3 + 2] = mesh[j]->addVertex((face[i])[2] - 1, vert3, 3);
			
			}
		}

	}


	


	for (int i = 0; i < coord.size(); i++){

		delete coord[i];
	}

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



}

Mesh::Mesh(std::string mltName, int numberTriangles){

	m_numberTriangles = numberTriangles;
	m_mltName = mltName;

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


void Mesh::setVertexName(unsigned int a_vertexName){

	m_vertexName = a_vertexName;
}

void Mesh::setIndexName(unsigned int a_indexName){

	m_indexName = a_indexName;
}


void Mesh::setTextureName(unsigned int a_textureName){

	m_textureName = a_textureName;
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
