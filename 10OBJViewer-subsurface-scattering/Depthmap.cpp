#include "Depthmap.h"

Depthmap::Depthmap(Camera* camera){

	m_camera = camera;
	

	m_depthShader = new Shader("shader/depth.vert", "shader/depth.frag");
	m_normalShader = new Shader("shader/normalMap.vert", "shader/normalMap.frag");
	m_irradianceShader = new Shader("shader/irradiance.vert", "shader/irradiance.frag");
	m_depthMapShader = new Shader("shader/depthMap.vert", "shader/depthMap.frag");
	m_depthMapShader2 = new Shader("shader/depthMap2.vert", "shader/depthMap2.frag");

	thickness = new Shader("shader/thickness.vsh", "shader/thickness.fsh");

	m_renderShader = new TextureShader("shader/texture.vert", "shader/texture.frag");

	glUseProgram(m_renderShader->m_program);
	m_renderShader->loadMatrix("u_projection", camera->getProjectionMatrix());
	glUseProgram(0);

	createDepthmapFBO();
	createBuffer();

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
	normalData = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);
	irradianceData = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);
	depthData = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);
	depthData2 = (unsigned char*)malloc(depthmapWidth * depthmapHeight);
	depthData3 = (unsigned char*)malloc(depthmapWidth * depthmapHeight);
	depthData4 = (unsigned char*)malloc(depthmapWidth * depthmapHeight);
	depthData5 = (unsigned char*)malloc(3 * depthmapWidth * depthmapHeight);

}

Depthmap::~Depthmap(){

	if (m_depthShader){
		delete m_depthShader;
		m_depthShader = 0;
	}

	if (m_normalShader){
		delete m_normalShader;
		m_normalShader = 0;
	}

	if (m_irradianceShader){
		delete m_irradianceShader;
		m_irradianceShader = 0;
	}

	if (m_renderShader){
		delete m_renderShader;
		m_renderShader = 0;
	}

	if (m_depthMapShader){
		delete m_depthMapShader;
		m_depthMapShader = 0;
	}

	if (m_depthMapShader2){
		delete m_depthMapShader;
		m_depthMapShader = 0;
	}

	if (normalData){
		free(normalData);
		normalData = NULL;
	}

	if (irradianceData){
		free(irradianceData);
		normalData = NULL;
	}

	if (depthData){
		free(depthData);
		depthData = NULL;
	}

	if (depthData2){
		free(depthData2);
		depthData2 = NULL;
	}

	if (depthData3){
		free(depthData3);
		depthData3 = NULL;
	}

	if (depthData4){
		free(depthData4);
		depthData4 = NULL;
	}

	if (depthData5){
		free(depthData4);
		depthData4 = NULL;
	}
}

