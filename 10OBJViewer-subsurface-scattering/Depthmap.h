#ifndef __depthmapH__
#define __depthmapH__

#include "Camera.h"
#include "Shader.h"
#include "ObjModel.h"

class Depthmap{

public:

	Depthmap();
	~Depthmap();

	unsigned char *irradianceData;
	unsigned char *normalDataColor;
	unsigned char *normalDataDepth;
	unsigned char *depthData;
	unsigned char *singleChannelData;

	unsigned int irradianceMapTexture;
	unsigned int normalMapTexture;
	unsigned int normalDepthTexture;
	unsigned int depthmapTexture;
	unsigned int singleChannelTexture;
	unsigned int texture;

	void setViewMatrix(const Vector3f &lightPos, const Vector3f &target, const Vector3f &up);
	void setProjectionMatrix(float fovx, float aspect, float znear, float zfar);	
	void setOrthMatrix(float left, float right, float bottom, float top, float znear, float zfar);
	
	void renderIrradianceMap(Model const* model);
	void renderNormalMap(Model const* model);
	void renderToDepthTexture(Model const* model);
	void renderToSingleChannel(Model const* model);
	void setViewport(int width, int height);

	const Matrix4f &getDepthPassMatrix() const;
	const Matrix4f &getProjectionMatrix() const;
	const Matrix4f &getProjectionMatrixD3D() const;
	const Matrix4f &getLinearProjectionMatrixD3D() const;
	const Matrix4f &getOrthographicMatrix() const;
	const Matrix4f &getViewMatrix() const;
	const Vector3f &getPosition() const;

private :

	TextureShader *m_renderShader;
	unsigned int normalFBO;
	unsigned int normalFBOMSAA;

	unsigned int fboIrradianceMSAA;
	unsigned int fboCIrradiance;

	unsigned int fboNormalMSAA;
	unsigned int fboCNormal;
	unsigned int fboDNormal;

	unsigned int fboDepthmap;

	unsigned int fboSingleChannelMSAA;
	unsigned int fboCSingleChannel;

	unsigned int depthmapWidth = 512*2;
	unsigned int depthmapHeight = 512*2;

	unsigned int m_viewportWidth = 640;
	unsigned int m_viewportHeight = 480;

	unsigned int depthViewportWidth = 512*2;
	unsigned int depthViewportHeight = 512*2;

	unsigned int m_quadVBO;
	unsigned int m_indexQuad;
	int m_size = 25;
	std::vector<float> m_vertex;

	Matrix4f m_biasMatrix;
	Matrix4f m_viewMatrix;
	Matrix4f m_projMatrix;
	Matrix4f m_projMatrixD3D;
	Matrix4f m_linearProjMatrixD3D;
	Matrix4f m_orthMatrix;

	Vector3f m_eye;
	Vector3f m_target;
	
	Shader *m_irradianceShader;
	Shader *m_normalShader;
	Shader* m_depthMapShader;
	Shader* m_depthMapShader2;

	void createDepthmapFBO();	
};

#endif // __depthmapH__