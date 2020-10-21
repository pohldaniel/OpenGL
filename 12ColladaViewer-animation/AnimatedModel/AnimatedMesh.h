#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include <unordered_map>
#include <queue>
#include <iostream>
#include <map>

#include "..\tinyxml\tinyxml.h"

#include "..\Extension.h"
#include "..\ColladaLoader\ColladaLoader.h"
#include "..\Vector.h"

struct Joint {
	int index;
	std::string name;
	std::vector<Joint> children;

	Matrix4f localBindTransform;
	Matrix4f inverseBindTransform;

	Matrix4f animatedTransform;
};

class AnimatedMesh {

public:
	AnimatedMesh(ColladaLoader loader);
	virtual ~AnimatedMesh();

	void Draw();
	std::vector<Matrix4f> GetBoneArray();

	void applyPoseToJoints(std::unordered_map<std::string, Matrix4f> currentPose);
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

	void addJointsToArray(Joint rootJoint, std::vector<Matrix4f> &boneArray);
	void applyPoseToJoints(std::unordered_map<std::string, Matrix4f> currentPose, Joint &joint, Matrix4f parentTransform);

	unsigned int _vao;
	unsigned int _vbo[NUM_BUFFERS];
	unsigned int _drawCount;
	const std::string& path;
	std::unordered_map<std::string, unsigned int> _boneIdMap;
	std::vector<std::string> jointsList;

	Joint rootJoint;
	ColladaLoader loader;
};
#endif