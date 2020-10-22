
#include <vector>
#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(ColladaLoader loader) : loader(loader) {
	
	std::vector<Vector3f> positions;
	std::vector<Vector2f> texCoords;
	std::vector<Vector3f> normals;
	std::vector<std::array<unsigned int, 4>> jointIds;
	std::vector<Vector4f> jointWeights;
	std::vector<unsigned int> indices;

	loader.loadData(positions, texCoords, normals, jointIds, jointWeights, indices, m_jointsList);
	loader.createJoints(rootJoint);

	m_drawCount = indices.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(NUM_BUFFERS, m_vbo);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[TEX_COORD]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Joint Ids
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[JOINT_IDS]);
	glBufferData(GL_ARRAY_BUFFER, jointIds.size() * sizeof(std::array<unsigned int, 4>), &jointIds.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, 0, 0);

	//Joint Weights
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[JOINT_WEIGHTS]);
	glBufferData(GL_ARRAY_BUFFER, jointWeights.size() * sizeof(Vector4f), &jointWeights.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	
}


AnimatedMesh::~AnimatedMesh() {
	glDeleteVertexArrays(1, &m_vao);
}

void AnimatedMesh::draw() {

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Matrix4f> AnimatedMesh::getBoneArray() {
	std::vector<Matrix4f> boneArray;

	boneArray.assign(m_jointsList.size(), Matrix4f::IDENTITY);
	addJointsToArray(rootJoint, boneArray);

	return boneArray;
}

void AnimatedMesh::addJointsToArray(Joint rootJoint, std::vector<Matrix4f> &boneArray) {

	boneArray[rootJoint.index] = rootJoint.animatedTransform;
	for (int i = 0; i < rootJoint.children.size(); i++) {
		addJointsToArray(rootJoint.children[i], boneArray);
	}
}

void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, Matrix4f> currentPose) {
	applyPoseToJoints(currentPose, rootJoint, Matrix4f::IDENTITY);
}

void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, Matrix4f> currentPose, Joint &joint, Matrix4f parentTransform) {

	Matrix4f currentTransform;
	if (currentPose.find(joint.name) == currentPose.end()) {
		//check for identity maybe there is a bette way
		bool check = true;
		for (short i = 0; i < 4; i++) {
			check = check && memcmp(parentTransform[i], Matrix4f::IDENTITY[i], sizeof(float) * 4) == 0;
		}

		if (check) {
			currentTransform = joint.localBindTransform;			
		}else {
			currentTransform = parentTransform * joint.localBindTransform;
		}

	}else {
		currentTransform = parentTransform * currentPose.at(joint.name);
	}

	for (int i = 0; i < joint.children.size(); i++) {
		applyPoseToJoints(currentPose, joint.children[i], currentTransform);
	}

	if (currentPose.find(joint.name) == currentPose.end()) {
		//check for identity maybe there is a bette way
		bool check = true;
		for (short i = 0; i < 4; i++) {
			check = check && memcmp(parentTransform[i], Matrix4f::IDENTITY[i], sizeof(float) * 4) == 0;
		}

		if (check) {
			joint.animatedTransform = joint.localBindTransform * joint.inverseBindTransform;
		}else {
			joint.animatedTransform = currentTransform * joint.inverseBindTransform;
		}

	}else {
		joint.animatedTransform = currentTransform * joint.inverseBindTransform;
	}
}