#ifndef _MESHSPHERE_H
#define _MESHSPHERE_H

#include <vector>
#include <memory>

#include "..\Texture.h"
#include "..\Extension.h"
#include "..\Camera.h"
#include "..\Shader.h"
#include "..\Vector.h"
#include "..\Entity3D.h"


class MeshSphere {

public:

	MeshSphere(float radius, const std::string &texture);
	MeshSphere(float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture);
	~MeshSphere();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera, Entity3D entity);

private:

	int m_uResolution;
	int m_vResolution;
	float m_radius;
	float m_invRadius;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;
	bool m_generateNormalDerivatives;

	bool m_isInitialized;
	bool m_hasTexels;
	bool m_hasNormals;
	bool m_hasTangents;
	bool m_hasNormalDerivatives;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[4];
	unsigned int m_drawCount;

	ModelMatrix m_modelMatrix;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;
};
#endif