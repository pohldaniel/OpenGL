#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include <unordered_map>
#include <queue>
#include <iostream>
#include <map>

#include "..\..\Extension.h"
#include "..\..\Vector.h"
#include "..\ColladaLoader\ColladaLoader.h"

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

	void draw();
	std::vector<Matrix4f> getBoneArray();
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

	unsigned int m_vao;
	unsigned int m_vbo[NUM_BUFFERS];
	unsigned int m_drawCount;

	std::unordered_map<std::string, unsigned int> m_boneIdMap;
	std::vector<std::string> m_jointsList;

	Joint rootJoint;
	ColladaLoader loader;
};
#endif