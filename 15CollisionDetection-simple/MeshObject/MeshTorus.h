#ifndef _MESHTORUS_H
#define _MESHTORUS_H

#include <vector>
#include <memory>

#include "..\Texture.h"
#include "..\Extension.h"
#include "..\Camera.h"
#include "..\Shader.h"
#include "..\Vector.h"

class MeshTorus {

public:

	MeshTorus(float radius, float tubeRadius, const std::string &texture);
	MeshTorus(const Vector3f &position, float radius, float tubeRadius, const std::string &texture);
	MeshTorus(const Vector3f &position, float radius, float tubeRadius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture);
	~MeshTorus();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;

private:

	int m_mainSegments;
	int m_tubeSegments;
	float m_radius;
	float m_tubeRadius;
	Vector3f m_position;

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