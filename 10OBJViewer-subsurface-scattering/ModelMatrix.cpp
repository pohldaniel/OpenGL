#include <iostream>
#include "ModelMatrix.h"

ModelMatrix::ModelMatrix(){
	pos = false;
	T.identity();
	invT.identity();

	startPosition = Vector3f(0.0, 0.0, 0.0);
}

ModelMatrix::~ModelMatrix(){

}

void ModelMatrix::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz){

	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

	T =  rotMtx * T;
	invT = invT * invRotMtx;


	
	//T = T * Translate
	T[0][0] = T[0][0] + T[0][3] * dx; T[0][1] = T[0][1] + T[0][3] * dz; T[0][2] = T[0][2] + T[0][3] * dy;
	T[1][0] = T[1][0] + T[1][3] * dx; T[1][1] = T[1][1] + T[1][3] * dz; T[1][2] = T[1][2] + T[1][3] * dy;
	T[2][0] = T[2][0] + T[2][3] * dx; T[2][1] = T[2][1] + T[2][3] * dz; T[2][2] = T[2][2] + T[2][3] * dy;
	T[3][0] = T[3][0] + dx; T[3][1] = T[3][1] + dy; T[3][2] = T[3][2] + dz;

	//T^-1 = Translate^-1 * T^-1 
	invT[3][0] = invT[3][0] - dx*invT[0][0] - dz*invT[2][0] - dy*invT[1][0];
	invT[3][1] = invT[3][1] - dx*invT[0][1] - dz*invT[2][1] - dy*invT[1][1];
	invT[3][2] = invT[3][2] - dx*invT[0][2] - dz*invT[2][2] - dy*invT[1][2];

	startPosition = Vector3f(dx, dy, dz);
	position = Vector3f(dx, dy, dz);
	startOrientation = rotMtx;
	orientation = rotMtx;

	pos = true;

}

void ModelMatrix::rotate(const Vector3f &axis, float degrees){

	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	orientation = rotMtx * orientation;

	Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

	if (startPosition.zero()){
		
		T = rotMtx * T;
		invT = invT * invRotMtx;

	}else{
		
		
		//invT =invT *  translate *rotMtx * invTranslate  
		float tmp1 = invRotMtx[3][0], tmp2 = invRotMtx[3][1], tmp3 = invRotMtx[3][2], tmp4 = invRotMtx[3][3];
		invRotMtx[3][0] = startPosition[0] * (tmp4 - invRotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * invRotMtx[1][0] - startPosition[2] * invRotMtx[2][0];
		invRotMtx[3][1] = startPosition[1] * (tmp4 - invRotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[0][1] - startPosition[2] * invRotMtx[2][1];
		invRotMtx[3][2] = startPosition[2] * (tmp4 - invRotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[0][2] - startPosition[1] * invRotMtx[1][2];
		invRotMtx[3][3] = startPosition[0] * (tmp1 - invRotMtx[0][3]) + startPosition[1] * (tmp2 - invRotMtx[1][3]) + startPosition[2] * (tmp3 - invRotMtx[2][3]) + tmp4;
		invT =  invT * invRotMtx;
		
		//T =  translate *rotMtx * invTranslate * T 
		
		tmp1 = rotMtx[3][0], tmp2 = rotMtx[3][1], tmp3 = rotMtx[3][2], tmp4 = rotMtx[3][3];
		rotMtx[3][0] = startPosition[0] * (tmp4 - rotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * rotMtx[1][0] - startPosition[2] * rotMtx[2][0];
		rotMtx[3][1] = startPosition[1] * (tmp4 - rotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[0][1] - startPosition[2] * rotMtx[2][1];
		rotMtx[3][2] = startPosition[2] * (tmp4 - rotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[0][2] - startPosition[1] * rotMtx[1][2];
		rotMtx[3][3] = startPosition[0] * (tmp1 - rotMtx[0][3]) + startPosition[1] * (tmp2 - rotMtx[1][3]) + startPosition[2] * (tmp3 - rotMtx[2][3]) + tmp4;
		
		T = rotMtx * T;
	}
}

void ModelMatrix::translate(float dx, float dy, float dz){

	position = position + Vector3f(dx, dy, dz);

	T[0][0] = T[0][0] + T[0][3] * dx; T[0][1] = T[0][1] + T[0][3] * dz; T[0][2] = T[0][2] + T[0][3] * dy;
	T[1][0] = T[1][0] + T[1][3] * dx; T[1][1] = T[1][1] + T[1][3] * dz; T[1][2] = T[1][2] + T[1][3] * dy;
	T[2][0] = T[2][0] + T[2][3] * dx; T[2][1] = T[2][1] + T[2][3] * dz; T[2][2] = T[2][2] + T[2][3] * dy;
	T[3][0] = T[3][0] + dx; T[3][1] = T[3][1] + dy; T[3][2] = T[3][2] + dz;


	invT[3][0] = invT[3][0] - dx*invT[0][0] - dz*invT[2][0] - dy*invT[1][0];
	invT[3][1] = invT[3][1] - dx*invT[0][1] - dz*invT[2][1] - dy*invT[1][1];
	invT[3][2] = invT[3][2] - dx*invT[0][2] - dz*invT[2][2] - dy*invT[1][2];

}

void ModelMatrix::scale(float a, float b, float c){

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	T[0][0] = T[0][0] * a;  T[1][0] = T[1][0] * b; T[2][0] = T[2][0] * c;
	T[0][1] = T[0][1] * a;  T[1][1] = T[1][1] * b; T[2][1] = T[2][1] * c;
	T[0][2] = T[0][2] * a;  T[1][2] = T[1][2] * b; T[2][2] = T[2][2] * c;


	invT[0][0] = invT[0][0] * (1.0 / a); invT[0][1] = invT[0][1] * (1.0 / b); invT[0][2] = invT[0][2] * (1.0 / c);
	invT[1][0] = invT[1][0] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[1][2] = invT[1][2] * (1.0 / c);
	invT[2][0] = invT[2][0] * (1.0 / a); invT[2][1] = invT[2][1] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	invT[3][0] = invT[3][0] * (1.0 / a); invT[3][1] = invT[3][1] * (1.0 / b); invT[3][2] = invT[3][2] * (1.0 / c);
}

const Matrix4f &ModelMatrix::getTransformationMatrix() const{

	return T;
}

const Matrix4f &ModelMatrix::getInvTransformationMatrix() const{

	return invT;

}