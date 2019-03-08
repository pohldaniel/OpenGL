#ifndef __depthmapH__
#define __depthmapH__

#include "Camera.h"
#include "Shader.h"
#include "Object.h"

class Depthmap{

public:

	Depthmap(Camera* camera);
	~Depthmap();

	unsigned char *normalData;
	unsigned char *depthData;
	unsigned char *depthData2;
	unsigned char *depthData3;
	unsigned char *depthData4;
	unsigned char *depthData5;

	unsigned int depthmapTexture;
	unsigned int depthmapTexture2;
	unsigned int depthmapTexture3;
	unsigned int depthmapTexture4;
	unsigned int singleChannel;
	

	unsigned int irradianceMap;
	unsigned int normalMap;

	unsigned int texture;

	void setViewMatrix(const Vector3f &lightPos, const Vector3f &target, const Vector3f &up);
	void setProjectionMatrix(float fovx, float aspect, float znear, float zfar);	
	void renderToDepthTexture(Object const* object);
	void renderToDepthTexture2(Object const* object);
	void renderToDepthTexture3(Object const* object);
	void renderToDepthTexture4(Object const* object);
	void renderNormalMap(Object const* object);
	void renderIrradianceMap(Object const* object);

	void render(unsigned int texture);

	const Matrix4f &getDepthPassMatrix() const;
	const Matrix4f &getProjectionMatrix() const;
	const Matrix4f &getViewMatrix() const;
	const Vector3f &getPosition() const;

private :

	unsigned int fboCDepthMSAA;
	unsigned int fboCDepth;

	unsigned int depthmapFBO2;
	
	unsigned int normalFBO;
	unsigned int normalFBOMSAA;

	unsigned int fboIrradianceMSAA;
	unsigned int fboCIrradiance;

	unsigned int fboNormalMSAA;
	unsigned int fboCNormal;
	unsigned int fboDNormal;

	unsigned int fboDDepthMSAA;
	unsigned int fboDDepth;

	unsigned int fboSingleChannelMSAA;
	unsigned int fboCSingleChannel;

	unsigned int depthmapWidth = 512;
	unsigned int depthmapHeight = 512;

	unsigned int viewportWidth = 640;
	unsigned int viewportHeight = 480;

	unsigned int depthViewportWidth = 512;
	unsigned int depthViewportHeight = 512;

	unsigned int m_quadVBO;
	unsigned int m_indexQuad;
	int m_size = 25;
	std::vector<float> m_vertex;

	Matrix4f m_biasMatrix;
	Matrix4f m_viewMatrix;
	Matrix4f m_projMatrix;
	

	Vector3f m_eye;
	Vector3f m_target;
	

	
	TextureShader *m_renderShader;

	Shader *m_normalShader;
	Shader* m_depthShader;
	Shader* m_depthMapShader;
	Shader* m_depthMapShader2;
	Shader *m_irradianceShader;

	Camera* m_camera;

	void createDepthmapFBO();
	void createBuffer();
	
};

#endif // __depthmapH__