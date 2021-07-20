# include "ColladaLoader.h"
# include "..\AnimatedModel\AnimatedMesh.h"
# include "..\Animator\Animation.h"

ColladaLoader::ColladaLoader(const std::string & path) : m_path(path) {
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
	doc.Clear();
}

void ColladaLoader::loadData(std::vector<Vector3f> &positions,
							 std::vector<Vector2f> &texCoords,
							 std::vector<Vector3f> &normals,							
							 std::vector<unsigned int> &indices, 							
							 std::vector<std::array<unsigned int, 4>> &jointIds,
							 std::vector<Vector4f> &jointWeights,
							 std::vector<std::string> &jointsList) {

	std::vector<VertexSkinData> skinningData;
	std::string geometryName;
	//geometry | id -> controller | skin source -> controller | id -> visualScene | instance_controller url -> visualScene | Armature name -> animation name

	loadGeometry(positions, texCoords, normals, indices, geometryName);
	loadController(jointIds, jointWeights, jointsList, skinningData);
	createIndexBuffer(positions, texCoords, normals, indices, jointIds, jointWeights, skinningData);
	
	TiXmlNode* node = findRoot(doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene"), "instance_controller", "url", "#Armature_mushroom-skin");

	//TiXmlNode* node = findRoot("instance_controller", "#Armature_mushroom-skin");
	if (!node) {
		node = findRoot(doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene"), "instance_geometry", "url", "#Cube-mesh");

		
	}

	//if (node) std::cout << ((TiXmlElement*)node)->Attribute("id") << std::endl;
	//std::cout << geometryName;
	//fill up the 'SkeletonData: skeletonData' structure. Needed to create the 'Joint: rootJoint' structure out of "AnimatedMesh.h"
	loadVisualScene(jointsList);

	/** free the memory of temporarily vector **/
	skinningData.clear();
	skinningData.shrink_to_fit();

	/** alternative **/
	//std::vector<VertexSkinData>().swap(skinningData);
}

void ColladaLoader::loadGeometryAndController(std::map<std::string, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>> &_geometry) {
	doc.LoadFile();
	

	TiXmlElement* geometryLib = doc.RootElement()->FirstChildElement("library_geometries");

	TiXmlElement* geometry = geometryLib->FirstChildElement("geometry");
	while (geometry != NULL) {

		inputNodes = 0;offsetPositions = 0;offsetNormals = 0;offsetTexcoords = 0;offsetColors = 0; numberOfTriangles = 0;
		prim_count = 0;num_indices = 0;pos_count = 0;norm_count = 0;tex_count = 0;
		hasPositions = false;hasNormals = false;hasTexCoords = false;hasColors = false;

		std::string name = std::string(geometry->Attribute("id"));

		std::vector<Vector3f> positions;
		std::vector<Vector3f> normals;
		std::vector<Vector2f> texCoords;
		std::vector<unsigned int> indices;

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

			}
			else if (hasNormals) {
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

			}
			else if (hasTexCoords) {
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

		/*std::vector<std::array<unsigned int, 4>>  jointIds;
		std::vector<Vector4f> jointWeights;
		std::vector<std::string> jointsList;
		std::vector<VertexSkinData> skinningData;
		TiXmlElement* controllerLib = doc.RootElement()->FirstChildElement("library_controllers");
		if (controllerLib) {
			TiXmlElement* controller = controllerLib->FirstChildElement("controller");
			while (controller != NULL) {

				TiXmlNode* node = findRoot(doc.RootElement()->FirstChildElement("library_controllers"), "skin", "source", "#" + name);
				if (node) {
					name = "C" + std::string(((TiXmlElement*)node)->Attribute("name"));
					
					
					loadController(controller, jointIds, jointWeights, jointsList, skinningData);
				}



				controller = controller->NextSiblingElement("controller");


			}
		}*/

		_geometry.insert(std::pair<std::string, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>>(name, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>(positions, normals, texCoords, indices, inputNodes)));
		geometry = geometry->NextSiblingElement("geometry");
	}
	
}

void ColladaLoader::loadGeometry(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<unsigned int> &indices, std::string &geometryName) {
	
	TiXmlElement* geometry = doc.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");
	geometryName = geometry->Attribute("id");

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

bool ColladaLoader::loadController(std::string& name, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<std::string> &jointsList, std::vector<VertexSkinData> &skinningData) {
	
	char* text;
	TiXmlElement* controller = doc.RootElement()->FirstChildElement("library_controllers");
	if (controller == NULL) return false;

	controller = controller->FirstChildElement("controller");

	while (controller != NULL) {

		TiXmlElement* skin = controller->FirstChildElement("skin");
		if (std::string(skin->Attribute("source")).compare("#" + name) == 0) {
			name = std::string(controller->Attribute("id"));
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
			return true;
		}
		controller = controller->NextSiblingElement("controller");
	}

	return false;
}

bool ColladaLoader::loadController(std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<std::string> &jointsList, std::vector<VertexSkinData> &skinningData) {


	char* text;
	TiXmlElement* controller = doc.RootElement()->FirstChildElement("library_controllers");

	if (controller == NULL) return false;

	controller = controller->FirstChildElement("controller");

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

	return true;
}

void ColladaLoader::createIndexBuffer(short vertexLayout, std::vector<Vector3f> &positions, std::vector<Vector3f> &normals, std::vector<Vector2f> &texCoords, std::vector<unsigned int> &indices, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<VertexSkinData> &skinningData) {
	IndexBufferCreator indexBufferCreator;

	indexBufferCreator.numberOfTriangles = indices.size() / (vertexLayout * 3);
	indexBufferCreator.offsetPositions = 0;
	indexBufferCreator.offsetNormals = 1;
	indexBufferCreator.offsetTexcoords = 2;
	indexBufferCreator.inputNodes = vertexLayout;
	indexBufferCreator.positionsInOut = positions;
	indexBufferCreator.normalsInOut = normals;
	indexBufferCreator.texCoordsInOut = texCoords;
	indexBufferCreator.indexBufferIn = indices;
	indexBufferCreator.skinningDataIn = skinningData;

	indexBufferCreator.createIndexBuffer();


	positions = indexBufferCreator.positionsInOut;
	normals = indexBufferCreator.normalsInOut;
	texCoords = indexBufferCreator.texCoordsInOut;
	jointWeights = indexBufferCreator.jointWeightsOut;
	jointIds = indexBufferCreator.jointIdsOut;
	indices = indexBufferCreator.indexBufferOut;

}


void ColladaLoader::createIndexBuffer(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<unsigned int> &indices, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<VertexSkinData> &skinningData){

	IndexBufferCreator indexBufferCreator;

	indexBufferCreator.numberOfTriangles = indices.size() / (inputNodes  * 3);
	indexBufferCreator.offsetPositions = 0;
	indexBufferCreator.offsetNormals = 1;
	indexBufferCreator.offsetTexcoords = 2;
	indexBufferCreator.inputNodes = inputNodes;
	indexBufferCreator.positionsInOut = positions;
	indexBufferCreator.normalsInOut = normals;
	indexBufferCreator.texCoordsInOut = texCoords;
	indexBufferCreator.indexBufferIn = indices;
	indexBufferCreator.skinningDataIn = skinningData;

	indexBufferCreator.createIndexBuffer();

	
	positions = indexBufferCreator.positionsInOut;
	normals = indexBufferCreator.normalsInOut;
	texCoords = indexBufferCreator.texCoordsInOut;
	jointWeights = indexBufferCreator.jointWeightsOut;
	jointIds = indexBufferCreator.jointIdsOut;
	indices = indexBufferCreator.indexBufferOut;
}

bool ColladaLoader::search(TiXmlNode* rootNode, std::string searchNode, std::string attribute, std::string value) {
	TiXmlNode* nextNode = rootNode->FirstChildElement();
	while (nextNode) {
		
		if (search(nextNode, searchNode, attribute, value)) {
			return true;
		}
	
		if ( (nextNode && std::string(((TiXmlElement*)nextNode)->Value()).compare(searchNode) == 0)
			  && ((TiXmlElement*)nextNode)->Attribute(attribute.c_str()) && std::string(((TiXmlElement*)nextNode)->Attribute(attribute.c_str())).compare(value) == 0) {
			return true;
		}

		nextNode = nextNode->NextSiblingElement();
	}
	return false;	
}

TiXmlNode* ColladaLoader::findRoot(TiXmlElement* rootNode, std::string searchNode, std::string attribute, std::string value) {
	TiXmlNode* nextNode = rootNode->FirstChildElement();

	while (nextNode != NULL) {
		if (search(nextNode, searchNode, attribute, value))
			break;
		nextNode = nextNode->NextSiblingElement();
	}
	return nextNode;
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

	Matrix4f::transpose(offsetMatrix);

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

void ColladaLoader::loadVisualScene(std::string _name, std::vector<std::string> jointsList, SkeletonData &_skeletonData, Matrix4f &modelMatrix) {
	TiXmlNode* libScene = doc.RootElement()->FirstChildElement("library_visual_scenes");

	TiXmlNode* rootNode;
	if (libScene) {
		rootNode = findRoot(doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene"), "instance_controller", "url", "#" + _name);
		if (!rootNode) {
			rootNode = findRoot(doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene"), "instance_geometry", "url", "#" + _name);
		}
	}
	if (!rootNode) return;
	//////////////get the model matrix/////////////////////////
	char* text = (char*)rootNode->FirstChild()->FirstChild()->Value();
	modelMatrix[0][0] = atof(strtok(text, " "));
	short start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {
			modelMatrix[i][j] = atof(strtok(NULL, " "));
		}
		start = 0;
	}
	Matrix4f::transpose(modelMatrix);
	///////////////////////////////////////////////////////////
	rootNode = rootNode->FirstChildElement("node");
	if (!rootNode) return;
	text = (char*)rootNode->FirstChild()->FirstChild()->Value();


	Matrix4f offsetMatrix;
	offsetMatrix[0][0] = atof(strtok(text, " "));

	start = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = start; j < 4; j++) {
			offsetMatrix[i][j] = atof(strtok(NULL, " "));
		}
		start = 0;
	}

	Matrix4f::transpose(offsetMatrix);

	std::string name = std::string(((TiXmlElement*)rootNode)->Attribute("sid"));
	std::vector<std::string>::iterator it = std::find(jointsList.begin(), jointsList.end(), name);

	_skeletonData.headJoint = JointData(std::distance(jointsList.begin(), it), std::string(((TiXmlElement*)rootNode)->Attribute("id")), offsetMatrix);

	//breadth - first search
	TiXmlNode* nextNode = rootNode->FirstChildElement("node");
	while (nextNode != NULL) {
		_skeletonData.headJoint.addChild(search(nextNode, jointsList));
		nextNode = nextNode->NextSiblingElement("node");
	}
	_skeletonData.jointCount = jointsList.size();
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

	Matrix4f::transpose(offsetMatrix);

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

void ColladaLoader::createJoints(SkeletonData _skeletonData, Joint &joint) {

	joint.index = _skeletonData.headJoint.index;
	joint.name = _skeletonData.headJoint.nameId;
	joint.localBindTransform = _skeletonData.headJoint.bindLocalTransform;

	Matrix4f bindTransform = _skeletonData.headJoint.bindLocalTransform;
	joint.inverseBindTransform = bindTransform.inverse();

	for (JointData child : _skeletonData.headJoint.children) {
		joint.children.push_back(createJoints(child, bindTransform));
	}
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
	TiXmlElement* animation = doc.RootElement()->FirstChildElement("library_animations")->FirstChildElement("animation");
	name = std::string(animation->Attribute("name"));

	animation = animation->FirstChildElement("animation");
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

			Matrix4f::transpose(matrix);
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
}

/*void ColladaLoader::loadAnimation(Animation &animation) {


}*/

/////////////////////////////////////////////////////////////////////////////////////////
void IndexBufferCreator::createIndexBuffer() {

	indexBufferOut.resize(numberOfTriangles * 3);

	short vertexstride = 3 * inputNodes, stride;
	if (!skinningDataIn.empty()){
		stride = 14;
		for (int i = 0; i < numberOfTriangles; i++) {

			float vertex1[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][0]    , positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][1]    , positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][1]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 0]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 0]][1],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].jointIds[0], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].jointIds[1], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].jointIds[2],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].weights[0] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].weights[1] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]].weights[2]
			};

			indexBufferOut[i * 3] = addVertex(indexBufferIn[i * 3], &vertex1[0], stride);

			float vertex2[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][0], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][1], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][1]   , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 1]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 1]][1],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].jointIds[0], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].jointIds[1], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].jointIds[2],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].weights[0] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].weights[1] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]].weights[2]
			};

			indexBufferOut[i * 3 + 1] = addVertex(indexBufferIn[i * 3 + 3], &vertex2[0], stride);

			float vertex3[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][0], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][1], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 2]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 2]][1]  , normalsInOut[indexBufferIn[i * 3 + offsetNormals + inputNodes * 2]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 2]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 2]][1],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].jointIds[0], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].jointIds[1], skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].jointIds[2],
				skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].weights[0] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].weights[1] , skinningDataIn[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]].weights[2]
			};

			indexBufferOut[i * 3 + 2] = addVertex(indexBufferIn[i * 3 + 6], &vertex3[0], stride);
		}
	}else {
		stride = 8;
		for (int i = 0; i < numberOfTriangles; i++) {

			float vertex1[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][0]    , positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][1]    , positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 0]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][1]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 0]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 0]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 0]][1]
			};

			indexBufferOut[i * 3] = addVertex(indexBufferIn[i * 3], &vertex1[0], stride);

			float vertex2[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][0], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][1], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 1]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][1]   , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 1]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 1]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 1]][1]
			};

			indexBufferOut[i * 3 + 1] = addVertex(indexBufferIn[i * 3 + 3], &vertex2[0], stride);

			float vertex3[] = { positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][0], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][1], positionsInOut[indexBufferIn[i * vertexstride + offsetPositions + inputNodes * 2]][2],
				normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 2]][0]  , normalsInOut[indexBufferIn[i * vertexstride + offsetNormals + inputNodes * 2]][1]  , normalsInOut[indexBufferIn[i * 3 + offsetNormals + inputNodes * 2]][2],
				texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 2]][0], texCoordsInOut[indexBufferIn[i * vertexstride + offsetTexcoords + inputNodes * 2]][1]
			};

			indexBufferOut[i * 3 + 2] = addVertex(indexBufferIn[i * 3 + 6], &vertex3[0], stride);
		}

	}



	std::map<int, std::vector<int>>().swap(m_vertexCache);

	positionsInOut.clear();
	positionsInOut.shrink_to_fit();
	normalsInOut.clear();
	normalsInOut.shrink_to_fit();
	texCoordsInOut.clear();
	texCoordsInOut.shrink_to_fit();
	jointIdsOut.clear();
	jointIdsOut.shrink_to_fit();
	jointWeightsOut.clear();
	jointWeightsOut.shrink_to_fit();
	indexBufferIn.clear();
	indexBufferIn.shrink_to_fit();

	for (int i = 0; i < vertexBuffer.size(); i = i + stride) {

		Vector3f position = Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]);
		Vector3f normal = Vector3f(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]);
		Vector2f texCoord = Vector2f(vertexBuffer[i + 6], 1.0 - vertexBuffer[i + 7]);
		
		positionsInOut.push_back(position);
		texCoordsInOut.push_back(texCoord);
		normalsInOut.push_back(normal);

		if (!skinningDataIn.empty()) {
			std::array<unsigned int, 4> jointId = { vertexBuffer[i + 8], vertexBuffer[i + 9], vertexBuffer[i + 10], 0 };
			Vector4f jointWeight = Vector4f(vertexBuffer[i + 11], vertexBuffer[i + 12], vertexBuffer[i + 13], 0.0);
			jointIdsOut.push_back(jointId);
			jointWeightsOut.push_back(jointWeight);
		}
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
}

int IndexBufferCreator::addVertex(int hash, const float *pVertex, int numberOfBytes) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBuffer.size() / numberOfBytes);
		vertexBuffer.insert(vertexBuffer.end(), pVertex, pVertex + numberOfBytes);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBuffer[index * numberOfBytes];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* numberOfBytes) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBuffer.size() / numberOfBytes);
			vertexBuffer.insert(vertexBuffer.end(), pVertex, pVertex + numberOfBytes);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}