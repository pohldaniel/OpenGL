#include "Animator.h"
#include "..\AnimatedModel\AnimatedModel.h"
#include <iostream>


glm::vec3 GetInterpolated(glm::vec3 start, glm::vec3 end, float progression) {

	float x = start.x + (end.x - start.x) * progression;
	float y = start.y + (end.y - start.y) * progression;
	float z = start.z + (end.z - start.z) * progression;
	return glm::vec3(x, y, z);
}




glm::mat4 toRotationMatrix(glm::quat quat) {
	/*glm::mat4 matrix = glm::mat4(1.0);
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

	return matrix;*/

	float x2 = quat.x + quat.x;
	float y2 = quat.y + quat.y;
	float z2 = quat.z + quat.z;
	float xx = quat.x * x2;
	float xy = quat.x * y2;
	float xz = quat.x * z2;
	float yy = quat.y * y2;
	float yz = quat.y * z2;
	float zz = quat.z * z2;
	float wx = quat.w * x2;
	float wy = quat.w * y2;
	float wz = quat.w * z2;

	glm::mat4 matrix = glm::mat4(1.0);

	matrix[0][0] = 1.0f - (yy + zz);
	matrix[0][1] = xy - wz;
	matrix[0][2] = xz + wy;
	matrix[0][3] = 0.0f;

	matrix[1][0] = xy + wz;
	matrix[1][1] = 1.0f - (xx + zz);
	matrix[1][2] = yz - wx;
	matrix[1][3] = 0.0f;

	matrix[2][0] = xz - wy;
	matrix[2][1] = yz + wx;
	matrix[2][2] = 1.0f - (xx + yy);
	matrix[2][3] = 0.0f;

	matrix[3][0] = 0.0f;
	matrix[3][1] = 0.0f;
	matrix[3][2] = 0.0f;
	matrix[3][3] = 1.0f;

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

Quaternion interpolateQuat(Quaternion a, Quaternion b, float blend) {
	Quaternion result = Quaternion(0.0, 0.0, 0.0, 1.0);
	float dot = a[3] * b[3] + a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	float blendI = 1.0 - blend;

	if (dot < 0) {
		result[3] = blendI * a[3] + blend * -b[3];
		result[0] = blendI * a[0] + blend * -b[0];
		result[1] = blendI * a[1] + blend * -b[1];
		result[2] = blendI * a[2] + blend * -b[2];
	}
	else {
		result[3] = blendI * a[3] + blend * b[3];
		result[0] = blendI * a[0] + blend * b[0];
		result[1] = blendI * a[1] + blend * b[1];
		result[2] = blendI * a[2] + blend * b[2];
	}

	return Quaternion::normalize(result);
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

void Animator::addAnimation(ColladaLoader loader){
	_animations.push_back(std::make_shared<Animation>(loader));
}


void Animator::Update(double elapsedTime){

	//increase animationTime
	_animationTime += elapsedTime;
	if (_animationTime > _currentAnimation->getDuration()) {
		_animationTime = fmod(_animationTime, _currentAnimation->getDuration());
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
	std::unordered_map<std::string, Matrix4f> currentPose2;

	std::map<std::string, JointTransformData>::iterator it = previousFrame.pose.begin();
	while (it != previousFrame.pose.end()) {

		std::string name = it->first;
			
		glm::vec3 position = GetInterpolated(previousFrame.pose.at(name).positonKeys, nextFrame.pose.at(name).positonKeys, progression);
		glm::vec3 scale = GetInterpolated(previousFrame.pose.at(name).scallingKeys, nextFrame.pose.at(name).scallingKeys, progression);
		
		/**Built in interpolation correspondents to the built in "glm::quat_cast function" at Animamtion.cpp*/
		//glm::quat rotation = glm::slerp(previousFrame.pose.at(name).rotationKeys, nextFrame.pose.at(name).rotationKeys, progression);
		//currentPose.insert(std::make_pair(name, glm::scale(scale) * glm::translate(position) *  glm::toMat4(rotation)));

		/**Custom interpolation correspondets to the custom "fromMatrix function" at Animation.cpp*/
		//glm::quat rotation = interpolateQuat(previousFrame.pose.at(name).rotationKeys, nextFrame.pose.at(name).rotationKeys, progression);
		//currentPose.insert(std::make_pair(name, glm::scale(scale) * glm::translate(position) * toRotationMatrix(rotation)));

		Quaternion rot = interpolateQuat(previousFrame.pose.at(name).rotation, nextFrame.pose.at(name).rotation, progression);
		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);

		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		glm::mat4 tmp;

		tmp[0][0] = mat[0][0]; tmp[0][1] = mat[0][1]; tmp[0][2] = mat[0][2]; tmp[0][3] = mat[0][3];
		tmp[1][0] = mat[1][0]; tmp[1][1] = mat[1][1]; tmp[1][2] = mat[1][2]; tmp[1][3] = mat[1][3];
		tmp[2][0] = mat[2][0]; tmp[2][1] = mat[2][1]; tmp[2][2] = mat[2][2]; tmp[2][3] = mat[2][3];
		tmp[3][0] = mat[3][0]; tmp[3][1] = mat[3][1]; tmp[3][2] = mat[3][2]; tmp[3][3] = mat[3][3];

		currentPose.insert(std::make_pair(name, glm::scale(scale) * glm::translate(position) * tmp));
		//currentPose2.insert(std::make_pair(name, sca * trans * mat));
		it++;
	}
	return currentPose;
}


