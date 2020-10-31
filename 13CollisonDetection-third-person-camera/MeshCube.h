#ifndef _MESHCUBE_H
#define _MESHCUBE_H

#include <vector>
#include <memory>

#include "Animation\AnimatedModel\Texture.h"
#include "Extension.h"
#include "Camera.h"
#include "Shader.h"
#include "Bitmap.h"
#include "Vector.h"

class MeshCube {

public:

	MeshCube(float width, float height, float depth, const std::string &texture);
	MeshCube(const Vector3f &position, float width, float height, float depth, const std::string &texture);
	MeshCube(const Vector3f &position, float width, float height, float depth, bool generateTexels, const std::string &texture);
	~MeshCube();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void buildMesh4Q();
	void draw(const Camera camera);

private:

	int m_uResolution;
	int m_vResolution;

	float m_width;
	float m_height;
	float m_depth;
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