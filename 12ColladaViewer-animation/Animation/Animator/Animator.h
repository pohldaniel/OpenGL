#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <memory>
#include <unordered_map>
#include "Animation.h"

class AnimatedModel;

class Animator{

public:
	Animator(AnimatedModel* model);

	void startAnimation(const std::string& animationName);
	void addAnimation(ColladaLoader loader);
	void update(double elapsedTime);

private:
	AnimatedModel* m_model;

	std::vector<std::shared_ptr<Animation>> m_animations;
	std::shared_ptr<Animation> m_currentAnimation;
	double m_animationTime;
	
	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose();
};

#endif