#include "Depthmap.h"

Depthmap::Depthmap(Camera* camera){

	m_camera = camera;
	depthmapTexture = 0;

	m_depthShader = new Shader("shader/depth.vert", "shader/depth.frag");
	m_normalShader = new Shader("shader/normalMap.vert", "shader/normalMap.frag");
	m_irradianceShader = new Shader("shader/irradiance.vert", "shader/irradiance.frag");

	m_renderShader = new TextureShader("shader/texture.vert", "shader/texture.frag");

	glUseProgram(m_renderShader->m_program);
	m_renderShader->loadMatrix("u_projection", camera->getProjectionMatrix());
	glUseProgram(0);

	createDepthmapFBO();
	createBuffer();

	m_biasMatrix = Matrix4f( 0.5, 0.0, 0.0, 0.0,
							 0.0, 0.5, 0.0, 0.0,
							 0.0, 0.0, 0.5, 0.0,
							 0.5, 0.5, 0.5, 1.0 );


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
}

void Depthmap::createDepthmapFBO(){

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_2D, irradianceMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int irradianceRboColor;
	unsigned int irradianceDepth;
	unsigned int irradianceRbo;

	glGenFramebuffers(1, &irradianceFBOMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBOMSAA);

	// create a MSAA renderbuffer object to store color info
	glGenRenderbuffers(1, &irradianceRboColor);
	glBindRenderbuffer(GL_RENDERBUFFER, irradianceRboColor);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &irradianceDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, irradianceDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach msaa RBOs to FBO attachment points
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, irradianceRboColor);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irradianceDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// create a normal (no MSAA) FBO to hold a render-to-texture
	glGenFramebuffers(1, &irradianceFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
	glGenRenderbuffers(1, &irradianceRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, irradianceRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, irradianceMap, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irradianceRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	////////////////////////////////////////////////////////////////////

	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int normalRboColor;
	unsigned int normalDepth;
	unsigned int normalRbo;

	glGenFramebuffers(1, &normalFBOMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, normalFBOMSAA);

	// create a MSAA renderbuffer object to store color info
	glGenRenderbuffers(1, &normalRboColor);
	glBindRenderbuffer(GL_RENDERBUFFER, normalRboColor);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &normalDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, normalDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach msaa RBOs to FBO attachment points
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, normalRboColor);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, normalDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// create a normal (no MSAA) FBO to hold a render-to-texture
	glGenFramebuffers(1, &normalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, normalFBO);
	glGenRenderbuffers(1, &normalRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, normalRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, normalRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &normalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, normalFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glGenTextures(1, &depthmapTexture);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depthmapWidth, depthmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int depthRboColor;
	unsigned int depthDepth;
	unsigned int depthRbo;

	glGenFramebuffers(1, &depthmapFBOMSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBOMSAA);

	// create a MSAA renderbuffer object to store color info
	glGenRenderbuffers(1, &depthRboColor);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRboColor);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &depthDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, depthDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach msaa RBOs to FBO attachment points
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, depthRboColor);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// create a normal (no MSAA) FBO to hold a render-to-texture
	glGenFramebuffers(1, &depthmapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	glGenRenderbuffers(1, &depthRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmapTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	/*glGenFramebuffers(1, &depthmapFBO);
	glGenTextures(1, &depthmapTexture);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	/////////////////////////////////////////////////////////////////////////////////////////////

	// create a texture object
	glGenTextures(1, &depthmapTexture2);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int rboId;

	glGenFramebuffers(1, &depthmapFBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO2);
	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthmapWidth, depthmapHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture2, 0);

	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	/*glGenFramebuffers(1, &depthmapFBO2);
	glGenTextures(1, &depthmapTexture2);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture2, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);*/
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
	projection.perspective(fovx, (GLfloat)depthmapWidth / (GLfloat)depthmapHeight, znear, zfar);
	m_projMatrix = projection;

	glUseProgram(m_depthShader->m_program);
	m_depthShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_normalShader->m_program);
	m_normalShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

	glUseProgram(m_irradianceShader->m_program);
	m_irradianceShader->loadMatrix("u_projection", m_projMatrix);
	glUseProgram(0);

}

const Matrix4f &Depthmap::getDepthPassMatrix() const{

	return  m_biasMatrix * m_projMatrix;
}

const Matrix4f &Depthmap::getProjectionMatrix() const{

	return  m_projMatrix;
}


const Matrix4f &Depthmap::getViewMatrix() const{

	return m_viewMatrix;
}


const Vector3f &Depthmap::getPosition() const{

	return m_eye;
}



void Depthmap::renderToDepthTexture(Object const* object){

	
	glUseProgram(m_depthShader->m_program);
	
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBOMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_depthShader->loadMatrix("u_modelView", m_viewMatrix *  object->m_model->getTransformationMatrix());


		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_depthShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, depthmapFBOMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthmapFBO);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);                           // scale filter


	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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

		m_depthShader->loadMatrix("u_modelView", m_viewMatrix *  object->m_model->getTransformationMatrix());


		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_depthShader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_depthShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glViewport(0, 0, viewportWidth, viewportHeight);


	glBindTexture(GL_TEXTURE_2D, depthmapTexture2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


}

void Depthmap::renderNormalMap(Object const* object){

	glUseProgram(m_normalShader->m_program);

	
	glBindFramebuffer(GL_FRAMEBUFFER, normalFBOMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_normalShader->loadMatrix("u_modelView", m_viewMatrix *  object->m_model->getTransformationMatrix());
		m_normalShader->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(m_viewMatrix *  object->m_model->getTransformationMatrix()));

		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_normalShader->bindAttributes(object->m_model->getMesches()[i], NULL);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_normalShader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, normalFBOMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, normalFBO);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);                           // scale filter

	
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Depthmap::renderIrradianceMap(Object const* object){

	glUseProgram(m_irradianceShader->m_program);


	glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBOMSAA);
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_irradianceShader->loadMatrix("u_modelView", m_viewMatrix *  object->m_model->getTransformationMatrix());
		m_irradianceShader->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(m_viewMatrix *  object->m_model->getTransformationMatrix()));

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


	glBindFramebuffer(GL_READ_FRAMEBUFFER, irradianceFBOMSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, irradianceFBO);
	glBlitFramebuffer(0, 0, depthmapWidth, depthmapHeight, 0, 0, depthmapWidth, depthmapHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);                           // scale filter


	glBindTexture(GL_TEXTURE_2D, irradianceMap);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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