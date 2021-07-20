#ifndef __skyboxH__
#define __skyboxH__

#include <iostream>
#include <vector>
#include <memory>

#include "Extension.h"
#include "Bitmap.h"
#include "Camera.h"
#include "Shader.h"

class SkyBox {

public:

	SkyBox(const char* filename, int size, bool flipVertical, bool flipHorizontal, const Vector3f &position = Vector3f(0.0f, 0.0f, 0.0f));
	~SkyBox();

	void setProjectionMatrix(const Matrix4f &projection);
	void draw(const Camera camera);
	unsigned int m_cubemap;
private:

	static void FlipVertical(unsigned int width, unsigned int height, unsigned short channels, unsigned char(*&image));
	static void FlipHorizontal(unsigned int width, unsigned int height, unsigned short channels, unsigned char(*&image));

	void createCubeMap(const char* filename, bool flipVertical, bool flipHorizontal);
	void loadSide(unsigned int side_target, const char* file_name, bool flipVertical, bool flipHorizontal);
	void createBuffer(const Vector3f &position);

	int m_size = 50;
	std::vector<float> m_postions;
	std::shared_ptr<Shader> m_shader;
	
	unsigned int m_skyboxVBO;
	unsigned int m_vao;
};
#endif