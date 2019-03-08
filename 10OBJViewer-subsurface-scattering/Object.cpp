#include "Object.h"

Object::Object(){

	m_numberOfShader = 0;

	


}

Object::~Object(){

	for (int i = 0; i < m_numberOfShader; i++){

		if (m_shader[i]){
			delete m_shader[i];
			m_shader[i] = 0;
		}
	}
	m_shader = 0;
}



void Object::initModel(const char* a_objPath){
	m_model = new Model();
	m_model->loadObject(a_objPath,Vector3f(0.0, 0.0, 0.0), 0.3);
	Bitmap* bitmap;
	for (int i = 0; i < m_model->numberOfMeshes(); i++){

		if (m_model->getMesches()[i]->readMaterial((m_model->getModelDirectory() + "/" + m_model->getMltPath()).c_str())){
			
			//Load the colorMap Texture
			if (m_model->getMesches()[i]->getMaterial().colorMapPath != ""){

				bitmap = new Bitmap();
				if (bitmap->loadBitmap24((m_model->getModelDirectory() + "/" +
					m_model->getMesches()[i]->getMaterial().colorMapPath).c_str())){

					glGenTextures(1, &m_model->getMesches()[i]->m_textureName);
					glBindTexture(GL_TEXTURE_2D, m_model->getMesches()[i]->m_textureName);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

					glGenerateMipmap(GL_TEXTURE_2D);

				}
				delete bitmap;
				bitmap = NULL;
			}
			//Load the normalMap Texture
			if (m_model->getMesches()[i]->getMaterial().bumpMapPath != ""){
				m_model->getMesches()[i]->generateTangents();

				bitmap = new Bitmap();
				if (bitmap->loadBitmap24((m_model->getModelDirectory() + "/" +
					m_model->getMesches()[i]->getMaterial().bumpMapPath).c_str())){

					glGenTextures(1, &m_model->getMesches()[i]->m_normalMap);
					glBindTexture(GL_TEXTURE_2D, m_model->getMesches()[i]->m_normalMap);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

					glGenerateMipmap(GL_TEXTURE_2D);

				}
				delete bitmap;
				bitmap = NULL;
			}

			//Load the diplacement map
			if (m_model->getMesches()[i]->getMaterial().displacementMapPath != ""){
				m_model->getMesches()[i]->generateTangents();

				bitmap = new Bitmap();
				if (bitmap->loadBitmap24((m_model->getModelDirectory() + "/" +
					m_model->getMesches()[i]->getMaterial().displacementMapPath).c_str())){



					glGenTextures(1, &m_model->getMesches()[i]->m_displacementMap);
					glBindTexture(GL_TEXTURE_2D, m_model->getMesches()[i]->m_displacementMap);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

					glGenerateMipmap(GL_TEXTURE_2D);

				}
				delete bitmap;
				bitmap = NULL;
			}
		// default material
		}else{
			
			m_model->getMesches()[i]->setMaterial(Vector3f(0.3, 0.3, 0.3), Vector3f(0.8, 0.8, 0.8), Vector3f(0.5, 0.5, 0.5), 20);
			m_model->getMesches()[i]->generateNormals();
		}
		//Bind buffer
		glGenBuffers(1, &m_model->getMesches()[i]->m_vertexName);
		glBindBuffer(GL_ARRAY_BUFFER, m_model->getMesches()[i]->m_vertexName);
		glBufferData(GL_ARRAY_BUFFER, m_model->getMesches()[i]->getNumberOfVertices() * 4, m_model->getMesches()[i]->getVertexBuffer(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_model->getMesches()[i]->m_indexName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_model->getMesches()[i]->m_indexName);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_model->getMesches()[i]->getNumberOfIndices() * 4, m_model->getMesches()[i]->getIndexBuffer(), GL_STATIC_DRAW);
	}

}

void Object::initShader(){

	m_shader = (Shader**)malloc(m_model->numberOfMeshes() * sizeof(Shader*));

	for (int i = 0; i < m_model->numberOfMeshes(); i++){


		if (m_model->getMesches()[i]->m_hasTangents && m_model->getMesches()[i]->getMaterial().displacementMapPath != ""){
			m_numberOfShader++;
			m_shader[i] = new Shader("shader/subsurface.vert", "shader/subsurface.frag");
			m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			
		}else if (m_model->getMesches()[i]->m_hasTangents && m_model->getMesches()[i]->getMaterial().bumpMapPath != "") {
			m_numberOfShader++;
			m_shader[i] = new Shader("shader/normal.vert", "shader/normal.frag");
			m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			
		}else if (m_model->getMesches()[i]->m_hasTextureCoords && m_model->getMesches()[i]->m_hasNormals){
			m_numberOfShader++;
			m_shader[i] = new Shader("shader/phongTexture.vert", "shader/phongTexture.frag");
			m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			
		}else if (m_model->getMesches()[i]->m_hasNormals){
			m_numberOfShader++;
			m_shader[i] = new Shader("shader/phongColor.vert", "shader/phongColor.frag");
			m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			
		}else{
			m_numberOfShader++;
			m_shader[i] = new Shader("shader/texture.vert", "shader/texture.frag");
			m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			
		}

	}

}

void Object::initShader(const char* a_vertexPath, const char* a_fragmentPath){

	m_shader = (Shader**)malloc(m_model->numberOfMeshes() * sizeof(Shader*));

	
			for (int i = 0; i < m_model->numberOfMeshes(); i++){
				m_numberOfShader++;
				m_shader[i] = new Shader(a_vertexPath, a_fragmentPath);
				m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
			}
		

}

void Object::initShader(Shader * shader){

	m_shader = (Shader**)malloc(m_model->numberOfMeshes() * sizeof(Shader*));
	

	for (int i = 0; i < m_model->numberOfMeshes(); i++){
		m_numberOfShader++;
		m_shader[i] = shader;
		m_shader[i]->loadMaterial(m_model->getMesches()[i]->getMaterial());
	}
}