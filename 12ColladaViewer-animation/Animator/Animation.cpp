#include "Animation.h"
#include <iostream>


glm::quat fromMatrix(const glm::mat4 &m) {

	float w, x, y, z;
	float diagonal = m[0][0] + m[1][1] + m[2][2];
	if (diagonal > 0) {
		float w4 = (float)(sqrtf(diagonal + 1.0) * 2.0);
		w = w4 / 4.0;
		x = (m[2][1] - m[1][2]) / w4;
		y = (m[0][2] - m[2][0]) / w4;
		z = (m[1][0] - m[0][1]) / w4;
	}else if ((m[0][0] >  m[1][1]) && (m[0][0] > m[2][2])) {
		float x4 = (float)(sqrtf(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0);
		w = (m[2][1] - m[1][2]) / x4;
		x = x4 / 4.0;
		y = (m[0][1] + m[1][0]) / x4;
		z = (m[0][2] + m[2][0]) / x4;
	}else if (m[1][1] > m[2][2]) {
		float y4 = (float)(sqrtf(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0);
		w = (m[0][2] - m[2][0]) / y4;
		x = (m[0][1] + m[1][0]) / y4;
		y = y4 / 4.0;
		z = (m[1][2] + m[2][1]) / y4;
	}else {
		float z4 = (float)(sqrtf(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0);
		w = (m[1][0] - m[0][1]) / z4;
		x = (m[0][2] + m[2][0]) / z4;
		y = (m[1][2] + m[2][1]) / z4;
		z = z4 / 4.0;
	}

	return glm::quat(w, x, y, z);
}


Animation::Animation(const std::string &path, const std::string &rootJoinName){
	
	TiXmlDocument doc(path.c_str());
	doc.LoadFile();
	TiXmlElement* animation = doc.RootElement()->FirstChildElement("library_animations")->FirstChildElement("animation")->FirstChildElement("animation");

	TiXmlElement* source;
	TiXmlElement* floatArray;
	char* text;
	int numberOfKeyframes;

	std::vector<float> times;

	glm::mat4 CORRECTION = glm::mat4(1.0f);
	CORRECTION = glm::rotate(CORRECTION, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	while (animation != NULL) {

		JointAnim jAnim;

		source = animation->FirstChildElement("source");
		floatArray = source->FirstChildElement("float_array");
		floatArray->QueryIntAttribute("count", &numberOfKeyframes);

		text = (char*)(floatArray->GetText());
		times.push_back(atof(strtok(text, " ")));

		for (int k = 1; k < numberOfKeyframes; k++) {
			times.push_back(atof(strtok(NULL, " ")));
		}

		source = source->NextSiblingElement("source");
		floatArray = source->FirstChildElement("float_array");

		text = (char*)(floatArray->GetText());

		source = source->NextSiblingElement("channel");
		std::string target = std::string(source->Attribute("target"));
		std::string jointNameId = target.substr(0, target.find('/'));


		keyFrames.resize(numberOfKeyframes);

		glm::mat4 matrix;
		matrix[0][0] = atof(strtok(text, " "));

		short start = 1;

		for (int k = 0; k < numberOfKeyframes; k++) {
			for (int i = 0; i < 4; i++) {
				for (int j = start; j < 4; j++) {
					matrix[i][j] = atof(strtok(NULL, " "));
				}
				start = 0;
			}

			matrix = glm::transpose(matrix);
			if (jointNameId.compare(rootJoinName) == 0) {
				std::cout << jointNameId.c_str()  << "------------"<<  std::endl;			
				//matrix = CORRECTION * matrix;
			}
					

			keyFrames[k].time = times[k];
			
			glm::vec3 position = glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
			
			float sx = glm::length(glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]));
			float sy = glm::length(glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]));
			float sz = glm::length(glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]));

			
			glm::vec3 scale = glm::vec3(sx, sy, sz);

			/*matrix[0][0] = matrix[0][0] * 1.0 / (scale[0]); matrix[0][1] = matrix[0][1] * 1.0 / (scale[0]); matrix[0][2] = matrix[0][2] * 1.0 / (scale[0]); matrix[3][0] = 0.0;
			matrix[1][0] = matrix[1][0] * 1.0 / (scale[1]); matrix[1][1] = matrix[1][1] * 1.0 / (scale[1]); matrix[1][2] = matrix[1][2] * 1.0 / (scale[1]); matrix[3][1] = 0.0;
			matrix[2][0] = matrix[2][0] * 1.0 / (scale[2]); matrix[2][1] = matrix[2][1] * 1.0 / (scale[2]); matrix[2][2] = matrix[2][2] * 1.0 / (scale[2]); matrix[3][2] = 0.0;
			matrix[0][3] = 0.0;								matrix[1][3] = 0.0;								matrix[2][3] = 0.0;								matrix[3][3] = 1.0;*/
			
			//keyFrames[k].pose.insert(std::pair<std::string, JointTransformData>(jointNameId, JointTransformData(jointNameId, position, glm::quat_cast(matrix), scale)));	
			
			keyFrames[k].pose.insert(std::pair<std::string, JointTransformData>(jointNameId, JointTransformData(jointNameId, position, fromMatrix(matrix), scale)));

			jAnim._positionKeys.insert(std::make_pair(times[k], position));
			jAnim._rotationKeys.insert(std::make_pair(times[k], fromMatrix(matrix)));
			jAnim._scallingKeys.insert(std::make_pair(times[k], scale));
		}
		
		jAnim._name = jointNameId;
		_jointAnims.push_back(jAnim);
		animation = animation->NextSiblingElement("animation");
	}

	_name = "";
	_duration = times[numberOfKeyframes - 1];

	duration = times[numberOfKeyframes - 1];
}
