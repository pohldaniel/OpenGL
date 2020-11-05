#ifndef _MESHSPIRAL_H
#define _MESHSPIRA_H

#include <vector>
#include <memory>

#include "..\Texture.h"
#include "..\Extension.h"
#include "..\Camera.h"
#include "..\Shader.h"
#include "..\Vector.h"

class MeshSpiral {

public:

	MeshSpiral(float radius, float tubeRadius, int numRotations, float length, const std::string &texture);
	MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, const std::string &texture);
	MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture);
	~MeshSpiral();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);

private:

	int m_mainSegments;
	int m_tubeSegments;

	int m_numRotations;
	float m_length;
	bool m_repeatTexture;
	
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