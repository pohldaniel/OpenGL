#ifndef __depthmapH__
#define __depthmapH__

#include "Camera.h"
#include "Shader.h"
#include "Object.h"

class Depthmap{

public:

	Depthmap(Camera* camera);
	~Depthmap();

	unsigned int depthmapTexture;
	
	
	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);

	void setViewMatrix(const Vector3f &lightPos, const Vector3f &target, const Vector3f &up);
	void setProjectionMatrix(float fovx, float aspect, float znear, float zfar);
	void setDepthViewport(unsigned int widht, unsigned int height);
	void renderToDepthTexture(Object const* object);
	void render();


private :

	unsigned int depthmapFBO;
	unsigned int depthmapWidth = 1024;
	unsigned int depthmapHeight = 1024;

	unsigned int viewportWidth = 640;
	unsigned int viewportHeight = 480;

	unsigned int depthViewportWidth = viewportWidth * 2;
	unsigned int depthViewportHeight = viewportHeight * 2;

	unsigned int m_quadVBO;
	unsigned int m_indexQuad;
	int m_size = 25;
	std::vector<float> m_vertex;


	ModelMatrix m_modelMatrix;
	Matrix4f m_viewMatrix;
	Matrix4f m_projMatrix;
	
	DepthShader* m_shader;
	TextureShader *m_renderShader;

	Camera* m_camera;

	void createDepthmapFBO();
	void createBuffer();
	
};

#endif // __depthmapH__