void Depthmap::createDepthmapFBO(){

	////////////////////////////irradianceMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_2D, irradianceMap);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, irradianceMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////depthMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &depthmapTexture3);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//////////////////////////setup MSAA framebuffer///////////////////////////////////////
	unsigned int colorBuf0;
	glGenRenderbuffers(1, &colorBuf0);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuf0);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	unsigned int depthBuf0;
	glGenRenderbuffers(1, &depthBuf0);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf0);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &fboCDepthMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCDepthMSAA);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuf0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////end setup MSAA framebuffer///////////////////////////////////////

	//////////////////////////setup normal (no MSAA) FBOs as blit target///////////////////////
	glGenFramebuffers(1, &fboCDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, fboCDepth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmapTexture3, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////depthMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &depthmapTexture2);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &depthmapFBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture2, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////normalMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	
	glGenTextures(1, &depthmapTexture);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &fboDNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDNormal);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	////////////////////////////depthMap rendertarget//////////////////////////////////////////////
	glGenTextures(1, &depthmapTexture4);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//////////////////////////setup MSAA framebuffer///////////////////////////////////////

	unsigned int depthBuf2;
	glGenRenderbuffers(1, &depthBuf2);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf2);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glGenFramebuffers(1, &fboDDepthMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDDepthMSAA);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf2);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////end setup MSAA framebuffer///////////////////////////////////////

	//////////////////////////setup normal (no MSAA) FBOs as blit target///////////////////////
	glGenFramebuffers(1, &fboDDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDDepth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture4, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////setup MSAA lumiance framebuffer///////////////////////////////////////
	glGenTextures(1, &singleChannel);
	glBindTexture(GL_TEXTURE_2D, singleChannel);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, singleChannel, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////

	
	

	glGenFramebuffers(1, &sssFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, sssFbo);

	glGenTextures(1, &sssTexture);
	glBindTexture(GL_TEXTURE_2D, sssTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, depthmapWidth, depthmapHeight, 0, GL_RGB, GL_FLOAT, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sssTexture, 0);

	//GLuint depthRenderbuffer;
	//glGenRenderbuffers(1, &depthRenderbuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, WINDOW_WIDTH, WINDOW_HEIGHT);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("failed to make complete framebuffer object %x", status);
		//assert(false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

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

	glUseProgram(m_depthShader->m_program);
	m_depthShader->loadMatrix("u_projection", m_projMatrixD3D);
	glUseProgram(0);

	glUseProgram(m_normalShader->m_program);
	m_normalShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_irradianceShader->m_program);
	m_irradianceShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);


	glUseProgram(m_depthMapShader->m_program);
	m_depthMapShader->loadMatrix("u_projection", m_linearProjMatrixD3D);
	glUseProgram(0);

	glUseProgram(m_depthMapShader2->m_program);
	m_depthMapShader2->loadMatrix("u_projection", m_projMatrixD3D);
	glUseProgram(0);

	glUseProgram(thickness->m_program);
	thickness->loadMatrix("u_projection", m_projMatrixD3D);
	glUseProgram(0);

}



