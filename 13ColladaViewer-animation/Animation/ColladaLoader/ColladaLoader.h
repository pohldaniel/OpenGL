#pragma once
#include <unordered_map>
#include <queue>
#include <iostream>
#include <map>
#include <array>

#include "..\..\Third\tinyxml\tinyxml.h"
#include"..\..\Vector.h"

struct VertexSkinData;
struct IndexBufferCreator {
	short inputNodes, offsetPositions, offsetNormals, offsetTexcoords;
	unsigned int numberOfTriangles;

	std::vector<VertexSkinData> skinningDataIn;
	std::vector<unsigned int> indexBufferIn;

	std::vector<Vector3f> positionsInOut;
	std::vector<Vector3f> normalsInOut;
	std::vector<Vector2f> texCoordsInOut;
	
	std::vector<std::array<unsigned int, 4>> jointIdsOut;
	std::vector<Vector4f> jointWeightsOut;

	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer();

private:
	std::map<int, std::vector<int>> m_vertexCache;
	std::vector <float> vertexBuffer;
	int addVertex(int hash, const float *pVertex, int stride);
};

struct WeightData {
	int jointId;
	float weight;


	WeightData(int jointId, float weight) : jointId(jointId), weight(weight) {}

	bool operator<(const struct WeightData& other) const {
		return weight < other.weight;
	}
};

struct VertexSkinData {
public:
	void addJointEffect(int jointId, float weight) {
		pq.push(WeightData(jointId, weight));
	}

	void fill() {
		while (!pq.empty()) {
			jointIds.push_back(pq.top().jointId);
			weights.push_back(pq.top().weight);
			pq.pop();
		}
	}

	void limitJointNumber(int max) {
		fillEmptyWeights(max);
	}
	
	void fillEmptyWeights(int max) {

		while (jointIds.size() < max) {
			jointIds.push_back(0);
			weights.push_back(0.0);
		}
	}

	std::priority_queue<WeightData> pq;
	std::vector<int> jointIds;
	std::vector<float> weights;
};

struct JointData {
	int index;
	std::string nameId;
	Matrix4f bindLocalTransform;
	std::vector<JointData> children;

	JointData(int _index, std::string _nameId, Matrix4f _bindLocalTransform) {
		index = _index;
		nameId = _nameId;
		bindLocalTransform = _bindLocalTransform;
	}

	JointData() {}

	void addChild(JointData child) {
		children.push_back(child);
	}
};

struct SkeletonData {

	int jointCount;
	JointData headJoint;

	SkeletonData(int _jointCount, JointData _headJoint) {
		jointCount = _jointCount;
		headJoint = _headJoint;
	}

	SkeletonData() {}
};

struct Joint;
struct KeyFrameData;

class ColladaLoader {

public:
	ColladaLoader(const std::string & _path);
	~ColladaLoader();

	void loadData(std::vector<Vector3f> &positions,
				  std::vector<Vector2f> &texCoords,
				  std::vector<Vector3f> &normals,
				  std::vector<unsigned int> &indices,
				  std::vector<std::array<unsigned int, 4>> &jointIds,
				  std::vector<Vector4f> &jointWeights,
				  std::vector<std::string> &jointsList);

	void loadVisualScene(std::vector<std::string> &jointsList);
	void createJoints(Joint &joint);

	void createJoints(SkeletonData skeletonData, Joint &joint);

	void loadAnimation(std::vector<KeyFrameData> &m_keyFrames, float &duration, std::string &name);
	//void loadAnimation(Animation &animation);
	TiXmlDocument doc;

	
	void loadGeometryAndController(std::map<std::string, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>> &geometry);
	bool loadController(std::string& name, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<std::string> &jointsList, std::vector<VertexSkinData> &skinningData);
	void createIndexBuffer(short vertexLayout, std::vector<Vector3f> &positions, std::vector<Vector3f> &normals, std::vector<Vector2f> &texCoords, std::vector<unsigned int> &indices, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<VertexSkinData> &skinningData);
	void loadVisualScene(std::string name, std::vector<std::string> jointsList, SkeletonData &skeletonData, Matrix4f &modelMatrix);

private:
	const std::string& m_path;
	

	

	void loadGeometry(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<unsigned int> &indices, std::string &geometryName);
	bool loadController(std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<std::string> &jointsList, std::vector<VertexSkinData> &skinningData);
	
	void createIndexBuffer(std::vector<Vector3f> &positions, std::vector<Vector2f> &texCoords, std::vector<Vector3f> &normals, std::vector<unsigned int> &indices, std::vector<std::array<unsigned int, 4>> &jointIds, std::vector<Vector4f> &jointWeights, std::vector<VertexSkinData> &skinningData);
	TiXmlNode* findRoot(TiXmlElement* rootNode, std::string node, std::string attribute, std::string value);
	bool search(TiXmlNode* _node, std::string _search, std::string attribute, std::string value);
	JointData search(TiXmlNode* node, std::vector<std::string> &jointsList);
	Joint createJoints(JointData data, Matrix4f parentBindTransform);

	SkeletonData skeletonData;

	short inputNodes, offsetPositions, offsetNormals, offsetTexcoords, offsetColors;
	int prim_count, num_indices, pos_count, norm_count, tex_count, numberOfTriangles;
	bool hasPositions, hasNormals, hasTexCoords, hasColors;
};