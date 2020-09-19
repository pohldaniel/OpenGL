#include "Animator.h"
#include "..\AnimatedModel\AnimatedModel.h"
#include <iostream>


glm::vec3 GetInterpolated(glm::vec3 start, glm::vec3 end, float progression) {

	float x = start.x + (end.x - start.x) * progression;
	float y = start.y + (end.y - start.y) * progression;
	float z = start.z + (end.z - start.z) * progression;
	return glm::vec3(x, y, z);
}

glm::quat GetInterpolatedQuat(glm::quat start, glm::quat end, float progression) {

	glm::quat rot = glm::slerp(start, end, progression);
	return glm::normalize(rot);
}

glm::quat fromMatrix3(const glm::mat4 &m) {
	float w, x, y, z;
	float diagonal = m[0][0] + m[1][1] + m[2][2];
	if (diagonal > 0) {
		float w4 = (float)(sqrtf(diagonal + 1.0) * 2.0);
		w = w4 / 4.0;
		x = (m[2][1] - m[1][2]) / w4;
		y = (m[0][2] - m[2][0]) / w4;
		z = (m[1][0] - m[0][1]) / w4;
	}
	else if ((m[0][0] >  m[1][1]) && (m[0][0] > m[2][2])) {
		float x4 = (float)(sqrtf(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0);
		w = (m[2][1] - m[1][2]) / x4;
		x = x4 / 4.0;
		y = (m[0][1] + m[1][0]) / x4;
		z = (m[0][2] + m[2][0]) / x4;
	}
	else if (m[1][1] > m[2][2]) {
		float y4 = (float)(sqrtf(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0);
		w = (m[0][2] - m[2][0]) / y4;
		x = (m[0][1] + m[1][0]) / y4;
		y = y4 / 4.0;
		z = (m[1][2] + m[2][1]) / y4;
	}
	else {
		float z4 = (float)(sqrtf(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0);
		w = (m[1][0] - m[0][1]) / z4;
		x = (m[0][2] + m[2][0]) / z4;
		y = (m[1][2] + m[2][1]) / z4;
		z = z4 / 4.0;
	}

	return glm::quat(w, x, y, z);
}

glm::mat4 toRotationMatrix(glm::quat quat) {
	glm::mat4 matrix = glm::mat4(1.0);
	float xy = quat.x * quat.y;
	float xz = quat.x * quat.z;
	float xw = quat.x * quat.w;
	float yz = quat.y * quat.z;
	float yw = quat.y * quat.w;
	float zw = quat.z * quat.w;
	float xSquared = quat.x * quat.x;
	float ySquared = quat.y * quat.y;
	float zSquared = quat.z * quat.z;

	matrix[0][0] = 1 - 2 * (ySquared + zSquared);
	matrix[0][1] = 2 * (xy - zw);
	matrix[0][2] = 2 * (xz + yw);
	matrix[0][3] = 0;
	matrix[1][0] = 2 * (xy + zw);
	matrix[1][1] = 1 - 2 * (xSquared + zSquared);
	matrix[1][2] = 2 * (yz - xw);
	matrix[1][3] = 0;
	matrix[2][0] = 2 * (xz - yw);
	matrix[2][1] = 2 * (yz + xw);
	matrix[2][2] = 1 - 2 * (xSquared + ySquared);
	matrix[2][3] = 0;
	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;

	return matrix;
}


glm::quat interpolateQuat(glm::quat a, glm::quat b, float blend) {
	glm::quat result = glm::quat(0.0, 0.0, 0.0, 1.0);
	float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	float blendI = 1.0 - blend;

	if (dot < 0) {
		result.w = blendI * a.w + blend * -b.w;
		result.x = blendI * a.x + blend * -b.x;
		result.y = blendI * a.y + blend * -b.y;
		result.z = blendI * a.z + blend * -b.z;
	}else {
		result.w = blendI * a.w + blend * b.w;
		result.x = blendI * a.x + blend * b.x;
		result.y = blendI * a.y + blend * b.y;
		result.z = blendI * a.z + blend * b.z;
	}

	return glm::normalize(result);
}


Animator::Animator(AnimatedModel *model){
	_model = model;
}

void Animator::startAnimation(const std::string & animationName){

	for (auto animation : _animations){

		if (animation->getName() == animationName){
			_animationTime = 0;
			_currentAnimation = animation;
		}
	}
}

void Animator::addAnimation(const std::string &filename, const std::string &rootJoinName){

	_animations.push_back(std::make_shared<Animation>(filename, rootJoinName));

}


void Animator::Update(double elapsedTime){

	//increase animationTime
	_animationTime += elapsedTime;
	if (_animationTime > _currentAnimation->getDuration()) {
		_animationTime = fmod(_animationTime, _currentAnimation->duration);
	}

	std::unordered_map<std::string, glm::mat4> currentPose = calculateCurrentAnimationPose();

	

	_model->_meshes[0]->applyPoseToJoints(currentPose);
	
}


std::unordered_map<std::string, glm::mat4> Animator::calculateCurrentAnimationPose() {

	std::vector<KeyFrameData> keyFrames = _currentAnimation->keyFrames;

	/**c++ implementation*/
	std::vector<KeyFrameData>::iterator upper = std::upper_bound(keyFrames.begin() + 1, keyFrames.end(), _animationTime, KeyFrameData::greater_than());
	KeyFrameData  nextFrame = *upper;
	KeyFrameData  previousFrame = *(std::prev(upper));

	/**custom implementation*/
	/*KeyFrameData  previousFrame = keyFrames[0];
	KeyFrameData  nextFrame = keyFrames[0];
	for (int i = 1; i < keyFrames.size(); i++) {
		nextFrame = keyFrames[i];
		if (nextFrame.time >  _animationTime) {
			break;
		}
		previousFrame = keyFrames[i];
	}*/


	float totalTime = nextFrame.time - previousFrame.time;
	float currentTime = _animationTime - previousFrame.time;
	float progression = currentTime / totalTime;

	std::unordered_map<std::string, glm::mat4> currentPose;

	std::map<std::string, JointTransformData>::iterator it = previousFrame.pose.begin();
	while (it != previousFrame.pose.end()) {

		std::string name = it->first;
		
	
		glm::vec3 position = GetInterpolated(previousFrame.pose.at(name).positonKeys, nextFrame.pose.at(name).positonKeys, progression);
		glm::vec3 scale = GetInterpolated(previousFrame.pose.at(name).scallingKeys, nextFrame.pose.at(name).scallingKeys, progression);
		
		/**Built in interpolation correspondents to the built in "glm::quat_cast function" at Animamtion.cpp*/
		//glm::quat rotation = glm::slerp(previousFrame.pose.at(name).rotationKeys, nextFrame.pose.at(name).rotationKeys, progression);
		//currentPose.insert(std::make_pair(name, glm::scale(scale) * glm::translate(position) *  glm::toMat4(rotation)));

		/**Custom interpolation correspondets to the custom "fromMatrix function" at Animation.cpp*/
		glm::quat rotation = interpolateQuat(previousFrame.pose.at(name).rotationKeys, nextFrame.pose.at(name).rotationKeys, progression);
		currentPose.insert(std::make_pair(name, glm::scale(scale) * glm::translate(position) * toRotationMatrix(rotation)));
		it++;
	}
	return currentPose;
}


