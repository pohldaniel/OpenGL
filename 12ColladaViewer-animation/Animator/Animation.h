#ifndef JOINT_ANIM_H
#define JOINT_ANIM_H

#include <map>
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>
#include <string>
#include <vector>



struct JointTransformData {

	std::string jointNameId;
	glm::vec3 positonKeys;
	glm::quat rotationKeys;
	glm::vec3 scallingKeys;

	JointTransformData(std::string _jointNameId, glm::vec3 _positon, glm::quat _rotationKeys, glm::vec3 _scallingKeys) {
		jointNameId = _jointNameId;
		positonKeys = _positon;
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

	
	Animation(const std::string &path, const std::string &rootJoinName);
	virtual ~Animation() {}

	inline std::string getName() { return _name; }
	inline double getDuration() { return duration; }

	std::string _name;
	std::vector<KeyFrameData> keyFrames;
	double duration;
private:
	



};

#endif