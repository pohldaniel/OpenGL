#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <string>
#include <vector>
#include "..\..\Vector.h"
#include "..\ColladaLoader\ColladaLoader.h"


struct JointTransformData {

	std::string jointNameId;
	Vector3f positonKeys;
	Quaternion rotationKeys;
	Vector3f scallingKeys;

	JointTransformData(std::string _jointNameId, Vector3f _positonKeys, Quaternion _rotationKeys, Vector3f _scallingKeys) {
		jointNameId = _jointNameId;
		positonKeys = _positonKeys;
		rotationKeys = _rotationKeys;
		scallingKeys = _scallingKeys;
	}
};

struct KeyFrameData {

	double time;
	std::map<std::string, JointTransformData> pose;

	struct greater_than {
		bool operator()(const double b, const KeyFrameData &a) {
			return a.time > b;
		}
	};
};

class Animation{

	friend class Animator;

public:	
	Animation(ColladaLoader loader);
	virtual ~Animation() {}

	inline std::string getName() { return m_name; }
	inline float getDuration() { return m_duration; }
	
private:
	
	std::string m_name;
	std::vector<KeyFrameData> m_keyFrames;
	float m_duration;


};

#endif