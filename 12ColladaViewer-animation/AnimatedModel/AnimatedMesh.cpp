#include "AnimatedMesh.h"
#include <vector>

#include "../tinyxml/tinyxml.h"



AnimatedMesh::AnimatedMesh(const std::string & _path) : path(_path) {

	//create local variables to load json into
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::uvec4> jointIds;
	std::vector<glm::vec4> jointWeights;
	std::vector<unsigned int> indices;

	loadData(positions, texCoords, normals, jointIds, jointWeights, indices);

	LoadJointsData(_boneIdMap, _boneDataMap);
	LoadJointsData2();
	rootJoint = createJoints(skeletonData.headJoint);
	rootJoint.calcInverseBindTransform(glm::mat4(1.0));

	

	Joint3 joint = rootJoint;
	//Joint3 joint2 = rootJoint.children[0];
	//Joint3 joint3 = rootJoint.children[0].children[0];

	_drawCount = indices.size();

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(NUM_BUFFERS, _vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[TEX_COORD]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Joint Ids
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[JOINT_IDS]);
	glBufferData(GL_ARRAY_BUFFER, jointIds.size() * sizeof(glm::uvec4), &jointIds.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, 0, 0);

	//Joint Weights
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[JOINT_WEIGHTS]);
	glBufferData(GL_ARRAY_BUFFER, jointWeights.size() * sizeof(glm::vec4), &jointWeights.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void AnimatedMesh::loadData(std::vector<glm::vec3> &_positions, std::vector<glm::vec2> &_texCoords, std::vector<glm::vec3> &_normals, std::vector<glm::uvec4> &_jointIds, std::vector<glm::vec4> &_jointWeights, std::vector<unsigned int> &_indices) {

	glm::mat4 CORRECTION = glm::mat4(1.0f);
	CORRECTION = glm::rotate(CORRECTION, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::uvec4> jointIds;
	std::vector<glm::vec4> jointWeights;
	std::vector<unsigned int> indices;

	int prim_count, num_indices, pos_count, norm_count, tex_count, numberOfTriangles;
	bool hasPositions, hasNormals, hasTexCoords;

	TiXmlDocument doc(path.c_str());
	doc.LoadFile();
	TiXmlElement* geometry = doc.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");
	TiXmlElement* mesh = geometry->FirstChildElement("mesh");


	TiXmlElement* primitives = mesh->FirstChildElement("triangles");
	primitives->QueryIntAttribute("count", &prim_count);


	TiXmlElement* input = primitives->FirstChildElement("input");
	short inputNodes = 0;
	while (input != NULL) {
		inputNodes++;
		if (std::string(input->Attribute("semantic")).compare("VERTEX") == 0) {
			hasPositions = true;
			input = input->NextSiblingElement("input");
			continue;
		}
		else if (std::string(input->Attribute("semantic")).compare("NORMAL") == 0) {
			hasNormals = true;
			input = input->NextSiblingElement("input");
			continue;
		}
		else if (std::string(input->Attribute("semantic")).compare("TEXCOORD") == 0) {
			hasTexCoords = true;
			input = input->NextSiblingElement("input");
			continue;
		}

		input = input->NextSiblingElement("input");
	}

	TiXmlElement* source = mesh->FirstChildElement("source");
	TiXmlElement* floatArray;
	char* text;

	while (source != NULL) {
		if (hasPositions) {

			floatArray = source->FirstChildElement("float_array");
			text = (char*)(floatArray->GetText());
			floatArray->QueryIntAttribute("count", &pos_count);
			pos_count = pos_count / 3;

			float x = atof(strtok(text, " "));  float y = atof(strtok(NULL, " ")); float z = atof(strtok(NULL, " "));

			glm::vec4 position = CORRECTION * glm::vec4(x, y, z, 1.0);
			positions.push_back(glm::vec3(position[0], position[1], position[2]));



			for (unsigned int index = 1; index < pos_count; index++) {
				x = atof(strtok(NULL, " ")); y = atof(strtok(NULL, " ")); z = atof(strtok(NULL, " "));
				position = CORRECTION * glm::vec4(x, y, z, 1.0);
				positions.push_back(glm::vec3(position[0], position[1], position[2]));
			}

			hasPositions = false;
			source = source->NextSiblingElement("source");
			continue;
		}
		else if (hasNormals) {
			floatArray = source->FirstChildElement("float_array");
			text = (char*)(floatArray->GetText());
			floatArray->QueryIntAttribute("count", &norm_count);
			norm_count = norm_count / 3;

			float x = atof(strtok(text, " "));  float y = atof(strtok(NULL, " ")); float z = atof(strtok(NULL, " "));
			glm::vec4 normal = CORRECTION * glm::vec4(x, y, z, 0.0);
			normals.push_back(glm::vec3(normal[0], normal[1], normal[2]));

			for (unsigned int index = 1; index < norm_count; index++) {
				x = atof(strtok(NULL, " ")); y = atof(strtok(NULL, " ")); z = atof(strtok(NULL, " "));
				normal = CORRECTION * glm::vec4(x, y, z, 0.0);
				normals.push_back(glm::vec3(normal[0], normal[1], normal[2]));
			}

			hasNormals = false;
			source = source->NextSiblingElement("source");
			continue;
		}
		else if (hasTexCoords) {
			floatArray = source->FirstChildElement("float_array");
			text = (char*)(floatArray->GetText());
			floatArray->QueryIntAttribute("count", &tex_count);
			tex_count = tex_count / 2;

			float u = atof(strtok(text, " "));  float v = atof(strtok(NULL, " "));
			texCoords.push_back(glm::vec2(u, v));

			for (unsigned int index = 1; index < tex_count; index++) {
				u = atof(strtok(NULL, " ")); v = atof(strtok(NULL, " "));
				texCoords.push_back(glm::vec2(u, v));
			}

			hasTexCoords = false;
			source = source->NextSiblingElement("source");
			continue;
		}
		source = source->NextSiblingElement("source");
	}

	primitives->QueryIntAttribute("count", &numberOfTriangles);
	TiXmlElement* p = primitives->FirstChildElement("p");

	// Read the index values
	text = (char*)(p->GetText());

	num_indices = 3 * inputNodes * prim_count;
	indices.push_back(atoi(strtok(text, " ")));
	for (int index = 1; index < num_indices; index++) {
		indices.push_back((unsigned int)atoi(strtok(NULL, " ")));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	TiXmlElement* controller = doc.RootElement()->FirstChildElement("library_controllers")->FirstChildElement("controller");
	TiXmlElement* skin = controller->FirstChildElement("skin");

	source = skin->FirstChildElement("source");

	TiXmlElement* joints = skin->FirstChildElement("joints");
	input = joints->FirstChildElement("input");

	std::vector<std::string> jointsList;

	while (input != NULL) {

		if (std::string(input->Attribute("semantic")).compare("JOINT") == 0) {

			std::string jointDataId = std::string(input->Attribute("source")).erase(0, 1);

			while (source != NULL) {

				if (std::string(source->Attribute("id")).compare(jointDataId) == 0) {

					TiXmlElement* nameArray = source->FirstChildElement("Name_array");

					text = (char*)(nameArray->GetText());

					jointsList.push_back(strtok(text, " "));
					for (int index = 1; index < atoi(nameArray->Attribute("count")); index++) {
						jointsList.push_back(strtok(NULL, " "));
					}

					break;
				}
				source = source->NextSiblingElement("source");
			}

			break;
		}

		input = input->NextSiblingElement("input");
	}

	TiXmlElement* vertexWeights = skin->FirstChildElement("vertex_weights");
	input = vertexWeights->FirstChildElement("input");

	std::vector<float> weights;

	while (input != NULL) {

		if (std::string(input->Attribute("semantic")).compare("WEIGHT") == 0) {

			std::string weightsDataId = std::string(input->Attribute("source")).erase(0, 1);

			while (source != NULL) {

				if (std::string(source->Attribute("id")).compare(weightsDataId) == 0) {

					TiXmlElement* floatArray = source->FirstChildElement("float_array");

					text = (char*)(floatArray->GetText());

					weights.push_back(atof(strtok(text, " ")));
					for (int index = 1; index < atoi(floatArray->Attribute("count")); index++) {
						weights.push_back(atof(strtok(NULL, " ")));
					}

					break;
				}
				source = source->NextSiblingElement("source");
			}
			break;
		}
		input = input->NextSiblingElement("input");
	}

	TiXmlElement* vcount = vertexWeights->FirstChildElement("vcount");
	std::vector<float> effectiveJointCounts;

	text = (char*)(vcount->GetText());
	char * pch = (char*)strtok(text, " ");
	effectiveJointCounts.push_back(atoi(pch));
	while ((pch = strtok(NULL, " ")) != NULL) {
		effectiveJointCounts.push_back(atoi(pch));
	}

	TiXmlElement* v = vertexWeights->FirstChildElement("v");

	std::vector<int> rawData;
	text = (char*)(v->GetText());
	pch = (char*)strtok(text, " ");
	rawData.push_back(atoi(pch));
	while ((pch = strtok(NULL, " ")) != NULL) {
		rawData.push_back(atoi(pch));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int maxWeights = 3;
	std::vector<VertexSkinData2> skinningData;
	int pointer = 0;
	for (int i = 0; i < effectiveJointCounts.size(); i++) {
		VertexSkinData2 skinData;
		for (int j = 0; j < effectiveJointCounts[i]; j++) {
			int jointId = rawData[pointer++];
			int weightId = rawData[pointer++];
		
			skinData.addJointEffect(jointId, weights[weightId]);
		}
		skinData.fill();
		skinData.limitJointNumber(maxWeights);


		skinningData.push_back(skinData);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	m_indexBuffer.resize(numberOfTriangles * 3);


	short offsetPositions = 0, offsetNormals = 1, offsetTexcoords = 2, offsetColors = 3, offset = inputNodes, vertexstride = 3 * inputNodes;

	for (int i = 0; i < numberOfTriangles; i++) {

		float vertex1[] = { positions[indices[i * vertexstride + offsetPositions + offset * 0]][0]    , positions[indices[i * vertexstride + offsetPositions + offset * 0]][1]    , positions[indices[i * vertexstride + offsetPositions + offset * 0]][2],
			normals[indices[i * vertexstride + offsetNormals + offset * 0]][0]  , normals[indices[i * vertexstride + offsetNormals + offset * 0]][1]  , normals[indices[i * vertexstride + offsetNormals + offset * 0]][2],
			texCoords[indices[i * vertexstride + offsetTexcoords + offset * 0]][0], texCoords[indices[i * vertexstride + offsetTexcoords + offset * 0]][1],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].jointIds[0], skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].jointIds[1], skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].jointIds[2],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].weights[0] , skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].weights[1] , skinningData[indices[i * vertexstride + offsetPositions + offset * 0]].weights[2]
		};

		m_indexBuffer[i * 3] = addVertex(indices[i * 3], &vertex1[0], 14);

		float vertex2[] = { positions[indices[i * vertexstride + offsetPositions + offset * 1]][0], positions[indices[i * vertexstride + offsetPositions + offset * 1]][1], positions[indices[i * vertexstride + offsetPositions + offset * 1]][2],
			normals[indices[i * vertexstride + offsetNormals + offset * 1]][0]  , normals[indices[i * vertexstride + offsetNormals + offset * 1]][1]   , normals[indices[i * vertexstride + offsetNormals + offset * 1]][2],
			texCoords[indices[i * vertexstride + offsetTexcoords + offset * 1]][0], texCoords[indices[i * vertexstride + offsetTexcoords + offset * 1]][1],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].jointIds[0], skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].jointIds[1], skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].jointIds[2],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].weights[0] , skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].weights[1] , skinningData[indices[i * vertexstride + offsetPositions + offset * 1]].weights[2]
		};

		m_indexBuffer[i * 3 + 1] = addVertex(indices[i * 3 + 3], &vertex2[0], 14);

		float vertex3[] = { positions[indices[i * vertexstride + offsetPositions + offset * 2]][0], positions[indices[i * vertexstride + offsetPositions + offset * 2]][1], positions[indices[i * vertexstride + offsetPositions + offset * 2]][2],
			normals[indices[i * vertexstride + offsetNormals + offset * 2]][0]  , normals[indices[i * vertexstride + offsetNormals + offset * 2]][1]  , normals[indices[i * 3 + offsetNormals + offset * 2]][2],
			texCoords[indices[i * vertexstride + offsetTexcoords + offset * 2]][0], texCoords[indices[i * vertexstride + offsetTexcoords + offset * 2]][1],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].jointIds[0], skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].jointIds[1], skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].jointIds[2],
			skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].weights[0] , skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].weights[1] , skinningData[indices[i * vertexstride + offsetPositions + offset * 2]].weights[2]
		};

		m_indexBuffer[i * 3 + 2] = addVertex(indices[i * 3 + 6], &vertex3[0], 14);
	}

	for (int i = 0; i < m_indexBuffer.size(); i++) {

		glm::vec3 position = glm::vec3(m_vertexBuffer[m_indexBuffer[i] * 14], m_vertexBuffer[m_indexBuffer[i] * 14 + 1], m_vertexBuffer[m_indexBuffer[i] * 14 + 2]);
		glm::vec3 normal = glm::vec3(m_vertexBuffer[m_indexBuffer[i] * 14 + 3], m_vertexBuffer[m_indexBuffer[i] * 14 + 4], m_vertexBuffer[m_indexBuffer[i] * 14 + 5]);
		glm::vec2 texCoord = glm::vec2(m_vertexBuffer[m_indexBuffer[i] * 14 + 6], 1.0 - m_vertexBuffer[m_indexBuffer[i] * 14 + 7]);
		glm::uvec4 jointId = glm::uvec4(m_vertexBuffer[m_indexBuffer[i] * 14 + 8], m_vertexBuffer[m_indexBuffer[i] * 14 + 9], m_vertexBuffer[m_indexBuffer[i] * 14 + 10], 0);
		glm::vec4 jointWeight = glm::vec4(m_vertexBuffer[m_indexBuffer[i] * 14 + 11], m_vertexBuffer[m_indexBuffer[i] * 14 + 12], m_vertexBuffer[m_indexBuffer[i] * 14 + 13], 0.0);

		_positions.push_back(position);
		_texCoords.push_back(texCoord);
		_normals.push_back(normal);
		_jointIds.push_back(jointId);
		_jointWeights.push_back(jointWeight);
	}
	_indices = m_indexBuffer;
}

