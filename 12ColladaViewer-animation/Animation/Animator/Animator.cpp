#include "Animator.h"
#include <iostream>
#include "..\AnimatedModel\AnimatedModel.h"

Vector3f GetInterpolated(Vector3f start, Vector3f end, float progression) {

	float x = start[0] + (end[0] - start[0]) * progression;
	float y = start[1] + (end[1] - start[1]) * progression;
	float z = start[2] + (end[2] - start[2]) * progression;
	return Vector3f(x, y, z);
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

	}else {
		result[3] = blendI * a[3] + blend * b[3];
		result[0] = blendI * a[0] + blend * b[0];
		result[1] = blendI * a[1] + blend * b[1];
		result[2] = blendI * a[2] + blend * b[2];
	}

	return Quaternion::normalize(result);
}

Animator::Animator(AnimatedModel *model){
	m_model = model;
}

void Animator::startAnimation(const std::string & animationName){

	for (auto animation : m_animations){

		if (animation->getName() == animationName){
			m_animationTime = 0;
			m_currentAnimation = animation;
		}
	}
}

void Animator::addAnimation(ColladaLoader loader){
	m_animations.push_back(std::make_shared<Animation>(loader));
}


void Animator::update(double elapsedTime){

	//increase animationTime
	m_animationTime += elapsedTime;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	std::unordered_map<std::string, Matrix4f> currentPose = calculateCurrentAnimationPose();
	m_model->m_meshes[0]->applyPoseToJoints(currentPose);
}

std::unordered_map<std::string, Matrix4f> Animator::calculateCurrentAnimationPose() {

	std::vector<KeyFrameData> keyFrames = m_currentAnimation->m_keyFrames;

	/**c++ implementation*/
	std::vector<KeyFrameData>::iterator upper = std::upper_bound(keyFrames.begin() + 1, keyFrames.end(), m_animationTime, KeyFrameData::greater_than());
	KeyFrameData  nextFrame = *upper;
	KeyFrameData  previousFrame = *(std::prev(upper));

	/**custom implementation*/
	/*KeyFrameData  previousFrame = keyFrames[0];
	KeyFrameData  nextFrame = keyFrames[0];
	for (int i = 1; i < keyFrames.size(); i++) {
	nextFrame = keyFrames[i];
	if (nextFrame.time >  m_animationTime) {
	break;
	}
	previousFrame = keyFrames[i];
	}*/


	float totalTime = nextFrame.time - previousFrame.time;
	float currentTime = m_animationTime - previousFrame.time;
	float progression = currentTime / totalTime;

	std::unordered_map<std::string, Matrix4f> currentPose;

	std::map<std::string, JointTransformData>::iterator it = previousFrame.pose.begin();
	while (it != previousFrame.pose.end()) {

		std::string name = it->first;

		Vector3f position = GetInterpolated(previousFrame.pose.at(name).positonKeys, nextFrame.pose.at(name).positonKeys, progression);
		Vector3f scale = GetInterpolated(previousFrame.pose.at(name).scallingKeys, nextFrame.pose.at(name).scallingKeys, progression);

		Quaternion rot = interpolateQuat(previousFrame.pose.at(name).rotationKeys, nextFrame.pose.at(name).rotationKeys, progression);
		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);

		Matrix4f::transpose(trans);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(name, sca * trans * mat));
		it++;
	}
	return currentPose;

}