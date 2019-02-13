#include "Depthmap.h"

Depthmap::Depthmap(Camera* camera){

	m_camera = camera;
	depthmapTexture = 0;

	m_shader = new DepthShader("shader/depth.vert", "shader/depth.frag");
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
}

Depthmap::~Depthmap(){

	if (m_shader){
		delete m_shader;
		m_shader = 0;
	}

	if (m_renderShader){
		delete m_renderShader;
		m_renderShader = 0;
	}
}

void Depthmap::createDepthmapFBO(){


	glGenFramebuffers(1, &depthmapFBO);
	glGenTextures(1, &depthmapTexture);
	glBindTexture(GL_TEXTURE_2D, depthmapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, depthmapWidth, depthmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Depthmap::setDepthViewport(unsigned int a_widht, unsigned int a_height){

	depthViewportWidth = a_widht;
	depthViewportHeight = a_height;
}

void Depthmap::setViewMatrix(const Vector3f &lightPos, const Vector3f &target, const Vector3f &up){

	
	Matrix4f view;
	view.lookAt(lightPos, target, up);
	m_viewMatrix = view;

	m_eye = lightPos;
}

void Depthmap::setProjectionMatrix(float fovx, float aspect, float znear, float zfar){


	Matrix4f projection;
	projection.perspective(fovx, (GLfloat)depthmapWidth / (GLfloat)depthmapHeight, znear, zfar);
	m_projMatrix = projection;

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_projection",   m_projMatrix);
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

const Matrix4f &Depthmap::getTransformation() const{

	return  m_modelMatrix.getInvTransformationMatrix();
}

const Vector3f &Depthmap::getPosition() const
{
	return m_eye;
}


void Depthmap::rotate(const Vector3f &axis, float degrees){

	m_modelMatrix.rotate(axis, degrees);

}

void Depthmap::translate(float dx, float dy, float dz){

	m_modelMatrix.translate(dx, dy, dz);

}



void Depthmap::renderToDepthTexture(Object const* object){

	
	glViewport(0, 0, depthViewportWidth, depthViewportHeight);

	glUseProgram(m_shader->m_program);
	
	

	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < object->m_model->numberOfMeshes(); i++){

		m_shader->loadMatrix("u_modelView", m_viewMatrix *  object->m_model->getTransformationMatrix());


		glBindBuffer(GL_ARRAY_BUFFER, object->m_model->getMesches()[i]->getVertexName());
		m_shader->bindAttributes(object->m_model->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->m_model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, object->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_shader->unbindAttributes(object->m_model->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glViewport(0, 0, viewportWidth, viewportHeight);

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
	m_renderShader->loadMatrix("u_modelView", m_camera->getViewMatrix() );

	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	m_renderShader->bindAttributes(texture);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexQuad);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_renderShader->unbindAttributes();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

}