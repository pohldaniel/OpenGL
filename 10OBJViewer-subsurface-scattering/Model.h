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
#include "ModelMatrix.h"

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
	bool hasMaterial() const;

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

	bool m_hasTextureCoords;
	bool m_hasNormals;
	bool m_hasTangents;

	

private:

	void indexVBO_P(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices);
	void indexVBO_PN(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices);
	void indexVBO_PT(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices);
	void indexVBO_PTN(std::vector<float> & in_vertices, std::vector<int> & out_indices, std::vector<float> & out_vertices);

	
	bool m_hasMaterial;
	int m_numberOfMeshes;

	std::string m_mltPath;
	std::string m_modelDirectory;
	
	ModelMatrix *modelMatrix;

	std::vector<Mesh*> mesh;

	
	Vector3f m_center;
	
};

class Mesh{

	friend Model;

public:

	bool m_hasTextureCoords;
	bool m_hasNormals;
	bool m_hasTangents;

	struct Material{

		float ambient[3];
		float diffuse[3];
		float specular[3];
		float shinies;
		std::string colorMapPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
	};
	
	Mesh();

	Mesh(std::string mltName, int numberTriangles);

	Mesh(int numberTriangles);

	~Mesh();

	
	void generateNormals();
	void generateTangents();

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
	unsigned int getNormalMap();
	unsigned int getDisplacementMap();

	void setVertexName(unsigned int a_vertexName);
	void setIndexName(unsigned int a_indexName);
	void setTextureName(unsigned int a_textureName);
	void setNormalMap(unsigned int a_textureName);
	void setColor(const Vector3f &position);
	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);

	const Vector3f &getColor() const;

	std::map<int, int > m_vertexCache;
	int addVertex(int hash, const float *pVertex, int n);


	unsigned int m_vertexName;
	unsigned int m_indexName;
	unsigned int m_textureName;
	unsigned int m_normalMap;
	unsigned int m_displacementMap;
private:

	std::vector <float> m_vertexBuffer;
	std::vector<int> m_indexBuffer;
	
	
	int m_numberTriangles;
	

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