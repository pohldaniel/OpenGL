#ifndef _MODEL_H
#define _MODEL_H

#include <iostream>
#include <vector>
#include <cfloat>
#include <string>
#include <array>
#include <fstream>
#include <map>
#include <algorithm>

#include "Vector.h"


class Mesh;
class Model{

public:

	Model();
	~Model();

	bool loadObject(const char* filename);
	bool loadObject(const char* filename, Vector3f& translate, float scale);

	std::vector<Mesh*> getMesches();

	std::string getMltPath();
	std::string getModelDirectory();

	int numberOfMeshes();
	bool hasTextureCoords() const;
	bool hasNormals() const;
	bool hasMaterial() const;

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	
private:

	bool m_hasTextureCoords;
	bool m_hasNormals;
	bool m_hasMaterial;
	int m_numberOfMeshes;

	std::string m_mltPath;
	std::string m_modelDirectory;
	
	

	std::vector<Mesh*> mesh;

	Matrix4f T;
	Matrix4f invT;
};

class Mesh{

	friend Model;

public:

	struct Material
	{
		float ambient[4];
		float diffuse[4];
		float specular[4];

		std::string colorMapPath;
		std::string bumpMapPath;
	};
	
	Mesh();

	Mesh(std::string mltName, int numberTriangles);

	~Mesh();

	
	bool readMaterial(const char* filename);

	const int *getIndexBuffer() const;
	const float *getVertexBuffer() const;
	

	int getNumberOfIndices() const;
	int getNumberOfVertices() const;
	int getNumberOfTriangles() const;
	

	
	std::string getMltName();
	

	Material getMaterial();

	unsigned int getVertexName();
	unsigned int getIndexName();
	unsigned int getTextureName();

	void setVertexName(unsigned int a_vertexName);
	void setIndexName(unsigned int a_indexName);
	void setTextureName(unsigned int a_textureName);
	void setColor(const Vector3f &position);

	const Vector3f &getColor() const;

private:

	unsigned int m_vertexName;
	unsigned int m_indexName;
	unsigned int m_textureName;
	
	std::vector <float> m_vertexBuffer;
	std::map<int, int > m_vertexCache;
	std::vector<int> m_indexBuffer;
	
	
	int m_numberTriangles;
	

	int addVertex(int hash, const float *pVertex, int n);
	bool mltCompare(std::string* mltName);
	Vector3f m_color;
	
	std::string m_mltName;

	Material m_material;

	float	xmin;
	float	xmax;
	float	ymin;
	float	ymax;
	float	zmin;
	float	zmax;

	
};



#endif