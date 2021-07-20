# include "ColladaLoader.h"
# include "..\AnimatedModel\AnimatedMesh.h"
# include "..\Animator\Animation.h"

ColladaLoader::ColladaLoader(const std::string & path) : m_path(path) {

	m_vertexBuffer.clear();
	m_indexBuffer.clear();
	m_vertexCache.clear();

	inputNodes = 0;
	offsetPositions = 0;
	offsetNormals = 0;
	offsetTexcoords = 0;
	offsetColors = 0;

	prim_count = 0;
	num_indices = 0;
	pos_count = 0;
	norm_count = 0;
	tex_count = 0;
	numberOfTriangles = 0;

	hasPositions = false;
	hasNormals = false;
	hasTexCoords = false;
	hasColors = false;

	doc = TiXmlDocument(m_path.c_str());
	doc.LoadFile();
}

ColladaLoader::~ColladaLoader() {
	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	
	std::map<int, std::vector<int>>().swap(m_vertexCache);
	doc.Clear();
}

void ColladaLoader::loadData(std::vector<Vector3f> &positions,
							 std::vector<Vector2f> &texCoords,
							 std::vector<Vector3f> &normals,
							 std::vector<std::array<unsigned int, 4>> &jointIds,
							 std::vector<Vector4f> &jointWeights,
							 std::vector<unsigned int> &indices, 
							 std::vector<std::string> &jointsList) {

	std::vector<VertexSkinData> skinningData;

	loadGeometry(positions, texCoords, normals, indices);
	loadController(jointIds, jointWeights, jointsList, skinningData);
	createIndexBuffer(positions, texCoords, normals, jointIds, jointWeights, indices, skinningData);
	
	//fill up the 'SkeletonData: skeletonData' structure. Needed to create the 'Joint: rootJoint' structure out of "AnimatedMesh.h"
	loadVisualScene(jointsList);

	/** free the memory of temporarily vector **/
	skinningData.clear();
	skinningData.shrink_to_fit();

	/** alternative **/
	//std::vector<VertexSkinData>().swap(skinningData);
}

