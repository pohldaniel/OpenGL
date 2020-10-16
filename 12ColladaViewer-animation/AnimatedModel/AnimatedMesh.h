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

#include "..\tinyxml\tinyxml.h"

#include "..\Extension.h"
#include "..\ColladaLoader\ColladaLoader.h"

struct Joint {
	int index;
	std::string name;
	std::vector<Joint> children;

	glm::mat4 localBindTransform;
	glm::mat4 inverseBindTransform;	

	glm::mat4 animatedTransform;
};

class AnimatedMesh {

public:
	AnimatedMesh(const std::string & _path);
	virtual ~AnimatedMesh();

	void Draw();
	std::vector<glm::mat4> GetBoneArray();
	void applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose);

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

	void addJointsToArray(Joint joint, std::vector<glm::mat4> &boneArray);
	void applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose, Joint &joint, glm::mat4 parentTransform);

	unsigned int _vao;
	unsigned int _vbo[NUM_BUFFERS];
	unsigned int _drawCount;
	const std::string& path;
	const glm::mat4 identity = glm::mat4(1.0f);
	std::unordered_map<std::string, unsigned int> _boneIdMap;
	std::vector<std::string> jointsList;

	Joint rootJoint;

};
#endif