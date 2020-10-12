#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <map>

#include "..\Extension.h"
#include "..\tinyxml\tinyxml.h"


struct S2 {
	int jointId;
	float weight;


	S2(int jointId, float weight) : jointId(jointId), weight(weight) {}

	bool operator<(const struct S2& other) const {
		return weight < other.weight;
	}
};

struct VertexSkinData2 {
public:
	void addJointEffect(int jointId, float weight) {
		pq.push(S2(jointId, weight));
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

	std::priority_queue<S2> pq;
	std::vector<int> jointIds;
	std::vector<float> weights;

private:

	void fillEmptyWeights(int max) {
		
		while (jointIds.size() < max) {
			jointIds.push_back(0);
			weights.push_back(0.0);
		}
	}
};

struct BoneData
{
	glm::mat4 offsetMatrix;
	glm::mat4 finalTransformation;
};

struct JointData {
	int index;
	std::string nameId;
	glm::mat4 bindLocalTransform;
	std::vector<JointData> children;

	JointData(int _index, std::string _nameId, glm::mat4 _bindLocalTransform) {
		index = _index;
		nameId = _nameId;
		bindLocalTransform = _bindLocalTransform;
	}

	JointData(){}

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

	SkeletonData(){}
};

struct Joint3 {
	int index;
	std::string name;
	std::vector<Joint3> children;

	glm::mat4 animatedTransform;

	glm::mat4 localBindTransform;
	glm::mat4 inverseBindTransform;

	Joint3(int _index, std::string _name, glm::mat4 _bindLocalTransform) {
		index = _index;
		name = _name;
		localBindTransform = _bindLocalTransform;
		
	}

	Joint3() {}

	void addChild(Joint3 child) {
		children.push_back(child);

	}

	void calcInverseBindTransform(glm::mat4 parentBindTransform) {

		glm::mat4 bindTransform = parentBindTransform * localBindTransform;
		inverseBindTransform = glm::inverse(bindTransform);

		for (int i = 0; i < children.size(); i++) {
			children[i].calcInverseBindTransform(bindTransform);
		}
	}
};

class AnimatedMesh {

public:
	AnimatedMesh(const std::string & _path);
	virtual ~AnimatedMesh();

	void Draw();

	std::vector<glm::mat4> GetBoneArray();

	std::vector<glm::mat4> GetBoneArray2();
	void addJointsToArray(Joint3 joint, std::vector<glm::mat4> &boneArray);

	void applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose);
	void applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose, Joint3 &joint, glm::mat4 parentTransform);


	std::unordered_map<std::string, unsigned int> _boneIdMap;
	std::unordered_map<unsigned int, BoneData> _boneDataMap;

	SkeletonData skeletonData;

	Joint3 rootJoint;

	std::vector<std::string> jointsList;

	void loadData(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &texCoords, std::vector<glm::vec3> &normals, std::vector<glm::uvec4> &jointIds, std::vector<glm::vec4> &jointWeights, std::vector<unsigned int> &indices);
	void LoadJointsData(std::unordered_map<std::string, unsigned int> &boneIdMap, std::unordered_map<unsigned int, BoneData> &boneDataMap);
	void LoadJointsData2();
	void search(TiXmlNode* node, std::string jointName, int index, std::unordered_map<std::string, unsigned int> &boneIdMap, std::unordered_map<unsigned int, BoneData> &boneDataMap);
	JointData AnimatedMesh::search(TiXmlNode* node);

	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int numberOfBytes);

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	Joint3 createJoints(JointData data);

	


private:

	enum MeshBufferPositions {
		POSITION,
		TEX_COORD,
		NORMAL,
		JOINT_IDS,
		JOINT_WEIGHTS,
		INDEX,
		NUM_BUFFERS
	};

	unsigned int _vao;
	unsigned int _vbo[NUM_BUFFERS];
	unsigned int _drawCount;
	const std::string& path;
	const glm::mat4 identity = glm::mat4(1.0f);
};
#endif