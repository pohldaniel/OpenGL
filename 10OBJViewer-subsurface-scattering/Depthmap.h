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
	

	void render(unsigned int texture);

	const Matrix4f &getDepthPassMatrix() const;
	const Matrix4f &getTransformation() const;
	const Matrix4f &getProjectionMatrix() const;
	const Matrix4f &getViewMatrix() const;
	const Vector3f &getPosition() const;

private :

	unsigned int depthmapFBO;
	unsigned int depthmapWidth = 4096;
	unsigned int depthmapHeight = 4096;

	unsigned int viewportWidth = 640;
	unsigned int viewportHeight = 480;

	unsigned int depthViewportWidth = 4096;
	unsigned int depthViewportHeight = 4096;

	unsigned int m_quadVBO;
	unsigned int m_indexQuad;
	int m_size = 25;
	std::vector<float> m_vertex;

	

	ModelMatrix m_modelMatrix;
	Matrix4f m_viewMatrix;
	Matrix4f m_projMatrix;
	Matrix4f m_biasMatrix;

	Vector3f m_eye;

	DepthShader* m_shader;
	TextureShader *m_renderShader;

	Camera* m_camera;

	void createDepthmapFBO();
	void createBuffer();
	
};

#endif // __depthmapH__