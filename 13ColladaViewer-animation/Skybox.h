#ifndef __skyboxH__
#define __skyboxH__

#include <iostream>
#include <vector>

#include "Extension.h"
#include "Bitmap.h"
#include "Camera.h"
#include "Shader.h"

class SkyBox{

	public:

		SkyBox(const char* filename, int size, Camera* camera, bool flipVertical, bool flipHorizontal);
		~SkyBox();

		unsigned int m_cubemap;
		unsigned int m_skyboxVBO;

		void render();

private:

		
		void createCubeMap(const char* filename, bool flipVertical, bool flipHorizontal);
		void loadSide(unsigned int side_target, const char* file_name, bool flipVertical, bool flipHorizontal);
		void createBuffer();

		
		int m_size = 50;
		std::vector<float> m_postions;

		Camera* m_camera;
		SkyboxShader* m_shader;
};






#endif // __shaderH__