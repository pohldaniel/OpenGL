#include "Depthmap.h"

Depthmap::Depthmap() {

	m_irradianceShader = new Shader("shader/irradiance.vert", "shader/irradiance.frag");
	m_normalShader = new Shader("shader/normalMap.vert", "shader/normalMap.frag");
	m_depthMapShader = new Shader("shader/depthMap.vert", "shader/depthMap.frag");
	m_depthMapShader2 = new Shader("shader/depthMap2.vert", "shader/depthMap2.frag");

	createDepthmapFBO();

	m_biasMatrix = Matrix4f( 0.5, 0.0, 0.0, 0.5,
							 0.0, 0.5, 0.0, 0.5,
							 0.0, 0.0, 0.5, 0.5,
							 0.0, 0.0, 0.0, 1.0 );

	Bitmap *bitmap = new Bitmap();
	if (bitmap->loadBitmap24("textures/jade.bmp")){

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

		glGenerateMipmap(GL_TEXTURE_2D);

	}
	delete bitmap;
	bitmap = NULL;

	//char arrays for creating debug bitmaps
	irradianceData = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);
	normalDataColor = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);
	normalDataDepth = (unsigned char*)malloc(depthmapWidth * depthmapHeight);
	depthData = (unsigned char*)malloc(depthmapWidth * depthmapHeight);
	singleChannelData = (unsigned char*)malloc(depthmapWidth * depthmapHeight);

}

Depthmap::~Depthmap(){

	if (m_normalShader){
		delete m_normalShader;
		m_normalShader = 0;
	}

	if (m_irradianceShader){
		delete m_irradianceShader;
		m_irradianceShader = 0;
	}

	if (m_depthMapShader){
		delete m_depthMapShader;
		m_depthMapShader = 0;
	}

	if (m_depthMapShader2){
		delete m_depthMapShader2;
		m_depthMapShader2 = 0;
	}

	if (irradianceData){
		free(irradianceData);
		irradianceData = NULL;
	}

	if (normalDataColor){
		free(normalDataColor);
		normalDataColor = NULL;
	}

	if (normalDataDepth){
		free(normalDataDepth);
		normalDataDepth = NULL;
	}

	if (depthData){
		free(depthData);
		depthData = NULL;
	}

	if (singleChannelData){
		free(singleChannelData);
		singleChannelData = NULL;
	}
}

void Depthmap::createDepthmapFBO(){

	////////////////////////////irradianceMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &irradianceMapTexture);
	glBindTexture(GL_TEXTURE_2D, irradianceMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//////////////////////////setup MSAA framebuffer///////////////////////////////////////
	unsigned int colorBuf8;
	glGenRenderbuffers(1, &colorBuf8);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuf8);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	unsigned int depthBuf8;
	glGenRenderbuffers(1, &depthBuf8);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf8);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &fboIrradianceMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboIrradianceMSAA);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuf8);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf8);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////end setup MSAA framebuffer///////////////////////////////////////

	//////////////////////////setup normal (no MSAA) FBOs as blit target///////////////////////
	glGenFramebuffers(1, &fboCIrradiance);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCIrradiance);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, irradianceMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////

	/**********************************************************************************************/
	////////////////////////////normalMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &normalMapTexture);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenTextures(1, &normalDepthTexture);
	glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//////////////////////////setup MSAA framebuffer///////////////////////////////////////
	unsigned int colorBuf1;
	glGenRenderbuffers(1, &colorBuf1);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuf1);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	
	unsigned int depthBuf1;
	glGenRenderbuffers(1, &depthBuf1);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf1);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glGenFramebuffers(1, &fboNormalMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboNormalMSAA);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuf1);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////end setup MSAA framebuffer///////////////////////////////////////

	//////////////////////////setup normal (no MSAA) FBOs as blit target///////////////////////
	glGenFramebuffers(1, &fboCNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCNormal);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &fboDNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDNormal);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, normalDepthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/**********************************************************************************************/


	/**********************************************************************************************/
	////////////////////////////depthMap rendertarget//////////////////////////////////////////////
	glGenFramebuffers(1, &fboDepthmap);
	glGenTextures(1, &depthmapTexture);

	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fboDepthmap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////
	/**********************************************************************************************/

	/**********************************************************************************************/
	//////////////////////////setup MSAA lumiance framebuffer///////////////////////////////////////
	glGenTextures(1, &singleChannelTexture);
	glBindTexture(GL_TEXTURE_2D, singleChannelTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, depthmapWidth, depthmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int colorBuf3;
	glGenRenderbuffers(1, &colorBuf3);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuf3);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RED, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_LUMINANCE, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	unsigned int depthBuf3;
	glGenRenderbuffers(1, &depthBuf3);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf3);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glGenFramebuffers(1, &fboSingleChannelMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboSingleChannelMSAA);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuf3);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf3);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////end setup MSAA framebuffer///////////////////////////////////////

	//////////////////////////setup normal (no MSAA) FBOs as blit target///////////////////////
	glGenFramebuffers(1, &fboCSingleChannel);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCSingleChannel);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, singleChannelTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////
	/**********************************************************************************************/
}

