#include <iostream>
#include "Animation.h"

Animation::Animation(ColladaLoader loader){
	
	TiXmlElement* animation = loader.doc.RootElement()->FirstChildElement("library_animations")->FirstChildElement("animation")->FirstChildElement("animation");


	TiXmlElement* source;
	TiXmlElement* floatArray;
	char* text;
	int numberOfKeyframes;

	std::vector<float> times;

	
	bool firtsBoneAtAnimation = true;

	while (animation != NULL) {

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

		Matrix4f  matrix;
		matrix[0][0] = atof(strtok(text, " "));
		short start = 1;

		for (int k = 0; k < numberOfKeyframes; k++) {
			for (int i = 0; i < 4; i++) {
				for (int j = start; j < 4; j++) {
					matrix[i][j] = atof(strtok(NULL, " "));
				}
				start = 0;
			}

			Matrix4f::transpose(matrix);
			keyFrames[k].time = times[k];
			
			Vector3f position = Vector3f(matrix[3][0], matrix[3][1], matrix[3][2]);
			
			float sx = Vector3f(matrix[0][0], matrix[1][0], matrix[2][0]).length();
			float sy = Vector3f(matrix[0][1], matrix[1][1], matrix[2][1]).length();
			float sz = Vector3f(matrix[0][2], matrix[1][2], matrix[2][2]).length();
			
			Vector3f scale = Vector3f(sx, sy, sz);
			//*matrix[0][0] = matrix[0][0] * 1.0 / (scale[0]); matrix[0][1] = matrix[0][1] * 1.0 / (scale[0]); matrix[0][2] = matrix[0][2] * 1.0 / (scale[0]); matrix[3][0] = 0.0;
			//*matrix[1][0] = matrix[1][0] * 1.0 / (scale[1]); matrix[1][1] = matrix[1][1] * 1.0 / (scale[1]); matrix[1][2] = matrix[1][2] * 1.0 / (scale[1]); matrix[3][1] = 0.0;
			//*matrix[2][0] = matrix[2][0] * 1.0 / (scale[2]); matrix[2][1] = matrix[2][1] * 1.0 / (scale[2]); matrix[2][2] = matrix[2][2] * 1.0 / (scale[2]); matrix[3][2] = 0.0;
			//*matrix[0][3] = 0.0;								matrix[1][3] = 0.0;								matrix[2][3] = 0.0;								matrix[3][3] = 1.0;

			keyFrames[k].pose.insert(std::pair<std::string, JointTransformData>(jointNameId, JointTransformData(jointNameId, position, Quaternion(matrix), scale)));
		}		
		animation = animation->NextSiblingElement("animation");
	}
	duration = times[numberOfKeyframes - 1];
	_name = "";
}
