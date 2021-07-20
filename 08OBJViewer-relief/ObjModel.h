#ifndef __OBJMODELH__
#define  __OBJMODELH__

#include <iostream>
#include <ctime>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Vector.h"
#include "Extension.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "ModelMatrix.h"

struct IndexBufferCreator {

	std::vector<std::array<int, 10>> face;
	std::vector<float> positionCoordsIn;
	std::vector<float> normalCoordsIn;
	std::vector<float> textureCoordsIn;

	std::vector <float> vertexBufferOut;
	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer();

private:
	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int stride);
};


class Mesh2;
class Model2 {
	friend Mesh2;
public:

	Model2();
	~Model2();

	void draw() const;
	void draw(const Camera camera) const;

	//size values
	unsigned int m_size, m_numVertices, m_numIndices, m_stride, m_offset, m_numberOfBytes;

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);
	void setRotXYZPos(const Vector3f &axisX, float degreesX,
		const Vector3f &axisY, float degreesY,
		const Vector3f &axisZ, float degreesZ,
		float dx, float dy, float dz);

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	bool loadObject(const char* filename);
	bool loadObject(const char* a_filename, Vector3f &rotate, float degree, Vector3f& translate, float scale);

	int getNumberOfIndices() const;

	std::string getMltPath();
	std::string getModelDirectory();

	void setShader(const char* vertex, const char* fragment);
	void setShader(Shader* shader);

	bool m_hasTextureCoords;
	bool m_hasNormals;
	bool m_hasTangents;

	std::vector<Mesh2*> m_mesh;

	int addVertex(int hash, float *pVertex, int stride);
	int m_numberOfMeshes;
	int m_numberOfTriangles;
	std::string m_mltPath;
	std::string m_modelDirectory;
	bool m_hasMaterial;
	Vector3f m_center;
	ModelMatrix *m_modelMatrix;

	std::vector <float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::map<int, std::vector<int>> m_vertexCache;
};

class Mesh2 {

	friend Model2;

public:
	///////////////////////////////geometry content
	struct Material {

		float ambient[3];
		float diffuse[3];
		float specular[3];
		float shinies;
		std::string diffuseTexPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
	};

	Mesh2(std::string mltName, int numberTriangles, Model2* model);
	Mesh2(int numberTriangles, Model2* model);
	~Mesh2();

	void draw();
	void draw(const Camera camera);

	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);
	Material getMaterial();
	bool readMaterial();
	void generateNormals();
	void generateTangents();

	Model2* m_model;
	std::string m_mltName;
	Material m_material;

	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride;
	///////////////////////////////////////OpenGL content//////////////////
	void createBuffer();
	void createTexture();

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[4];
	unsigned int m_drawCount;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

	std::vector <float> m_vertexBuffer;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
};
#endif