void ColladaLoader::loadGeometry(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<unsigned int> &indices) {

	TiXmlElement* geometry = doc.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");
	TiXmlElement* mesh = geometry->FirstChildElement("mesh");

	TiXmlElement* primitives = mesh->FirstChildElement("triangles");
	primitives->QueryIntAttribute("count", &prim_count);


	TiXmlElement* input = primitives->FirstChildElement("input");
	
	while (input != NULL) {
		int queryResult;
		

		if (std::string(input->Attribute("semantic")).compare("VERTEX") == 0) {
			inputNodes++;
			hasPositions = true;
			input->QueryIntAttribute("offset", &queryResult);
			offsetPositions = queryResult;
			input = input->NextSiblingElement("input");
			continue;

		}else if (std::string(input->Attribute("semantic")).compare("NORMAL") == 0) {
			inputNodes++;
			hasNormals = true;
			input->QueryIntAttribute("offset", &queryResult);
			offsetNormals = queryResult;
			input = input->NextSiblingElement("input");
			continue;

		}else if (std::string(input->Attribute("semantic")).compare("TEXCOORD") == 0) {
			inputNodes++;
			hasTexCoords = true;
			input->QueryIntAttribute("offset", &queryResult);
			offsetTexcoords = queryResult;
			input = input->NextSiblingElement("input");
			continue;

		}else if (std::string(input->Attribute("semantic")).compare("COLOR") == 0) {
			inputNodes++;
			hasColors = true;
			input->QueryIntAttribute("offset", &queryResult);
			offsetColors = queryResult;
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
			positions.push_back(Vector3f(x, y, z));

			for (unsigned int index = 1; index < pos_count; index++) {
				x = atof(strtok(NULL, " ")); y = atof(strtok(NULL, " ")); z = atof(strtok(NULL, " "));
				positions.push_back(Vector3f(x, y, z));
			}

			hasPositions = false;
			source = source->NextSiblingElement("source");
			continue;

		}else if (hasNormals) {
			floatArray = source->FirstChildElement("float_array");
			text = (char*)(floatArray->GetText());
			floatArray->QueryIntAttribute("count", &norm_count);
			norm_count = norm_count / 3;

			float x = atof(strtok(text, " "));  float y = atof(strtok(NULL, " ")); float z = atof(strtok(NULL, " "));
			normals.push_back(Vector3f(x, y, z));

			for (unsigned int index = 1; index < norm_count; index++) {
				x = atof(strtok(NULL, " ")); y = atof(strtok(NULL, " ")); z = atof(strtok(NULL, " "));
				normals.push_back(Vector3f(x, y, z));
			}

			hasNormals = false;
			source = source->NextSiblingElement("source");
			continue;

		}else if (hasTexCoords) {
			floatArray = source->FirstChildElement("float_array");
			text = (char*)(floatArray->GetText());
			floatArray->QueryIntAttribute("count", &tex_count);
			tex_count = tex_count / 2;

			float u = atof(strtok(text, " "));  float v = atof(strtok(NULL, " "));
			texCoords.push_back(Vector2f(u, v));

			for (unsigned int index = 1; index < tex_count; index++) {
				u = atof(strtok(NULL, " ")); v = atof(strtok(NULL, " "));
				texCoords.push_back(Vector2f(u, v));
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
}

void ColladaLoader::loadController(std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<std::string> &jointsList, std::vector<VertexSkinData> &skinningData) {


	char* text;
	TiXmlElement* controller = doc.RootElement()->FirstChildElement("library_controllers")->FirstChildElement("controller");
	TiXmlElement* skin = controller->FirstChildElement("skin");

	TiXmlElement* source = skin->FirstChildElement("source");

	TiXmlElement* joints = skin->FirstChildElement("joints");
	TiXmlElement* input = joints->FirstChildElement("input");

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

	int pointer = 0;
	for (int i = 0; i < effectiveJointCounts.size(); i++) {
		VertexSkinData skinData;
		for (int j = 0; j < effectiveJointCounts[i]; j++) {
			int jointId = rawData[pointer++];
			int weightId = rawData[pointer++];

			skinData.addJointEffect(jointId, weights[weightId]);
		}
		skinData.fill();
		skinData.limitJointNumber(maxWeights);

		skinningData.push_back(skinData);
	}	
}

void ColladaLoader::createIndexBuffer(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<unsigned int> &indices, std::vector<VertexSkinData> &skinningData) {

	m_indexBuffer.resize(numberOfTriangles * 3);

	short offset = inputNodes, vertexstride = 3 * inputNodes;

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

	positions.clear();
	normals.clear();
	texCoords.clear();
	jointIds.clear();
	jointWeights.clear();
	indices.clear();

	for (int i = 0; i < m_vertexBuffer.size(); i = i + 14) {

		Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
		Vector3f normal = Vector3f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4], m_vertexBuffer[i + 5]);
		Vector2f texCoord = Vector2f(m_vertexBuffer[i + 6], 1.0 - m_vertexBuffer[i + 7]);
		std::array<unsigned int, 4> jointId = { m_vertexBuffer[i + 8], m_vertexBuffer[i + 9], m_vertexBuffer[i + 10], 0 };
		Vector4f jointWeight = Vector4f(m_vertexBuffer[i + 11], m_vertexBuffer[i + 12], m_vertexBuffer[i + 13], 0.0);

		positions.push_back(position);
		texCoords.push_back(texCoord);
		normals.push_back(normal);
		jointIds.push_back(jointId);
		jointWeights.push_back(jointWeight);

	}
	indices = m_indexBuffer;
}

int ColladaLoader::addVertex(int hash, const float *pVertex, int numberOfBytes) {

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
			pCachedVertex = &m_vertexBuffer[index * numberOfBytes];

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


void ColladaLoader::loadVisualScene(std::vector<std::string> &jointsList) {
	TiXmlElement* rootNode = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node");
	while (rootNode != NULL) {

		if (std::string(rootNode->Attribute("id")).compare("Armature") == 0 || std::string(rootNode->Attribute("id")).compare("metarig") == 0) {
			break;
		}
		rootNode = rootNode->NextSiblingElement("node");
	}
	rootNode = rootNode->FirstChildElement("node");

	char* text = (char*)rootNode->FirstChild()->FirstChild()->Value();

	Matrix4f offsetMatrix;
	offsetMatrix[0][0] = atof(strtok(text, " "));

	short start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {
			offsetMatrix[i][j] = atof(strtok(NULL, " "));
		}
		start = 0;
	}

	Matrix4f::Transpose(offsetMatrix);

	std::string name = std::string(((TiXmlElement*)rootNode)->Attribute("sid"));
	std::vector<std::string>::iterator it = std::find(jointsList.begin(), jointsList.end(), name);

	skeletonData.headJoint = JointData(std::distance(jointsList.begin(), it), std::string(((TiXmlElement*)rootNode)->Attribute("id")), offsetMatrix);

	//breadth - first search
	TiXmlNode* nextNode = rootNode->FirstChildElement("node");
	while (nextNode != NULL) {
		skeletonData.headJoint.addChild(search(nextNode, jointsList));
		nextNode = nextNode->NextSiblingElement("node");
	}
	skeletonData.jointCount = jointsList.size();
}

JointData ColladaLoader::search(TiXmlNode* _node, std::vector<std::string> &jointsList) {
	JointData data;

	char* text = (char*)_node->FirstChild()->FirstChild()->Value();

	Matrix4f offsetMatrix;
	offsetMatrix[0][0] = atof(strtok(text, " "));

	short start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {
			offsetMatrix[i][j] = atof(strtok(NULL, " "));
		}
		start = 0;
	}

	Matrix4f::Transpose(offsetMatrix);

	std::string name = ((TiXmlElement*)_node)->Attribute("sid");
	std::vector<std::string>::iterator it = std::find(jointsList.begin(), jointsList.end(), name);
	data = JointData(std::distance(jointsList.begin(), it), ((TiXmlElement*)_node)->Attribute("id"), offsetMatrix);

	TiXmlNode* nextNode = _node->FirstChildElement("node");
	while (nextNode) {
		data.addChild(search(nextNode, jointsList));
		nextNode = nextNode->NextSiblingElement("node");
	}
	return data;
}