void Depthmap::setOrthMatrix(float left, float right, float bottom, float top, float znear, float zfar){

	Matrix4f projection;
	projection.orthographic(left, right, bottom, top, znear, zfar);
	m_orthMatrix = projection;
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


const Matrix4f &Depthmap::getViewMatrix() const{

	return m_viewMatrix;
}


const Vector3f &Depthmap::getPosition() const{

	return m_eye;
}



void Depthmap::renderToDepthTexture(Object const* object){

	
	glUseProgram(m_depthShader->m_program);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fboCDepthMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_depthShader->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);


		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_depthShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboCDepthMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCDepth);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);   

	/*glBindFramebuffer(GL_FRAMEBUFFER, fboCDepth);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_BGR, GL_UNSIGNED_BYTE, depthData);
	Bitmap::saveBitmap24("depth3.bmp", depthData, depthmapWidth, depthmapHeight);*/

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void Depthmap::renderToDepthTexture2(Object const* object){

	glViewport(0, 0, depthViewportWidth, depthViewportHeight);

	glUseProgram(m_depthShader->m_program);



	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO2);
	glClear(GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);


	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_depthShader->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);


		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_depthShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
	glViewport(0, 0, viewportWidth, viewportHeight);


	/*glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData3);
	Bitmap::saveBitmap8("depth2.bmp", depthData3, depthmapWidth, depthmapHeight);*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Depthmap::renderToDepthTexture3(Object const* object){

	glUseProgram(m_depthMapShader->m_program);


	glBindFramebuffer(GL_FRAMEBUFFER, fboDDepthMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_depthMapShader->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() *m_viewMatrix);
		m_depthMapShader->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(object->m_model->getTransformationMatrix() *m_viewMatrix));
		m_depthMapShader->loadMatrix("u_view", m_viewMatrix );

		m_depthMapShader->loadMatrix("u_model", object->m_model->getTransformationMatrix());

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_depthMapShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthMapShader->unbindAttributes(object->m_model->getMesches()[i]);

	}
	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);


	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboDDepthMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDDepth);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	/*glBindFramebuffer(GL_FRAMEBUFFER, fboDDepth);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData4);
	Bitmap::saveBitmap8("depth5.bmp", depthData4, depthmapWidth, depthmapHeight);*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Depthmap::renderToDepthTexture4(Object const* object){

	glUseProgram(m_depthMapShader2->m_program);


	glBindFramebuffer(GL_FRAMEBUFFER, fboSingleChannelMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_depthMapShader2->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_normalShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthMapShader2->unbindAttributes(object->m_model->getMesches()[i]);

	}
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);


	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboSingleChannelMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCSingleChannel);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);                      


	/*glBindFramebuffer(GL_FRAMEBUFFER, fboCSingleChannel);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_RED, GL_UNSIGNED_BYTE, depthData5);
	Bitmap::saveBitmap24("depth6.bmp", depthData5, depthmapWidth, depthmapHeight);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void Depthmap::renderNormalMap(Object const* object){

	glUseProgram(m_normalShader->m_program);

	
	glBindFramebuffer(GL_FRAMEBUFFER, fboNormalMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_normalShader->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);
		m_normalShader->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(object->m_model->getTransformationMatrix() * m_viewMatrix));

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_normalShader->bindAttributes(object->m_model->getMesches()[i], NULL);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_normalShader->unbindAttributes(object->m_model->getMesches()[i]);
		
	}
	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


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
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData2);
	Bitmap::saveBitmap8("depth.bmp", depthData2, depthmapWidth, depthmapHeight);*/
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void Depthmap::renderIrradianceMap(Object const* object){

	glUseProgram(m_irradianceShader->m_program);


	glBindFramebuffer(GL_FRAMEBUFFER, fboIrradianceMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_irradianceShader->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);
		m_irradianceShader->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(object->m_model->getTransformationMatrix() * m_viewMatrix));

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_irradianceShader->bindAttributes(object->m_model->getMesches()[i], texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_irradianceShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIrradianceMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboCIrradiance);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);


	/*glBindFramebuffer(GL_FRAMEBUFFER, fboCIrradiance);
	glReadPixels(0, 0, depthmapWidth, depthmapHeight, GL_BGR, GL_UNSIGNED_BYTE, irradianceData);
	Bitmap::saveBitmap24("irradiance.bmp", irradianceData, depthmapWidth, depthmapHeight);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Depthmap::renderSSS(Object const* object){

	glBindFramebuffer(GL_FRAMEBUFFER, sssFbo);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(thickness->m_program);

	thickness->loadMatrix("u_modelView", object->m_model->getTransformationMatrix() * m_viewMatrix);


	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());

		thickness->bindAttributes(object->m_model->getMesches()[i], NULL);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		thickness->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


	}

	glDisable(GL_BLEND);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//debug purpose
void Depthmap::createBuffer(){

	m_vertex.push_back(1.0 * m_size); m_vertex.push_back(1.0 * m_size); m_vertex.push_back(-3.0 * m_size); m_vertex.push_back(1.0); m_vertex.push_back(1.0);
	m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(1.0 * m_size); m_vertex.push_back(-3.0 * m_size); m_vertex.push_back(0.0); m_vertex.push_back(1.0);
	m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(-3.0 * m_size); m_vertex.push_back(0.0); m_vertex.push_back(0.0);	
	m_vertex.push_back(1.0 * m_size); m_vertex.push_back(-1.0 * m_size); m_vertex.push_back(-3.0 * m_size); m_vertex.push_back(1.0); m_vertex.push_back(0.0);
	

	glGenBuffers(1, &m_quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(float), &m_vertex[0], GL_STATIC_DRAW);

	static const GLushort index[] = {
		
		0, 1, 2,
		0, 2, 3
	};

	glGenBuffers(1, &m_indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

}



void Depthmap::render(unsigned int texture){

	

	glUseProgram(m_renderShader->m_program);
	m_renderShader->loadMatrix("u_modelView", m_camera->getViewMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	m_renderShader->bindAttributes(texture);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexQuad);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_renderShader->unbindAttributes();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

}