void Depthmap::setViewMatrix(const Vector3f &lightPos, const Vector3f &target, const Vector3f &up){

	Matrix4f view;
	view.lookAt(lightPos, target, up);
	m_viewMatrix = view;

	m_eye = lightPos;
	m_target = target;
}

void Depthmap::setProjectionMatrix(float fovx, float aspect, float znear, float zfar){

	Matrix4f projection;
	projection.perspectiveD3D(fovx, (GLfloat)depthmapWidth / (GLfloat)depthmapHeight, znear, zfar);
	m_projMatrixD3D = projection;

	projection.perspective(fovx, (GLfloat)depthmapWidth / (GLfloat)depthmapHeight, znear, zfar);
	m_projMatrix = projection;

	projection.linearPerspectiveD3D(fovx, (GLfloat)depthmapWidth / (GLfloat)depthmapHeight, znear, zfar);
	m_linearProjMatrixD3D = projection;

	glUseProgram(m_irradianceShader->m_program);
	m_irradianceShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_normalShader->m_program);
	m_normalShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_depthMapShader->m_program);
	m_depthMapShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_depthMapShader2->m_program);
	m_depthMapShader2->loadMatrix("u_projection", m_linearProjMatrixD3D);
	glUseProgram(0);
}

void Depthmap::setOrthMatrix(float left, float right, float bottom, float top, float znear, float zfar){

	Matrix4f projection;
	projection.orthographic(left, right, bottom, top, znear, zfar);
	m_orthMatrix = projection;

	glUseProgram(m_depthMapShader->m_program);
	m_depthMapShader->loadMatrix("u_projection", m_orthMatrix);
	glUseProgram(0);
}

const Matrix4f &Depthmap::getOrthographicMatrix() const{
	return m_orthMatrix;
}

const Matrix4f &Depthmap::getDepthPassMatrix() const{
	return    m_projMatrix * m_biasMatrix;
}

const Matrix4f &Depthmap::getProjectionMatrix() const{
	return  m_projMatrix;
}

const Matrix4f &Depthmap::getProjectionMatrixD3D() const{
	return  m_projMatrixD3D;
}

const Matrix4f &Depthmap::getLinearProjectionMatrixD3D() const{
	return  m_linearProjMatrixD3D;
}

const Matrix4f &Depthmap::getViewMatrix() const{
	return m_viewMatrix;
}


const Vector3f &Depthmap::getPosition() const{
	return m_eye;
}

void Depthmap::setViewport(int width, int height) {
	m_viewportWidth = width;
	m_viewportHeight = height;
}

void Depthmap::renderToDepthTexture(Model const* model) {
	
	glBindFramebuffer(GL_FRAMEBUFFER, fboDepthmap);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_depthMapShader->m_program);
	m_depthMapShader->loadMatrix("u_modelView", model->getTransformationMatrix() * m_viewMatrix);
	model->draw();
	glUseProgram(0);
	glViewport(0, 0, m_viewportWidth, m_viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

void Depthmap::renderToSingleChannel(Model const* model) {
	
	glBindFramebuffer(GL_FRAMEBUFFER, fboSingleChannelMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	glUseProgram(m_depthMapShader2->m_program);
	m_depthMapShader2->loadMatrix("u_modelView", model->getTransformationMatrix() * m_viewMatrix);
	model->draw();
	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_viewportWidth, m_viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, fboSingleChannelMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCSingleChannel);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);


	glBindFramebuffer(GL_FRAMEBUFFER, fboCSingleChannel);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_RED, GL_UNSIGNED_BYTE, singleChannelData);
	Bitmap::saveBitmap8("depth2.bmp", singleChannelData, depthmapWidth, depthmapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void Depthmap::renderNormalMap(Model const* model) {

	glBindFramebuffer(GL_FRAMEBUFFER, fboNormalMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	glUseProgram(m_normalShader->m_program);
	model->draw();
	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_viewportWidth, m_viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboNormalMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCNormal);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);                           // scale filter

	/*glBindFramebuffer(GL_FRAMEBUFFER, fboCNormal);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_BGR, GL_UNSIGNED_BYTE, normalData);
	Bitmap::saveBitmap24("normal.bmp", normalData, depthmapWidth, depthmapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboNormalMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDNormal);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	/*glBindFramebuffer(GL_FRAMEBUFFER, fboDNormal);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, normalDataDepth);
	Bitmap::saveBitmap8("depth.bmp", normalDataDepth, depthmapWidth, depthmapHeight);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Depthmap::renderIrradianceMap(Model const* model) {
	
	glBindFramebuffer(GL_FRAMEBUFFER, fboIrradianceMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	glUseProgram(m_irradianceShader->m_program);
	model->draw();
	glUseProgram(0);

	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_viewportWidth, m_viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIrradianceMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCIrradiance);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	/*glBindFramebuffer(GL_FRAMEBUFFER, fboCIrradiance);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_BGR, GL_UNSIGNED_BYTE, irradianceData);
	Bitmap::saveBitmap24("irradiance.bmp", irradianceData, depthmapWidth, depthmapHeight);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}