int AnimatedMesh::addVertex(int hash, const float *pVertex, int numberOfBytes) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(m_vertexBuffer.size() / numberOfBytes);
		m_vertexBuffer.insert(m_vertexBuffer.end(), pVertex, pVertex + numberOfBytes);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));

	}
	else {
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


void AnimatedMesh::LoadJointsData(std::unordered_map<std::string, unsigned int> &boneIdMap, std::unordered_map<unsigned int, BoneData> &boneDataMap) {

	

	TiXmlDocument doc(path.c_str());
	doc.LoadFile();

	TiXmlElement* controller = doc.RootElement()->FirstChildElement("library_controllers")->FirstChildElement("controller");
	TiXmlElement* skin = controller->FirstChildElement("skin");

	TiXmlElement* source = skin->FirstChildElement("source");

	TiXmlElement* joints = skin->FirstChildElement("joints");
	TiXmlElement* input = joints->FirstChildElement("input");

	char* text;
	while (input != NULL) {

		if (std::string(input->Attribute("semantic")).compare("JOINT") == 0) {

			std::string jointDataId = std::string(input->Attribute("source")).erase(0, 1);
			while (source != NULL) {

				if (std::string(source->Attribute("id")).compare(jointDataId) == 0) {

					TiXmlElement* nameArray = source->FirstChildElement("Name_array");

					text = (char*)(nameArray->GetText());

					jointsList.push_back(strtok(text, " "));
				
					for (int index = 1; index < atoi(nameArray->Attribute("count")); index++) {
						jointsList.push_back(strtok(NULL, " "));
					}

					break;
				}
				source = source->NextSiblingElement("source");
			}

			break;
		}
		input = input->NextSiblingElement("input");
	}

	TiXmlElement* rootNode = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node")->FirstChildElement("node");

	for (int i = 0; i < jointsList.size(); i++) {

		std::string name = std::string(rootNode->Attribute("sid"));
		if (name.compare(jointsList[i]) == 0) {
			char* text = (char*)rootNode->FirstChild()->FirstChild()->Value();

			glm::mat4 offsetMatrix;
			offsetMatrix[0][0] = atof(strtok(text, " "));

			short start = 1;
			for (int i = 0; i < 4; i++) {
				for (int j = start; j < 4; j++) {
					offsetMatrix[i][j] = atof(strtok(NULL, " "));
				}
				start = 0;
			}

			offsetMatrix = glm::transpose(offsetMatrix);
			offsetMatrix =  offsetMatrix;

			BoneData data;
			data.offsetMatrix = offsetMatrix;

			boneIdMap.insert(std::make_pair(name, i));
			boneDataMap.insert(std::make_pair(i, data));

			continue;

		}else {

			search(rootNode, jointsList[i], i, boneIdMap, boneDataMap);
		}
	}
}

void AnimatedMesh::LoadJointsData2() {

	glm::mat4 CORRECTION = glm::mat4(1.0f);
	CORRECTION = glm::rotate(CORRECTION, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	TiXmlDocument doc(path.c_str());
	doc.LoadFile();

	TiXmlElement* rootNode = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node")->FirstChildElement("node");

	char* text = (char*)rootNode->FirstChild()->FirstChild()->Value();

	glm::mat4 offsetMatrix;
	offsetMatrix[0][0] = atof(strtok(text, " "));

	short start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {

			offsetMatrix[i][j] = atof(strtok(NULL, " "));

		}
		start = 0;
	}

	offsetMatrix = glm::transpose(offsetMatrix);
	offsetMatrix = CORRECTION * offsetMatrix;

	std::string name = std::string(((TiXmlElement*)rootNode)->Attribute("sid"));
	std::vector<std::string>::iterator it = std::find(jointsList.begin(), jointsList.end(), name);

	skeletonData.headJoint = JointData(std::distance(jointsList.begin(), it), std::string(((TiXmlElement*)rootNode)->Attribute("id")), offsetMatrix);

	TiXmlNode* nextNode = rootNode->FirstChildElement("node");
	while (nextNode != NULL) {
		skeletonData.headJoint.addChild(search(nextNode));
		nextNode = nextNode->NextSiblingElement("node");
	}
	skeletonData.jointCount = jointsList.size();
}

void AnimatedMesh::search(TiXmlNode* node, std::string jointName, int index, std::unordered_map<std::string, unsigned int> &boneIdMap, std::unordered_map<unsigned int, BoneData> &boneDataMap) {


	for (TiXmlNode* child = node->FirstChild(); child; child = child->NextSibling()) {

		if (std::string(child->Value()).compare("node") == 0) {

			std::string name = std::string(((TiXmlElement*)child)->Attribute("sid"));

			if (name.compare(jointName) == 0) {



				char* text = (char*)child->FirstChild()->FirstChild()->Value();
				glm::mat4 offsetMatrix;
				offsetMatrix[0][0] = atof(strtok(text, " "));

				short start = 1;
				for (int i = 0; i < 4; i++) {
					for (int j = start; j < 4; j++) {
						offsetMatrix[i][j] = atof(strtok(NULL, " "));
					}
					start = 0;
				}

				offsetMatrix = glm::transpose(offsetMatrix);
				offsetMatrix = offsetMatrix;

				BoneData data;
				data.offsetMatrix = offsetMatrix;

				boneIdMap.insert(std::make_pair(name, index));
				boneDataMap.insert(std::make_pair(index, data));

			}
			else {
				search(child, jointName, index, boneIdMap, boneDataMap);
			}

		}
	}
}

JointData AnimatedMesh::search(TiXmlNode* _node) {

	JointData data;
	TiXmlNode* node = _node;

	char* text = (char*)node->FirstChild()->FirstChild()->Value();

	glm::mat4 offsetMatrix;
	offsetMatrix[0][0] = atof(strtok(text, " "));

	short start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {
			offsetMatrix[i][j] = atof(strtok(NULL, " "));
		}
		start = 0;
	}

	offsetMatrix = glm::transpose(offsetMatrix);
	offsetMatrix = offsetMatrix;

	std::string name = ((TiXmlElement*)node)->Attribute("sid");
	std::vector<std::string>::iterator it = std::find(jointsList.begin(), jointsList.end(), name);
	data.index = std::distance(jointsList.begin(), it);
	data.nameId = ((TiXmlElement*)node)->Attribute("id");
	data.bindLocalTransform = offsetMatrix;

	TiXmlNode* nextNode = node->FirstChildElement("node");
	while (nextNode) {
		data.addChild(search(nextNode));
		nextNode = nextNode->NextSiblingElement("node");
	}
	return data;
}

