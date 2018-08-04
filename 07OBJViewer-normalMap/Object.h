#ifndef __objectH__
#define __objectH__

#include "Bitmap.h"
#include "Shader.h"
#include "Model.h"




class Object{

public:


	GLuint program;


	Shader **m_shader;
	Model *m_model;



	Object();
	~Object();


	void initModel(const char* a_objPath);
	void initShader(const char* a_vertexPath, const char* a_fragmentPath, SelectedShader selectedShader);
	void initShader();



	void render();
	

private:
	int m_numberOfShader;
	Material m_material;
};


#endif // __objectH__