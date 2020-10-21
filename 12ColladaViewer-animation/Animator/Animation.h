#ifndef JOINT_ANIM_H
#define JOINT_ANIM_H

#include <map>
#include <string>
#include <vector>
#include "..\ColladaLoader\ColladaLoader.h"
#include "..\Vector.h"

struct JointTransformData {

	std::string jointNameId;
	Vector3f positonKeys;
	Quaternion rotationKeys;
	Vector3f scallingKeys;

	JointTransformData(std::string _jointNameId, Vector3f _positonKeys,Quaternion _rotationKeys, Vector3f _scallingKeys) {
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

#endif

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "..\tinyxml\tinyxml.h"


class Animation{

public:

	
	Animation(ColladaLoader loader);
	virtual ~Animation() {}

	inline std::string getName() { return _name; }
	inline double getDuration() { return duration; }

	std::string _name;
	std::vector<KeyFrameData> keyFrames;
	double duration;
private:
	



};

#endif