Joint3 AnimatedMesh::createJoints(JointData data) {
	Joint3 joint = Joint3(data.index, data.nameId, data.bindLocalTransform);
	for (JointData child : data.children) {
		joint.addChild(createJoints(child));
	}
	return joint;
}

AnimatedMesh::~AnimatedMesh() {
	glDeleteVertexArrays(1, &_vao);
}

void AnimatedMesh::Draw() {

	glBindVertexArray(_vao);
	//glDrawArrays(GL_TRIANGLES, 0, _drawCount);
	glDrawElements(GL_TRIANGLES, _drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<glm::mat4> AnimatedMesh::GetBoneArray() {

	std::vector<glm::mat4> boneArray;

	boneArray.assign(_boneDataMap.size(), glm::mat4());
	for (auto i : _boneDataMap) {

		boneArray[i.first] = i.second.finalTransformation;
	}

	return boneArray;
}

std::vector<glm::mat4> AnimatedMesh::GetBoneArray2() {
	std::vector<glm::mat4> boneArray;
	boneArray.assign(_boneDataMap.size(), glm::mat4());
	addJointsToArray(rootJoint, boneArray);

	return boneArray;
}

void AnimatedMesh::addJointsToArray(Joint3 rootJoint, std::vector<glm::mat4> &boneArray) {

	boneArray[rootJoint.index] = rootJoint.animatedTransform;

	for (int i = 0; i < rootJoint.children.size(); i++) {
		addJointsToArray(rootJoint.children[i], boneArray);
	}


}

void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose) {
	applyPoseToJoints(currentPose, rootJoint, glm::mat4(1.0));
}




void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose, Joint3 &joint, glm::mat4 parentTransform) {

	glm::mat4 CORRECTION = glm::mat4(1.0f);
	CORRECTION = glm::rotate(CORRECTION, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 currentTransform = currentPose.find(joint.name) == currentPose.end() ? joint.localBindTransform : parentTransform * currentPose.at(joint.name);

	if (joint.name.compare("Torso") == 0) {
		currentTransform = CORRECTION * currentTransform;
	}

	if (joint.name.compare("Armature_stalk") == 0) {
		currentTransform = CORRECTION * currentTransform;
	}

	if (joint.name.compare("metarig_forearm_R_001") == 0) {
		currentTransform =  currentTransform;
	}

	if (joint.name.compare("metarig_hand_R_001") == 0) {
		/**parentTransform = CORRECTION * currentTransform from "metarig_forearm_R_001"**/
		currentTransform = parentTransform * currentPose.at(joint.name);
	}
	

	for (int i = 0; i < joint.children.size(); i++) {	
			applyPoseToJoints(currentPose, joint.children[i], currentTransform);	
	}
	
	if (currentPose.find(joint.name) == currentPose.end()) {
		//check for identity maybe there is a bette way
		if (glm::length2(parentTransform[0]) == 1 && 
			glm::length2(parentTransform[1]) == 1 && 
			glm::length2(parentTransform[2]) == 1 && 
			glm::length2(parentTransform[3]) == 1) {

			joint.animatedTransform =  joint.localBindTransform * joint.inverseBindTransform;
		}else {
			joint.animatedTransform = parentTransform;
		}
	}else {		

		joint.animatedTransform = currentTransform * joint.inverseBindTransform;
	}
}