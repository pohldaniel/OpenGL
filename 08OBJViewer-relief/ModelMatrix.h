#ifndef _MODELMATRIX_H
#define _MODELMATRIX_H

#include "Vector.h"

class ModelMatrix{

public:

	ModelMatrix();
	~ModelMatrix();

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);

	void setRotXYZPos(const Vector3f &axisX, float degreesX, 
					  const Vector3f &axisY, float degreesY,
					  const Vector3f &axisZ, float degreesZ,
					  float dx, float dy, float dz);

	Matrix4f orientation;
	Vector3f position;

private:

	Matrix4f startOrientation;
	Vector3f startPosition;
	bool pos;
	bool posRot;

	Matrix4f T;
	Matrix4f invT;
};


#endif