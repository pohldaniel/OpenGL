
#include <vector>
#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(const std::string & _path) : path(_path) {
	
	ColladaLoader loader(_path);
	
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::uvec4> jointIds;
	std::vector<glm::vec4> jointWeights;
	std::vector<unsigned int> indices;

	loader.loadData(positions, texCoords, normals, jointIds, jointWeights, indices, jointsList);
	loader.createJoints(rootJoint);

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

	boneArray.assign(jointsList.size(), glm::mat4());
	addJointsToArray(rootJoint, boneArray);

	return boneArray;
}

void AnimatedMesh::addJointsToArray(Joint rootJoint, std::vector<glm::mat4> &boneArray) {

	boneArray[rootJoint.index] = rootJoint.animatedTransform;
	for (int i = 0; i < rootJoint.children.size(); i++) {
		addJointsToArray(rootJoint.children[i], boneArray);
	}
}

void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose) {
	applyPoseToJoints(currentPose, rootJoint, glm::mat4(1.0));
}

void AnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, glm::mat4> currentPose, Joint &joint, glm::mat4 parentTransform) {
	
	glm::mat4 currentTransform;
	if (currentPose.find(joint.name) == currentPose.end()) {
		//check for identity maybe there is a bette way
		const float *pParentTransform = (const float*)glm::value_ptr(parentTransform);
		const float *pIdentity = (const float*)glm::value_ptr(identity);
		//static const float pIdentity[] = { 1.0, 0.0 , 0.0 , 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
		if (memcmp(pParentTransform, pIdentity, sizeof(float) * 16) == 0) {
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
		const float *pParentTransform = (const float*)glm::value_ptr(parentTransform);
		const float *pIdentity = (const float*)glm::value_ptr(identity);
		//static const float pIdentity[] = { 1.0, 0.0 , 0.0 , 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
		if (memcmp(pParentTransform, pIdentity, sizeof(float) * 16) == 0) {
			joint.animatedTransform =  joint.localBindTransform * joint.inverseBindTransform;
		}else {
			joint.animatedTransform = currentTransform * joint.inverseBindTransform;
		}
	}else {		
		joint.animatedTransform = currentTransform * joint.inverseBindTransform;
	}
}