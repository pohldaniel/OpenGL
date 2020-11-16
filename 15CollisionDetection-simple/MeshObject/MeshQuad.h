#ifndef _MESHQUAD_H
#define _MESHQUAD_H

#include <vector>
#include <memory>

#include "..\Texture.h"
#include "..\Extension.h"
#include "..\Camera.h"
#include "..\Shader.h"
#include "..\Vector.h"

class MeshQuad {

public:

	MeshQuad(float width, float height, const std::string &texture);
	MeshQuad(float width, float height, bool generateTexels, const std::string &texture);
	~MeshQuad();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;

private:

	int m_uResolution;
	int m_vResolution;
	float m_width;
	float m_height;

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