void ColladaLoader::createJoints(Joint &joint) {
	
	joint.index = skeletonData.headJoint.index;
	joint.name = skeletonData.headJoint.nameId;
	joint.localBindTransform = skeletonData.headJoint.bindLocalTransform;

	Matrix4f bindTransform = skeletonData.headJoint.bindLocalTransform;
	joint.inverseBindTransform = bindTransform.inverse();

	for (JointData child : skeletonData.headJoint.children) {
		joint.children.push_back(createJoints(child, bindTransform));
	}
}

Joint ColladaLoader::createJoints(JointData data, Matrix4f parentBindTransform2) {

	Joint joint;

	joint.index = data.index;
	joint.name = data.nameId;
	joint.localBindTransform = data.bindLocalTransform;

	Matrix4f bindTransform = parentBindTransform2 * data.bindLocalTransform;
	joint.inverseBindTransform = bindTransform.inverse();

	for (JointData child : data.children) {
		joint.children.push_back(createJoints(child, bindTransform));
	}
	return joint;
}

void ColladaLoader::loadAnimation(std::vector<KeyFrameData> &m_keyFrames, float &duration, std::string &name) {
	TiXmlElement* animation = doc.RootElement()->FirstChildElement("library_animations")->FirstChildElement("animation")->FirstChildElement("animation");

	TiXmlElement* source;
	TiXmlElement* floatArray;
	char* text;
	int numberOfKeyframes;

	std::vector<float> times;

	while (animation != NULL) {

		source = animation->FirstChildElement("source");
		floatArray = source->FirstChildElement("float_array");
		floatArray->QueryIntAttribute("count", &numberOfKeyframes);

		text = (char*)(floatArray->GetText());
		times.push_back(atof(strtok(text, " ")));

		for (int k = 1; k < numberOfKeyframes; k++) {
			times.push_back(atof(strtok(NULL, " ")));
		}

		source = source->NextSiblingElement("source");
		floatArray = source->FirstChildElement("float_array");

		text = (char*)(floatArray->GetText());

		source = source->NextSiblingElement("channel");
		std::string target = std::string(source->Attribute("target"));
		std::string jointNameId = target.substr(0, target.find('/'));


		m_keyFrames.resize(numberOfKeyframes);

		Matrix4f  matrix;
		matrix[0][0] = atof(strtok(text, " "));
		short start = 1;

		for (int k = 0; k < numberOfKeyframes; k++) {
			for (int i = 0; i < 4; i++) {
				for (int j = start; j < 4; j++) {
					matrix[i][j] = atof(strtok(NULL, " "));
				}
				start = 0;
			}

			Matrix4f::Transpose(matrix);

			/*Matrix4f CORRECTION;
			CORRECTION.rotate( Vector3f(1.0f, 0.0f, 0.0f), - 90.0f);

			matrix = CORRECTION * matrix;*/

			m_keyFrames[k].time = times[k];

			Vector3f position = Vector3f(matrix[3][0], matrix[3][1], matrix[3][2]);

			float sx = Vector3f(matrix[0][0], matrix[1][0], matrix[2][0]).length();
			float sy = Vector3f(matrix[0][1], matrix[1][1], matrix[2][1]).length();
			float sz = Vector3f(matrix[0][2], matrix[1][2], matrix[2][2]).length();

			Vector3f scale = Vector3f(sx, sy, sz);

			//*matrix[0][0] = matrix[0][0] * 1.0 / (scale[0]); matrix[0][1] = matrix[0][1] * 1.0 / (scale[0]); matrix[0][2] = matrix[0][2] * 1.0 / (scale[0]); matrix[3][0] = 0.0;
			//*matrix[1][0] = matrix[1][0] * 1.0 / (scale[1]); matrix[1][1] = matrix[1][1] * 1.0 / (scale[1]); matrix[1][2] = matrix[1][2] * 1.0 / (scale[1]); matrix[3][1] = 0.0;
			//*matrix[2][0] = matrix[2][0] * 1.0 / (scale[2]); matrix[2][1] = matrix[2][1] * 1.0 / (scale[2]); matrix[2][2] = matrix[2][2] * 1.0 / (scale[2]); matrix[3][2] = 0.0;
			//*matrix[0][3] = 0.0;								matrix[1][3] = 0.0;								matrix[2][3] = 0.0;								matrix[3][3] = 1.0;

		
			m_keyFrames[k].pose.insert(std::pair<std::string, JointTransformData>(jointNameId, JointTransformData(jointNameId, position, Quaternion(matrix), scale)));
		}
		animation = animation->NextSiblingElement("animation");
	}
	duration = times[numberOfKeyframes - 1];
	name = "";
}