#include "vector.h"
#include <iostream>

Matrix4f::Matrix4f() {}
Matrix4f::~Matrix4f() {}

const Matrix4f Matrix4f::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

void Matrix4f::rotate(const Vector3f &axis, float degrees) {

	float rad = (degrees * PI) / 180.0f;
	float magnitude = axis.length();

	float x = axis[0] * (1.0 / magnitude);
	float y = axis[1] * (1.0 / magnitude);
	float z = axis[2] * (1.0 / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[1][0] = (x * y) * (1.0f - c) + (z * s);
	mtx[2][0] = (x * z) * (1.0f - c) - (y * s);
	mtx[3][0] = 0.0f;

	mtx[0][1] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[2][1] = (y * z) * (1.0f - c) + (x * s);
	mtx[3][1] = 0.0f;

	mtx[0][2] = (z * x) * (1.0f - c) + (y * s);
	mtx[1][2] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(const Vector3f &axis, float degrees) {

	float rad = (degrees * PI) / 180.0f;
	float magnitude = axis.length();

	float x = axis[0] * (1.0 / magnitude);
	float y = axis[1] * (1.0 / magnitude);
	float z = axis[2] * (1.0 / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);



	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
	mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
	mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::translate(float dx, float dy, float dz) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = 0.0f;

	mtx[0][3] = dx;
	mtx[1][3] = dy;
	mtx[2][3] = dz;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invTranslate(float dx, float dy, float dz) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = 0.0f;

	mtx[0][3] = -dx;
	mtx[1][3] = -dy;
	mtx[2][3] = -dz;
	mtx[3][3] = 1.0f;
}


void Matrix4f::scale(float a, float b, float c) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f * a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f * b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f * c;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}


void Matrix4f::invScale(float a, float b, float c) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f / a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f / b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f / c;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {

	Vector3f zAxis = eye - target;
	Vector3f::Normalize(zAxis);

	Vector3f xAxis = Vector3f::Cross(up, zAxis);
	Vector3f::Normalize(xAxis);

	Vector3f yAxis = Vector3f::Cross(zAxis, xAxis);
	Vector3f::Normalize(yAxis);

	mtx[0][0] = xAxis[0];
	mtx[1][0] = yAxis[0];
	mtx[2][0] = zAxis[0];
	mtx[3][0] = 0.0f;

	mtx[0][1] = xAxis[1];
	mtx[1][1] = yAxis[1];
	mtx[2][1] = zAxis[1];
	mtx[3][1] = 0.0f;

	mtx[0][2] = xAxis[2];
	mtx[1][2] = yAxis[2];
	mtx[2][2] = zAxis[2];
	mtx[3][2] = 0.0f;

	mtx[0][3] = -Vector3f::Dot(xAxis, eye);
	mtx[1][3] = -Vector3f::Dot(yAxis, eye);
	mtx[2][3] = -Vector3f::Dot(zAxis, eye);
	mtx[3][3] = 1.0f;

}

void Matrix4f::invLookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {

	Vector3f zAxis = eye - target;
	Vector3f::Normalize(zAxis);

	Vector3f xAxis = Vector3f::Cross(up, zAxis);
	Vector3f::Normalize(xAxis);

	Vector3f yAxis = Vector3f::Cross(zAxis, xAxis);
	Vector3f::Normalize(yAxis);

	mtx[0][0] = xAxis[0];
	mtx[1][0] = xAxis[1];
	mtx[2][0] = xAxis[2];
	mtx[3][0] = 0.0f;

	mtx[0][1] = yAxis[0];
	mtx[1][1] = yAxis[1];
	mtx[2][1] = yAxis[2];
	mtx[3][1] = 0.0f;


	mtx[0][2] = zAxis[0];
	mtx[1][2] = zAxis[1];
	mtx[2][2] = zAxis[2];
	mtx[3][2] = 0.0f;

	mtx[0][3] = eye[0];
	mtx[1][3] = eye[1];
	mtx[2][3] = eye[2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::perspective(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI*fovx / 360);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = yScale;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = (zfar + znear) / (znear - zfar);
	mtx[3][2] = -1.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = (2.0f * zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;
}

void Matrix4f::perspectiveD3D(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI*fovx / 360);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = yScale;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = zfar / (znear - zfar);
	mtx[3][2] = -1.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = (zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;

}

void Matrix4f::linearPerspectiveD3D(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI*fovx / 360);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	//float Q = zfar / (zfar - znear);
	//float near = ((zfar * znear) / (znear - zfar)) / (zfar / (znear - zfar));
	//float far = -N * Q / (1 - Q);

	mtx[0][0] = xScale;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = yScale;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1 / (znear - zfar);
	mtx[3][2] = -1.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = znear / (znear - zfar);
	mtx[3][3] = 0.0f;

}

void Matrix4f::invPerspective(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI*fovx / 360);

	mtx[0][0] = e * aspect;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = e;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 0.0;
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = (znear - zfar) / (2 * zfar * znear);
	mtx[3][3] = (znear + zfar) / (2 * zfar * znear);
}

void Matrix4f::orthographic(float left, float right, float bottom, float top, float znear, float zfar) {

	mtx[0][0] = 2 / (right - left);
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 2 / (top - bottom);
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 2 / (znear - zfar);
	mtx[3][2] = 0.0f;

	mtx[0][3] = (right + left) / (left - right);
	mtx[1][3] = (top + bottom) / (bottom - top);
	mtx[2][3] = (zfar + znear) / (znear - zfar);
	mtx[3][3] = 1.0f;
}

void Matrix4f::invOrthographic(float left, float right, float bottom, float top, float znear, float zfar) {

	mtx[0][0] = (right - left) * 0.5;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = (top - bottom) * 0.5;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = (znear - zfar) * 0.5;
	mtx[2][3] = -(zfar + znear) * 0.5;

	mtx[0][3] = (right + left) * 0.5;
	mtx[1][3] = (top + bottom) * 0.5;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

Matrix4f::Matrix4f(float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44) {

	mtx[0][0] = m11, mtx[0][1] = m12, mtx[0][2] = m13, mtx[0][3] = m14;
	mtx[1][0] = m21, mtx[1][1] = m22, mtx[1][2] = m23, mtx[1][3] = m24;
	mtx[2][0] = m31, mtx[2][1] = m32, mtx[2][2] = m33, mtx[2][3] = m34;
	mtx[3][0] = m41, mtx[3][1] = m42, mtx[3][2] = m43, mtx[3][3] = m44;
}

void Matrix4f::fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees) {
	// Constructs a rotation matrix based on a Euler Transform.
	// I use the popular NASA standard airplane convention of 
	// heading-pitch-roll (i.e., RzRxRy).

	headDegrees = (headDegrees * PI) / 180.0f;
	pitchDegrees = (pitchDegrees * PI) / 180.0f;
	rollDegrees = (rollDegrees * PI) / 180.0f;

	float cosH = cosf(headDegrees);
	float cosP = cosf(pitchDegrees);
	float cosR = cosf(rollDegrees);
	float sinH = sinf(headDegrees);
	float sinP = sinf(pitchDegrees);
	float sinR = sinf(rollDegrees);

	mtx[0][0] = cosR * cosH - sinR * sinP * sinH;
	mtx[0][1] = sinR * cosH + cosR * sinP * sinH;
	mtx[0][2] = -cosP * sinH;
	mtx[0][3] = 0.0f;

	mtx[1][0] = -sinR * cosP;
	mtx[1][1] = cosR * cosP;
	mtx[1][2] = sinP;
	mtx[1][3] = 0.0f;

	mtx[2][0] = cosR * sinH + sinR * sinP * cosH;
	mtx[2][1] = sinR * sinH - cosR * sinP * cosH;
	mtx[2][2] = cosP * cosH;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const {
	// Extracts the Euler angles from a rotation matrix. The returned
	// angles are in degrees. This method might suffer from numerical
	// imprecision for ill defined rotation matrices.
	//
	// This function only works for rotation matrices constructed using
	// the popular NASA standard airplane convention of heading-pitch-roll 
	// (i.e., RzRxRy).
	//
	// The algorithm used is from:
	//  David Eberly, "Euler Angle Formulas", Geometric Tools web site,
	//  http://www.geometrictools.com/Documentation/EulerAngles.pdf.

	float thetaX = asinf(mtx[1][2]);
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

	if (thetaX < HALF_PI)
	{
		if (thetaX > -HALF_PI) {

			thetaZ = atan2f(-mtx[1][0], mtx[1][1]);
			thetaY = atan2f(-mtx[0][2], mtx[2][2]);

		}
		else {

			// Not a unique solution.
			thetaZ = -atan2f(mtx[2][0], mtx[0][0]);
			thetaY = 0.0f;
		}

	}
	else {

		// Not a unique solution.
		thetaZ = atan2f(mtx[2][0], mtx[0][0]);
		thetaY = 0.0f;
	}

	headDegrees = (thetaY * 180.0f) / PI;
	pitchDegrees = (thetaX * 180.0f) / PI;
	rollDegrees = (thetaZ * 180.0f) / PI;
}

Matrix4f &Matrix4f::Translate(const float dx, const float dy, const float dz) {
	return Matrix4f(1.0f, 0.0f, 0.0f, dx,
		0.0f, 1.0f, 0.0f, dy,
		0.0f, 0.0f, 1.0f, dz,
		0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f &Matrix4f::Translate(Matrix4f &mtx, const float dx, const float dy, const float dz) {
	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = 0.0f;

	mtx[0][3] = dx;
	mtx[1][3] = dy;
	mtx[2][3] = dz;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f &Matrix4f::Scale(float x, float y, float z) {

	//if (x == 0) x = 1.0;
	//if (y == 0) y = 1.0;
	//if (z == 0) z = 1.0;
	return Matrix4f(x, 0.0f, 0.0f, 0.0f,
					0.0f, y, 0.0f, 0.0f,
					0.0f, 0.0f, z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f &Matrix4f::Scale(Matrix4f &mtx, float x, float y, float z) {
	mtx[0][0] = x;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = y;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = z;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f &Matrix4f::GetNormalMatrix(const Matrix4f &modelViewMatrix) {

	Matrix4f normalMatrix;
	float det;
	float invDet;

	det = modelViewMatrix[0][0] * (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) +
		modelViewMatrix[0][1] * (modelViewMatrix[1][2] * modelViewMatrix[2][0] - modelViewMatrix[2][2] * modelViewMatrix[1][0]) +
		modelViewMatrix[0][2] * (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]);

	invDet = 1.0 / det;

	normalMatrix[0][0] = (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) * invDet;
	normalMatrix[1][0] = (modelViewMatrix[2][1] * modelViewMatrix[0][2] - modelViewMatrix[2][2] * modelViewMatrix[0][1]) * invDet;
	normalMatrix[2][0] = (modelViewMatrix[0][1] * modelViewMatrix[1][2] - modelViewMatrix[1][1] * modelViewMatrix[0][2]) * invDet;
	normalMatrix[3][0] = 0.0;

	normalMatrix[0][1] = (modelViewMatrix[2][0] * modelViewMatrix[1][2] - modelViewMatrix[1][0] * modelViewMatrix[2][2]) * invDet;
	normalMatrix[1][1] = (modelViewMatrix[0][0] * modelViewMatrix[2][2] - modelViewMatrix[2][0] * modelViewMatrix[0][2]) * invDet;
	normalMatrix[2][1] = (modelViewMatrix[1][0] * modelViewMatrix[0][2] - modelViewMatrix[1][2] * modelViewMatrix[0][0]) * invDet;
	normalMatrix[3][1] = 0.0;

	normalMatrix[0][2] = (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]) * invDet;
	normalMatrix[1][2] = (modelViewMatrix[2][0] * modelViewMatrix[0][1] - modelViewMatrix[0][0] * modelViewMatrix[2][1]) * invDet;
	normalMatrix[2][2] = (modelViewMatrix[0][0] * modelViewMatrix[1][1] - modelViewMatrix[0][1] * modelViewMatrix[1][0]) * invDet;
	normalMatrix[3][2] = 0.0;

	normalMatrix[0][3] = 0.0;
	normalMatrix[1][3] = 0.0;
	normalMatrix[2][3] = 0.0;
	normalMatrix[3][3] = 1.0;


	return normalMatrix;
}

Matrix4f &Matrix4f::GetNormalMatrix(Matrix4f &mtx, const Matrix4f &modelViewMatrix) {
	float det;
	float invDet;

	det = modelViewMatrix[0][0] * (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) +
		modelViewMatrix[0][1] * (modelViewMatrix[1][2] * modelViewMatrix[2][0] - modelViewMatrix[2][2] * modelViewMatrix[1][0]) +
		modelViewMatrix[0][2] * (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]);

	invDet = 1.0 / det;

	mtx[0][0] = (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) * invDet;
	mtx[1][0] = (modelViewMatrix[2][1] * modelViewMatrix[0][2] - modelViewMatrix[2][2] * modelViewMatrix[0][1]) * invDet;
	mtx[2][0] = (modelViewMatrix[0][1] * modelViewMatrix[1][2] - modelViewMatrix[1][1] * modelViewMatrix[0][2]) * invDet;
	mtx[3][0] = 0.0;

	mtx[0][1] = (modelViewMatrix[2][0] * modelViewMatrix[1][2] - modelViewMatrix[1][0] * modelViewMatrix[2][2]) * invDet;
	mtx[1][1] = (modelViewMatrix[0][0] * modelViewMatrix[2][2] - modelViewMatrix[2][0] * modelViewMatrix[0][2]) * invDet;
	mtx[2][1] = (modelViewMatrix[1][0] * modelViewMatrix[0][2] - modelViewMatrix[1][2] * modelViewMatrix[0][0]) * invDet;
	mtx[3][1] = 0.0;

	mtx[0][2] = (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]) * invDet;
	mtx[1][2] = (modelViewMatrix[2][0] * modelViewMatrix[0][1] - modelViewMatrix[0][0] * modelViewMatrix[2][1]) * invDet;
	mtx[2][2] = (modelViewMatrix[0][0] * modelViewMatrix[1][1] - modelViewMatrix[0][1] * modelViewMatrix[1][0]) * invDet;
	mtx[3][2] = 0.0;

	mtx[0][3] = 0.0;
	mtx[1][3] = 0.0;
	mtx[2][3] = 0.0;
	mtx[3][3] = 1.0;


	return mtx;
}

Matrix4f &Matrix4f::GetPerspective(float fovx, float aspect, float znear, float zfar) {
	Matrix4f perspective;

	float e = tanf(PI*fovx / 360);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	perspective[0][0] = xScale;
	perspective[1][0] = 0.0f;
	perspective[2][0] = 0.0f;
	perspective[3][0] = 0.0f;

	perspective[0][1] = 0.0f;
	perspective[1][1] = yScale;
	perspective[2][1] = 0.0f;
	perspective[3][1] = 0.0f;

	perspective[0][2] = 0.0f;
	perspective[1][2] = 0.0f;
	perspective[2][2] = (zfar + znear) / (znear - zfar);
	perspective[3][2] = -1.0f;

	perspective[0][3] = 0.0f;
	perspective[1][3] = 0.0f;
	perspective[2][3] = (2.0f * zfar * znear) / (znear - zfar);
	perspective[3][3] = 0.0f;

	return perspective;
}

Matrix4f &Matrix4f::GetPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar) {
	float e = tanf(PI*fovx / 360);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = yScale;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = (zfar + znear) / (znear - zfar);
	mtx[3][2] = -1.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = (2.0f * zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;

	return mtx;
}

Matrix4f &Matrix4f::GetInvPerspective(float fovx, float aspect, float znear, float zfar) {
	Matrix4f invPerspective;

	float e = tanf(PI*fovx / 360);

	invPerspective[0][0] = e * aspect;
	invPerspective[0][1] = 0.0f;
	invPerspective[0][2] = 0.0f;
	invPerspective[0][3] = 0.0f;

	invPerspective[1][0] = 0.0f;
	invPerspective[1][1] = e;
	invPerspective[1][2] = 0.0f;
	invPerspective[1][3] = 0.0f;

	invPerspective[2][0] = 0.0f;
	invPerspective[2][1] = 0.0f;
	invPerspective[2][2] = 0.0;
	invPerspective[2][3] = -1.0f;

	invPerspective[3][0] = 0.0f;
	invPerspective[3][1] = 0.0f;
	invPerspective[3][2] = (znear - zfar) / (2 * zfar * znear);
	invPerspective[3][3] = (znear + zfar) / (2 * zfar * znear);

	return invPerspective;
}

Matrix4f &Matrix4f::GetInvPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar) {
	float e = tanf(PI*fovx / 360);

	mtx[0][0] = e * aspect;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = e;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 0.0;
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = (znear - zfar) / (2 * zfar * znear);
	mtx[3][3] = (znear + zfar) / (2 * zfar * znear);

	return mtx;
}

Matrix4f &Matrix4f::GetOrthographic(float left, float right, float bottom, float top, float znear, float zfar) {
	Matrix4f ortho;

	ortho[0][0] = 2 / (right - left);
	ortho[1][0] = 0.0f;
	ortho[2][0] = 0.0f;
	ortho[3][0] = 0.0f;

	ortho[0][1] = 0.0f;
	ortho[1][1] = 2 / (top - bottom);
	ortho[2][1] = 0.0f;
	ortho[3][1] = 0.0f;

	ortho[0][2] = 0.0f;
	ortho[1][2] = 0.0f;
	ortho[2][2] = 2 / (znear - zfar);
	ortho[3][2] = 0.0f;

	ortho[0][3] = (right + left) / (left - right);
	ortho[1][3] = (top + bottom) / (bottom - top);
	ortho[2][3] = (zfar + znear) / (znear - zfar);
	ortho[3][3] = 1.0f;

	return ortho;
}

Matrix4f &Matrix4f::GetOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar) {
	mtx[0][0] = 2 / (right - left);
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 2 / (top - bottom);
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 2 / (znear - zfar);
	mtx[3][2] = 0.0f;

	mtx[0][3] = (right + left) / (left - right);
	mtx[1][3] = (top + bottom) / (bottom - top);
	mtx[2][3] = (zfar + znear) / (znear - zfar);
	mtx[3][3] = 1.0f;

	return mtx;
}


Matrix4f &Matrix4f::GetInvOrthographic(float left, float right, float bottom, float top, float znear, float zfar) {
	Matrix4f invOrtho;

	invOrtho[0][0] = (right - left) * 0.5;
	invOrtho[1][0] = 0.0f;
	invOrtho[2][0] = 0.0f;
	invOrtho[3][0] = 0.0f;

	invOrtho[0][1] = 0.0f;
	invOrtho[1][1] = (top - bottom) * 0.5;
	invOrtho[2][1] = 0.0f;
	invOrtho[3][1] = 0.0f;

	invOrtho[0][2] = 0.0f;
	invOrtho[1][2] = 0.0f;
	invOrtho[2][2] = (znear - zfar) * 0.5;
	invOrtho[2][3] = -(zfar + znear) * 0.5;

	invOrtho[0][3] = (right + left) * 0.5;
	invOrtho[1][3] = (top + bottom) * 0.5;
	invOrtho[3][2] = 0.0f;
	invOrtho[3][3] = 1.0f;

	return invOrtho;
}

Matrix4f &Matrix4f::GetInvOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar) {
	mtx[0][0] = (right - left) * 0.5;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = (top - bottom) * 0.5;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = (znear - zfar) * 0.5;
	mtx[2][3] = -(zfar + znear) * 0.5;

	mtx[0][3] = (right + left) * 0.5;
	mtx[1][3] = (top + bottom) * 0.5;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

float *Matrix4f::operator[](int row) {

	return mtx[row];
}

const float *Matrix4f::operator[](int row) const {

	return mtx[row];
}


void Matrix4f::identity() {

	mtx[0][0] = 1.0f, mtx[0][1] = 0.0f, mtx[0][2] = 0.0f, mtx[0][3] = 0.0f;
	mtx[1][0] = 0.0f, mtx[1][1] = 1.0f, mtx[1][2] = 0.0f, mtx[1][3] = 0.0f;
	mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = 1.0f, mtx[2][3] = 0.0f;
	mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

Matrix4f &Matrix4f::operator+=(const Matrix4f &rhs) {

	Matrix4f tmp;

	// Row 1.
	tmp.mtx[0][0] = mtx[0][0] + rhs.mtx[0][0];
	tmp.mtx[0][1] = mtx[0][1] + rhs.mtx[0][1];
	tmp.mtx[0][2] = mtx[0][2] + rhs.mtx[0][2];
	tmp.mtx[0][3] = mtx[0][3] + rhs.mtx[0][3];

	// Row 2.
	tmp.mtx[1][0] = mtx[1][0] + rhs.mtx[1][0];
	tmp.mtx[1][1] = mtx[1][1] + rhs.mtx[1][1];
	tmp.mtx[1][2] = mtx[1][2] + rhs.mtx[1][2];
	tmp.mtx[1][3] = mtx[1][3] + rhs.mtx[1][3];

	// Row 3.
	tmp.mtx[2][0] = mtx[2][0] + rhs.mtx[2][0];
	tmp.mtx[2][1] = mtx[2][1] + rhs.mtx[2][1];
	tmp.mtx[2][2] = mtx[2][2] + rhs.mtx[2][2];
	tmp.mtx[2][3] = mtx[2][3] + rhs.mtx[2][3];

	// Row 4.
	tmp.mtx[3][0] = mtx[3][0] + rhs.mtx[3][0];
	tmp.mtx[3][1] = mtx[3][1] + rhs.mtx[3][1];
	tmp.mtx[3][2] = mtx[3][2] + rhs.mtx[3][2];
	tmp.mtx[3][3] = mtx[3][3] + rhs.mtx[3][3];

	*this = tmp;
	return *this;
}

Matrix4f &Matrix4f::operator*=(const Matrix4f &rhs) {

	Matrix4f tmp;

	// Row 1.
	tmp.mtx[0][0] = (rhs.mtx[0][0] * mtx[0][0]) + (rhs.mtx[0][1] * mtx[1][0]) + (rhs.mtx[0][2] * mtx[2][0]) + (rhs.mtx[0][3] * mtx[3][0]);
	tmp.mtx[0][1] = (rhs.mtx[0][0] * mtx[0][1]) + (rhs.mtx[0][1] * mtx[1][1]) + (rhs.mtx[0][2] * mtx[2][1]) + (rhs.mtx[0][3] * mtx[3][1]);
	tmp.mtx[0][2] = (rhs.mtx[0][0] * mtx[0][2]) + (rhs.mtx[0][1] * mtx[1][2]) + (rhs.mtx[0][2] * mtx[2][2]) + (rhs.mtx[0][3] * mtx[3][2]);
	tmp.mtx[0][3] = (rhs.mtx[0][0] * mtx[0][3]) + (rhs.mtx[0][1] * mtx[1][3]) + (rhs.mtx[0][2] * mtx[2][3]) + (rhs.mtx[0][3] * mtx[3][3]);

	// Row 2.
	tmp.mtx[1][0] = (rhs.mtx[1][0] * mtx[0][0]) + (rhs.mtx[1][1] * mtx[1][0]) + (rhs.mtx[1][2] * mtx[2][0]) + (rhs.mtx[1][3] * mtx[3][0]);
	tmp.mtx[1][1] = (rhs.mtx[1][0] * mtx[0][1]) + (rhs.mtx[1][1] * mtx[1][1]) + (rhs.mtx[1][2] * mtx[2][1]) + (rhs.mtx[1][3] * mtx[3][1]);
	tmp.mtx[1][2] = (rhs.mtx[1][0] * mtx[0][2]) + (rhs.mtx[1][1] * mtx[1][2]) + (rhs.mtx[1][2] * mtx[2][2]) + (rhs.mtx[1][3] * mtx[3][2]);
	tmp.mtx[1][3] = (rhs.mtx[1][0] * mtx[0][3]) + (rhs.mtx[1][1] * mtx[1][3]) + (rhs.mtx[1][2] * mtx[2][3]) + (rhs.mtx[1][3] * mtx[3][3]);

	// Row 3.
	tmp.mtx[2][0] = (rhs.mtx[2][0] * mtx[0][0]) + (rhs.mtx[2][1] * mtx[1][0]) + (rhs.mtx[2][2] * mtx[2][0]) + (rhs.mtx[2][3] * mtx[3][0]);
	tmp.mtx[2][1] = (rhs.mtx[2][0] * mtx[0][1]) + (rhs.mtx[2][1] * mtx[1][1]) + (rhs.mtx[2][2] * mtx[2][1]) + (rhs.mtx[2][3] * mtx[3][1]);
	tmp.mtx[2][2] = (rhs.mtx[2][0] * mtx[0][2]) + (rhs.mtx[2][1] * mtx[1][2]) + (rhs.mtx[2][2] * mtx[2][2]) + (rhs.mtx[2][3] * mtx[3][2]);
	tmp.mtx[2][3] = (rhs.mtx[2][0] * mtx[0][3]) + (rhs.mtx[2][1] * mtx[1][3]) + (rhs.mtx[2][2] * mtx[2][3]) + (rhs.mtx[2][3] * mtx[3][3]);

	// Row 4.
	tmp.mtx[3][0] = (rhs.mtx[3][0] * mtx[0][0]) + (rhs.mtx[3][1] * mtx[1][0]) + (rhs.mtx[3][2] * mtx[2][0]) + (rhs.mtx[3][3] * mtx[3][0]);
	tmp.mtx[3][1] = (rhs.mtx[3][0] * mtx[0][1]) + (rhs.mtx[3][1] * mtx[1][1]) + (rhs.mtx[3][2] * mtx[2][1]) + (rhs.mtx[3][3] * mtx[3][1]);
	tmp.mtx[3][2] = (rhs.mtx[3][0] * mtx[0][2]) + (rhs.mtx[3][1] * mtx[1][2]) + (rhs.mtx[3][2] * mtx[2][2]) + (rhs.mtx[3][3] * mtx[3][2]);
	tmp.mtx[3][3] = (rhs.mtx[3][0] * mtx[0][3]) + (rhs.mtx[3][1] * mtx[1][3]) + (rhs.mtx[3][2] * mtx[2][3]) + (rhs.mtx[3][3] * mtx[3][3]);

	*this = tmp;
	return *this;
}


Matrix4f &Matrix4f::operator^=(const Matrix4f &rhs) {

	Matrix4f tmp;

	// Row 1.
	tmp.mtx[0][0] = (mtx[0][0] * rhs.mtx[0][0]) + (mtx[0][1] * rhs.mtx[1][0]) + (mtx[0][2] * rhs.mtx[2][0]) + (mtx[0][3] * rhs.mtx[3][0]);
	tmp.mtx[0][1] = (mtx[0][0] * rhs.mtx[0][1]) + (mtx[0][1] * rhs.mtx[1][1]) + (mtx[0][2] * rhs.mtx[2][1]) + (mtx[0][3] * rhs.mtx[3][1]);
	tmp.mtx[0][2] = (mtx[0][0] * rhs.mtx[0][2]) + (mtx[0][1] * rhs.mtx[1][2]) + (mtx[0][2] * rhs.mtx[2][2]) + (mtx[0][3] * rhs.mtx[3][2]);
	tmp.mtx[0][3] = (mtx[0][0] * rhs.mtx[0][3]) + (mtx[0][1] * rhs.mtx[1][3]) + (mtx[0][2] * rhs.mtx[2][3]) + (mtx[0][3] * rhs.mtx[3][3]);

	// Row 2.
	tmp.mtx[1][0] = (mtx[1][0] * rhs.mtx[0][0]) + (mtx[1][1] * rhs.mtx[1][0]) + (mtx[1][2] * rhs.mtx[2][0]) + (mtx[1][3] * rhs.mtx[3][0]);
	tmp.mtx[1][1] = (mtx[1][0] * rhs.mtx[0][1]) + (mtx[1][1] * rhs.mtx[1][1]) + (mtx[1][2] * rhs.mtx[2][1]) + (mtx[1][3] * rhs.mtx[3][1]);
	tmp.mtx[1][2] = (mtx[1][0] * rhs.mtx[0][2]) + (mtx[1][1] * rhs.mtx[1][2]) + (mtx[1][2] * rhs.mtx[2][2]) + (mtx[1][3] * rhs.mtx[3][2]);
	tmp.mtx[1][3] = (mtx[1][0] * rhs.mtx[0][3]) + (mtx[1][1] * rhs.mtx[1][3]) + (mtx[1][2] * rhs.mtx[2][3]) + (mtx[1][3] * rhs.mtx[3][3]);

	// Row 3.
	tmp.mtx[2][0] = (mtx[2][0] * rhs.mtx[0][0]) + (mtx[2][1] * rhs.mtx[1][0]) + (mtx[2][2] * rhs.mtx[2][0]) + (mtx[2][3] * rhs.mtx[3][0]);
	tmp.mtx[2][1] = (mtx[2][0] * rhs.mtx[0][1]) + (mtx[2][1] * rhs.mtx[1][1]) + (mtx[2][2] * rhs.mtx[2][1]) + (mtx[2][3] * rhs.mtx[3][1]);
	tmp.mtx[2][2] = (mtx[2][0] * rhs.mtx[0][2]) + (mtx[2][1] * rhs.mtx[1][2]) + (mtx[2][2] * rhs.mtx[2][2]) + (mtx[2][3] * rhs.mtx[3][2]);
	tmp.mtx[2][3] = (mtx[2][0] * rhs.mtx[0][3]) + (mtx[2][1] * rhs.mtx[1][3]) + (mtx[2][2] * rhs.mtx[2][3]) + (mtx[2][3] * rhs.mtx[3][3]);

	// Row 4.
	tmp.mtx[3][0] = (mtx[3][0] * rhs.mtx[0][0]) + (mtx[3][1] * rhs.mtx[1][0]) + (mtx[3][2] * rhs.mtx[2][0]) + (mtx[3][3] * rhs.mtx[3][0]);
	tmp.mtx[3][1] = (mtx[3][0] * rhs.mtx[0][1]) + (mtx[3][1] * rhs.mtx[1][1]) + (mtx[3][2] * rhs.mtx[2][1]) + (mtx[3][3] * rhs.mtx[3][1]);
	tmp.mtx[3][2] = (mtx[3][0] * rhs.mtx[0][2]) + (mtx[3][1] * rhs.mtx[1][2]) + (mtx[3][2] * rhs.mtx[2][2]) + (mtx[3][3] * rhs.mtx[3][2]);
	tmp.mtx[3][3] = (mtx[3][0] * rhs.mtx[0][3]) + (mtx[3][1] * rhs.mtx[1][3]) + (mtx[3][2] * rhs.mtx[2][3]) + (mtx[3][3] * rhs.mtx[3][3]);

	*this = tmp;
	return *this;
}

Matrix4f Matrix4f::operator+(const Matrix4f &rhs) const {

	Matrix4f tmp(*this);
	tmp += rhs;
	return tmp;
}

Matrix4f Matrix4f::operator*(const Matrix4f &rhs) const {

	Matrix4f tmp(*this);
	tmp *= rhs;
	return tmp;
}

Matrix4f Matrix4f::operator^(const Matrix4f &rhs) const {

	Matrix4f tmp(*this);
	tmp ^= rhs;

	return tmp;
}

Matrix4f Matrix4f::transpose() {

	return Matrix4f(mtx[0][0], mtx[1][0], mtx[2][0], mtx[3][0],
		mtx[0][1], mtx[1][1], mtx[2][1], mtx[3][1],
		mtx[0][2], mtx[1][2], mtx[2][2], mtx[3][2],
		mtx[0][3], mtx[1][3], mtx[2][3], mtx[3][3]);
}

void Matrix4f::Transpose(Matrix4f &m) {

	float tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[0][3]; m[0][3] = m[3][0]; m[3][0] = tmp;

	tmp = m[2][1]; m[2][1] = m[1][2]; m[1][2] = tmp;
	tmp = m[3][1]; m[3][1] = m[1][3]; m[1][3] = tmp;

	tmp = m[2][3]; m[2][3] = m[3][2]; m[3][2] = tmp;
}

float Matrix4f::determinant() const {

	return (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1])
		* (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3])
		- (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1])
		* (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3])
		+ (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1])
		* (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3])
		+ (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1])
		* (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3])
		- (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1])
		* (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3])
		+ (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1])
		* (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]);
}

Matrix4f Matrix4f::inverse() const{
	// This method of computing the inverse of a 4x4 matrix is based
	// on a similar function found in Paul Nettle's matrix template
	// class (http://www.fluidstudios.com).
	//
	// If the inverse doesn't exist for this matrix, then the identity
	// matrix will be returned.

	Matrix4f tmp;
	float d = determinant();

	if (fabsf(d) < 0.0001f) {
		tmp.identity();

	}
	else {
		d = 1.0f / d;

		tmp.mtx[0][0] = d * (mtx[1][1] * (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3]) + mtx[2][1] * (mtx[3][2] * mtx[1][3] - mtx[1][2] * mtx[3][3]) + mtx[3][1] * (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3]));
		tmp.mtx[1][0] = d * (mtx[1][2] * (mtx[2][0] * mtx[3][3] - mtx[3][0] * mtx[2][3]) + mtx[2][2] * (mtx[3][0] * mtx[1][3] - mtx[1][0] * mtx[3][3]) + mtx[3][2] * (mtx[1][0] * mtx[2][3] - mtx[2][0] * mtx[1][3]));
		tmp.mtx[2][0] = d * (mtx[1][3] * (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1]) + mtx[2][3] * (mtx[3][0] * mtx[1][1] - mtx[1][0] * mtx[3][1]) + mtx[3][3] * (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]));
		tmp.mtx[3][0] = d * (mtx[1][0] * (mtx[3][1] * mtx[2][2] - mtx[2][1] * mtx[3][2]) + mtx[2][0] * (mtx[1][1] * mtx[3][2] - mtx[3][1] * mtx[1][2]) + mtx[3][0] * (mtx[2][1] * mtx[1][2] - mtx[1][1] * mtx[2][2]));

		tmp.mtx[0][1] = d * (mtx[2][1] * (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3]) + mtx[3][1] * (mtx[2][2] * mtx[0][3] - mtx[0][2] * mtx[2][3]) + mtx[0][1] * (mtx[3][2] * mtx[2][3] - mtx[2][2] * mtx[3][3]));
		tmp.mtx[1][1] = d * (mtx[2][2] * (mtx[0][0] * mtx[3][3] - mtx[3][0] * mtx[0][3]) + mtx[3][2] * (mtx[2][0] * mtx[0][3] - mtx[0][0] * mtx[2][3]) + mtx[0][2] * (mtx[3][0] * mtx[2][3] - mtx[2][0] * mtx[3][3]));
		tmp.mtx[2][1] = d * (mtx[2][3] * (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1]) + mtx[3][3] * (mtx[2][0] * mtx[0][1] - mtx[0][0] * mtx[2][1]) + mtx[0][3] * (mtx[3][0] * mtx[2][1] - mtx[2][0] * mtx[3][1]));
		tmp.mtx[3][1] = d * (mtx[2][0] * (mtx[3][1] * mtx[0][2] - mtx[0][1] * mtx[3][2]) + mtx[3][0] * (mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) + mtx[0][0] * (mtx[2][1] * mtx[3][2] - mtx[3][1] * mtx[2][2]));

		tmp.mtx[0][2] = d * (mtx[3][1] * (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]) + mtx[0][1] * (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3]) + mtx[1][1] * (mtx[3][2] * mtx[0][3] - mtx[0][2] * mtx[3][3]));
		tmp.mtx[1][2] = d * (mtx[3][2] * (mtx[0][0] * mtx[1][3] - mtx[1][0] * mtx[0][3]) + mtx[0][2] * (mtx[1][0] * mtx[3][3] - mtx[3][0] * mtx[1][3]) + mtx[1][2] * (mtx[3][0] * mtx[0][3] - mtx[0][0] * mtx[3][3]));
		tmp.mtx[2][2] = d * (mtx[3][3] * (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) + mtx[0][3] * (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1]) + mtx[1][3] * (mtx[3][0] * mtx[0][1] - mtx[0][0] * mtx[3][1]));
		tmp.mtx[3][2] = d * (mtx[3][0] * (mtx[1][1] * mtx[0][2] - mtx[0][1] * mtx[1][2]) + mtx[0][0] * (mtx[3][1] * mtx[1][2] - mtx[1][1] * mtx[3][2]) + mtx[1][0] * (mtx[0][1] * mtx[3][2] - mtx[3][1] * mtx[0][2]));

		tmp.mtx[0][3] = d * (mtx[0][1] * (mtx[2][2] * mtx[1][3] - mtx[1][2] * mtx[2][3]) + mtx[1][1] * (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3]) + mtx[2][1] * (mtx[1][2] * mtx[0][3] - mtx[0][2] * mtx[1][3]));
		tmp.mtx[1][3] = d * (mtx[0][2] * (mtx[2][0] * mtx[1][3] - mtx[1][0] * mtx[2][3]) + mtx[1][2] * (mtx[0][0] * mtx[2][3] - mtx[2][0] * mtx[0][3]) + mtx[2][2] * (mtx[1][0] * mtx[0][3] - mtx[0][0] * mtx[1][3]));
		tmp.mtx[2][3] = d * (mtx[0][3] * (mtx[2][0] * mtx[1][1] - mtx[1][0] * mtx[2][1]) + mtx[1][3] * (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) + mtx[2][3] * (mtx[1][0] * mtx[0][1] - mtx[0][0] * mtx[1][1]));
		tmp.mtx[3][3] = d * (mtx[0][0] * (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) + mtx[1][0] * (mtx[2][1] * mtx[0][2] - mtx[0][1] * mtx[2][2]) + mtx[2][0] * (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]));
	}

	return tmp;
}

//friend operator
Matrix4f operator*(float scalar, const Matrix4f &rhs) {

	Matrix4f tmp(rhs);

	tmp[0][0] *= scalar, tmp[0][1] *= scalar, tmp[0][2] *= scalar, tmp[0][3] *= scalar;
	tmp[1][0] *= scalar, tmp[1][1] *= scalar, tmp[1][2] *= scalar, tmp[1][3] *= scalar;
	tmp[2][0] *= scalar, tmp[2][1] *= scalar, tmp[2][2] *= scalar, tmp[2][3] *= scalar;
	tmp[3][0] *= scalar, tmp[3][1] *= scalar, tmp[3][2] *= scalar, tmp[3][3] *= scalar;
	return tmp;
}

//friend operator
Vector3f operator*(const Vector4f &lhs, const Matrix4f &rhs) {

	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + (lhs[3] * rhs.mtx[3][0]),
		(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + (lhs[3] * rhs.mtx[3][1]),
		(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[3][2]));
}

//friend operator
Vector3f operator*(const Matrix4f &rhs, const Vector4f &lhs) {

	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]) + (lhs[3] * rhs.mtx[0][3]),
					(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]) + (lhs[3] * rhs.mtx[1][3]),
					(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[2][3]));
}

/*//friend operator
Vector3f operator&(const Matrix4f &rhs, const Vector4f &lhs) {

	float norm = rhs[0][3] * lhs[0] + rhs[1][3] * lhs[1] + rhs[2][3] * lhs[2] + rhs[3][3] * lhs[3];

	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]) + (lhs[3] * rhs.mtx[0][3]),
		(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]) + (lhs[3] * rhs.mtx[1][3]),
		(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[2][3]));
}*/

//friend operator
Vector3f operator*(const Matrix4f &rhs, const Vector3f &lhs) {

	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]),
		(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]),
		(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]));
}

//friend operator
Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs) {

	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]),
		(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]),
		(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]));
}

//friend operator
Vector4f operator^(const Matrix4f &rhs, const Vector4f &lhs) {
	return Vector4f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]) + (lhs[3] * rhs.mtx[0][3]),
		(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]) + (lhs[3] * rhs.mtx[1][3]),
		(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[2][3]),
		(lhs[0] * rhs.mtx[3][0]) + (lhs[1] * rhs.mtx[3][1]) + (lhs[2] * rhs.mtx[3][2]) + (lhs[3] * rhs.mtx[3][3])
	);
}

Vector4f operator^(const Vector4f &lhs, const Matrix4f &rhs) {
	return Vector4f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + (lhs[3] * rhs.mtx[3][0]),
		(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + (lhs[3] * rhs.mtx[3][1]),
		(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[3][2]),
		(lhs[0] * rhs.mtx[0][3]) + (lhs[1] * rhs.mtx[1][3]) + (lhs[2] * rhs.mtx[2][3]) + (lhs[3] * rhs.mtx[3][3])
	);
}

void Matrix4f::print() {
	std::cout << mtx[0][0] << "  " << mtx[0][1] << "  " << mtx[0][2] << "  " << mtx[0][3] << std::endl;
	std::cout << mtx[1][0] << "  " << mtx[1][1] << "  " << mtx[1][2] << "  " << mtx[1][3] << std::endl;
	std::cout << mtx[2][0] << "  " << mtx[2][1] << "  " << mtx[2][2] << "  " << mtx[2][3] << std::endl;
	std::cout << mtx[3][0] << "  " << mtx[3][1] << "  " << mtx[3][2] << "  " << mtx[3][3] << std::endl;
	std::cout << "-------------------" << std::endl;
}

//////////////////////////////////////////////////////////////////////
Vector2f::Vector2f() {

	vec[0] = 0.0f;
	vec[1] = 0.0f;
}
Vector2f::~Vector2f() {}

Vector2f::Vector2f(float x_, float y_){
	vec[0] = x_;
	vec[1] = y_;

}

Vector2f::Vector2f(const Vector2f &rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
}

float &Vector2f::operator[](int index) {
	return vec[index];
}

const float Vector2f::operator[](int index) const {
	return vec[index];
}

const float* Vector2f::getVec()const {

	return vec;
}

Vector2f &Vector2f::operator=(const Vector2f &rhs) {
	vec[0] = rhs.vec[0], vec[1] = rhs.vec[1];
	return *this;
}

Vector2f &Vector2f::operator-=(const Vector2f &rhs) {
	vec[0] -= rhs.vec[0], vec[1] -= rhs.vec[1];
	return *this;
}

Vector2f &Vector2f::operator+=(const Vector2f &rhs) {
	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1];
	return *this;
}


Vector2f Vector2f::operator+(const Vector2f &rhs) const {
	Vector2f tmp(*this);
	tmp += rhs;
	return tmp;
}

Vector2f Vector2f::operator-(const Vector2f &rhs) const {
	Vector2f tmp(*this);
	tmp -= rhs;
	return tmp;
}

Vector2f Vector2f::operator*(float scalar) const {
	return Vector2f(vec[0] * scalar, vec[1] * scalar);
}

Vector2f Vector2f::operator/(float scalar) const {
	return Vector2f(vec[0] / scalar, vec[1] / scalar);
}

Vector2f operator-(const Vector2f &v) {
	return Vector2f(-v.vec[0], -v.vec[1]);
}

//friend operator
bool operator ==(Vector2f lhs, Vector2f rhs) {
	float epsilon = 0.000001;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon;
}
//////////////////////////////////////////////////////////////////////
Vector3f::Vector3f() {
	vec[0] = 0.0f;
	vec[1] = 0.0f;
	vec[2] = 0.0f;
}

Vector3f::~Vector3f() {}

Vector3f::Vector3f(float x_, float y_, float z_) {

	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;
}

float &Vector3f::operator[](int index) {
	return vec[index];
}

const float Vector3f::operator[](int index) const {
	return vec[index];
}

Vector3f &Vector3f::operator-=(const Vector3f &rhs) {

	vec[0] -= rhs.vec[0], vec[1] -= rhs.vec[1], vec[2] -= rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator+=(const Vector3f &rhs) {

	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1], vec[2] += rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator+(const Vector3f &rhs) const {
	Vector3f tmp(*this);
	tmp += rhs;
	return tmp;
}

Vector3f Vector3f::operator-(const Vector3f &rhs) const {

	Vector3f tmp(*this);
	tmp -= rhs;
	return tmp;
}

Vector3f Vector3f::operator*(float scalar) const {

	return Vector3f(vec[0] * scalar, vec[1] * scalar, vec[2] * scalar);
}

Vector3f Vector3f::operator/(float scalar) const {

	return Vector3f(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar);
}

bool Vector3f::zero() {

	return vec[0] == 0.0 && vec[1] == 0.0 && vec[2] == 0.0;
}

Vector3f Vector3f::Cross(const Vector3f &p, const Vector3f &q) {

	return Vector3f((p.vec[1] * q.vec[2]) - (p.vec[2] * q.vec[1]),
		(p.vec[2] * q.vec[0]) - (p.vec[0] * q.vec[2]),
		(p.vec[0] * q.vec[1]) - (p.vec[1] * q.vec[0]));
}

float Vector3f::Dot(const Vector3f &p, const Vector3f &q) {

	return (p.vec[0] * q.vec[0]) + (p.vec[1] * q.vec[1]) + (p.vec[2] * q.vec[2]);
}

float Vector3f::length() const {

	return sqrtf((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
}

float Vector3f::lengthSq() const {
	return (vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]);
}

void Vector3f::Normalize(Vector3f &p) {

	float invMag = p.length()!= 0.0f ? 1.0f / p.length() : 1.0f;
	p.vec[0] *= invMag, p.vec[1] *= invMag, p.vec[2] *= invMag;
}

Vector3f Vector3f::Min(const Vector3f &p, const Vector3f &q) {

	return Vector3f(std::min(p[0], q[0]), std::min(p[1], q[1]), std::min(p[2], q[2]));
}

Vector3f Vector3f::Max(const Vector3f &p, const Vector3f &q) {

	return Vector3f(std::max(p[0], q[0]), std::max(p[1], q[1]), std::max(p[2], q[2]));
}

Vector3f Vector3f::normalize() {

	float invMag = 1.0f / length();
	return Vector3f(vec[0] * invMag, vec[1] * invMag, vec[2] * invMag);
}


void Vector3f::set(float x_, float y_, float z_) {
	vec[0] = x_, vec[1] = y_, vec[2] = z_;
}

const float* Vector3f::getVec()const {

	return vec;
}

//friend operator
Vector3f operator-(const Vector3f &v) {
	return Vector3f(-v.vec[0], -v.vec[1], -v.vec[2]);
}

//friend operator
bool operator ==(Vector3f lhs, Vector3f rhs) {
	float epsilon = 0.000001;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon && fabs(lhs[2] - rhs[2]) <= epsilon;
}

//friend operator
Vector3f operator*(float lhs, const Vector3f &rhs) {
	return Vector3f(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}
////////////////////////////////////////////////////////////////////////////////////////

Vector4f::Vector4f() {
	vec[0] = 0.0f;
	vec[1] = 0.0f;
	vec[2] = 0.0f;
	vec[3] = 0.0f;
}
Vector4f::~Vector4f() {}

Vector4f::Vector4f(float x_, float y_, float z_, float w_){
	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;
	vec[3] = w_;
}

Vector4f::Vector4f(const Vector3f &rhs) {
	vec[0] = rhs[0];
	vec[1] = rhs[1];
	vec[2] = rhs[2];
	vec[3] = 1.0;
}

float &Vector4f::operator[](int index) {

	return vec[index];
}

const float Vector4f::operator[](int index) const {
	return vec[index];
}

Vector4f &Vector4f::operator+=(const Vector4f &rhs) {
	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1], vec[2] += rhs.vec[2], vec[3] += rhs.vec[3];
	return *this;
}

Vector4f &Vector4f::operator+(const Vector4f &rhs) const {
	Vector4f tmp(*this);
	tmp += rhs;
	return tmp;
}
//friend operator
Vector4f operator*(float lhs, const Vector4f &rhs) {
	return Vector4f(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
}

Vector4f operator*(const Vector4f &lhs, float rhs) {
	return Vector4f(lhs[0] * rhs,  lhs[1] * rhs, lhs[2] * rhs,  lhs[3] * rhs);
}
//////////////////////////////////////////////////////////////////////
const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion() {
	quat[0] = 0.0f;
	quat[1] = 0.0f;
	quat[2] = 0.0f;
	quat[3] = 0.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	quat[0] = x;
	quat[1] = y;
	quat[2] = z;
	quat[3] = w;
}

Quaternion::Quaternion(float headDegrees, float pitchDegrees, float rollDegrees) {
	fromHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
}

Quaternion::Quaternion(const Vector3f &axis, float degrees) {
	fromAxisAngle(axis, degrees);
}

Quaternion::Quaternion(const Matrix4f &m) {
	fromMatrix(m);
}

float &Quaternion::operator[](int index) {
	return quat[index];
}

const float Quaternion::operator[](int index) const {
	return quat[index];
}

bool Quaternion::operator==(const Quaternion &rhs) const {
	float epsilon = 0.000001;
	return fabs(quat[0] - rhs[0]) <= epsilon && fabs(quat[1] - rhs[1]) <= epsilon && fabs(quat[2] - rhs[2]) <= epsilon && fabs(quat[3] - rhs[3]) <= epsilon;
}

bool Quaternion::operator!=(const Quaternion &rhs) const {
	return !Quaternion::operator==(rhs);
}

Quaternion &Quaternion::operator+=(const Quaternion &rhs) {
	quat[3] += rhs[3], quat[0] += rhs[0], quat[1] += rhs[1], quat[2] += rhs[2];
	return *this;
}

Quaternion &Quaternion::operator-=(const Quaternion &rhs) {
	quat[3] -= rhs[3], quat[0] -= rhs[0], quat[1] -= rhs[1], quat[2] -= rhs[2];
	return *this;
}

Quaternion &Quaternion::operator*=(const Quaternion &rhs) {

	// Multiply so that rotations are applied in a left to right order.
	Quaternion tmp(		
		(quat[3] * rhs[0]) + (quat[0] * rhs[3]) - (quat[1] * rhs[2]) + (quat[2] * rhs[1]),
		(quat[3] * rhs[1]) + (quat[0] * rhs[2]) + (quat[1] * rhs[3]) - (quat[2] * rhs[0]),
		(quat[3] * rhs[2]) - (quat[0] * rhs[1]) + (quat[1] * rhs[0]) + (quat[2] * rhs[3]),
		(quat[3] * rhs[3]) - (quat[0] * rhs[0]) - (quat[1] * rhs[1]) - (quat[2] * rhs[2]));


	// Multiply so that rotations are applied in a right to left order.
	/*Quaternion tmp(
	(quat[3] * rhs[0]) + (quat[0] * rhs[3]) + (quat[1] * rhs[2]) - (quat[2] * rhs[1]),
	(quat[3] * rhs[1]) - (quat[0] * rhs[2]) + (quat[1] * rhs[3]) + (quat[2] * rhs[0]),
	(quat[3] * rhs[2]) + (quat[0] * rhs[1]) - (quat[1] * rhs[0]) + (quat[2] * rhs[3]),
	(quat[3] * rhs[3]) - (quat[0] * rhs[0]) - (quat[1] * rhs[1]) - (quat[2] * rhs[2]));*/
	*this = tmp;
	return *this;
}

Quaternion &Quaternion::operator*=(float scalar) {
	quat[3] *= scalar, quat[0] *= scalar, quat[1] *= scalar, quat[2] *= scalar;
	return *this;
}

Quaternion &Quaternion::operator/=(float scalar) {
	quat[3] /= scalar, quat[0] /= scalar, quat[1] /= scalar, quat[2] /= scalar;
	return *this;
}

Quaternion Quaternion::operator+(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp += rhs;
	return tmp;
}

Quaternion Quaternion::operator-(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp -= rhs;
	return tmp;
}

Quaternion Quaternion::operator*(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp *= rhs;
	return tmp;
}

Quaternion Quaternion::operator*(float scalar) const {
	Quaternion tmp(*this);
	tmp *= scalar;
	return tmp;
}

Quaternion Quaternion::operator/(float scalar) const {
	Quaternion tmp(*this);
	tmp /= scalar;
	return tmp;
}

void Quaternion::identity() {
	quat[0] = quat[1] = quat[2] = 0.0f, quat[3] = 1.0f;
}

float Quaternion::length() const {
	return sqrtf(quat[3] * quat[3] + quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2]);
}

void Quaternion::normalize() {
	float invMag = 1.0f / length();
	quat[3] *= invMag, quat[0] *= invMag, quat[1] *= invMag, quat[2] *= invMag;
}

void Quaternion::set(float x, float y, float z, float w) {
	quat[0] = x, quat[1] = y, quat[2] = z, quat[3] = w;
}

Quaternion Quaternion::conjugate() const {
	Quaternion tmp(-quat[0], -quat[1], -quat[2], quat[3]);
	return tmp;
}

Quaternion Quaternion::inverse() const {
	float invMag = 1.0f / length();
	return conjugate() * invMag;
}

void Quaternion::fromAxisAngle(const Vector3f &axis, float degrees) {
	float halfTheta = (degrees * PI) / 180.0f * 0.5f;
	float s = sinf(halfTheta);
	quat[3] = cosf(halfTheta), quat[0] = axis[0] * s, quat[1] = axis[1] * s, quat[2] = axis[2] * s;
}

void Quaternion::fromMatrix(const Matrix4f &m) {
	// Creates a quaternion from a rotation matrix. 
	// The algorithm used is from Allan and Mark Watt's "Advanced 
	// Animation and Rendering Techniques" (ACM Press 1992).
	float s = 0.0f;
	float q[4] = { 0.0f };
	float trace = m[0][0] + m[1][1] + m[2][2];

	if (trace > 0.0f) {
		s = sqrtf(trace + 1.0f);
		q[3] = s * 0.5f;
		s = 0.5f / s;
		q[0] = (m[2][1] - m[1][2]) * s;
		q[1] = (m[0][2] - m[2][0]) * s;
		q[2] = (m[1][0] - m[0][1]) * s;
	}else {

		int nxt[3] = { 1, 2, 0 };
		int i = 0, j = 0, k = 0;

		if (m[1][1] > m[0][0])
			i = 1;

		if (m[2][2] > m[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];
		s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

		q[i] = s * 0.5f;
		s = 0.5f / s;
		q[3] = (m[k][j] - m[j][k]) * s;
		q[j] = (m[j][i] + m[i][j]) * s;
		q[k] = (m[k][i] + m[i][k]) * s;	
	}

	quat[0] = q[0], quat[1] = q[1], quat[2] = q[2], quat[3] = q[3];
}


void Quaternion::fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees) {
	Matrix4f m;
	m.fromHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
	fromMatrix(m);
}


void Quaternion::toAxisAngle(Vector3f &axis, float &degrees) const {
	// Converts this quaternion to an axis and an angle.
	float sinHalfThetaSq = 1.0f - quat[3] * quat[3];

	// Guard against numerical imprecision and identity quaternions.
	if (sinHalfThetaSq <= 0.0f) {

		axis[0] = 1.0f, axis[1] = axis[2] = 0.0f;
		degrees = 0.0f;

	}else {

		float invSinHalfTheta = 1.0f / sqrtf(sinHalfThetaSq);

		axis[0] = quat[0] * invSinHalfTheta;
		axis[1] = quat[1] * invSinHalfTheta;
		axis[2] = quat[2] * invSinHalfTheta;
		degrees = (acosf(quat[2]) * 360.0f) / PI;
	}
}

Matrix4f Quaternion::toMatrix4f() const {
	// Converts this quaternion to a rotation matrix.
	//
	//  | 1 - 2(y^2 + z^2)	2(xy - wz)			2(xz + wy)			0  |
	//  | 2(xy + wz)		1 - 2(x^2 + z^2)	2(yz - wx)			0  |
	//  | 2(xz - wy)		2(yz + wx)			1 - 2(x^2 + y^2)	0  |
	//  | 0					0					0					1  |

	float x2 = quat[0] + quat[0];
	float y2 = quat[1] + quat[1];
	float z2 = quat[2] + quat[2];
	float xx = quat[0] * x2;
	float xy = quat[0] * y2;
	float xz = quat[0] * z2;
	float yy = quat[1] * y2;
	float yz = quat[1] * z2;
	float zz = quat[2] * z2;
	float wx = quat[3] * x2;
	float wy = quat[3] * y2;
	float wz = quat[3] * z2;

	Matrix4f m;

	m[0][0] = 1.0f - (yy + zz);
	m[0][1] = xy - wz;
	m[0][2] = xz + wy;
	m[0][3] = 0.0f;

	m[1][0] = xy + wz;
	m[1][1] = 1.0f - (xx + zz);
	m[1][2] = yz - wx;
	m[1][3] = 0.0f;

	m[2][0] = xz - wy;
	m[2][1] = yz + wx;
	m[2][2] = 1.0f - (xx + yy);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return m;
}

void Quaternion::toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const {
	Matrix4f m = toMatrix4f();
	m.toHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
}

void Quaternion::Normalize(Quaternion &q) {
	float invMag = 1.0f / q.length();
	q.quat[0] *= invMag; q.quat[1] *= invMag; q.quat[2] *= invMag; q.quat[3] *= invMag;
}

Quaternion &Quaternion::FromMatrix(const Matrix4f &m) {
	Quaternion quat;
	quat.fromMatrix(m);
	return quat;
}

Quaternion &Quaternion::FromMatrix(Quaternion &quat, const Matrix4f &m) {
	quat.fromMatrix(m);
	return quat;
}

//friend operator
Quaternion operator*(float lhs, const Quaternion &rhs) {
	return rhs * lhs;
}

//friend operator
bool operator ==(const Quaternion &lhs, const Quaternion &rhs) {
	float epsilon = 0.000001;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon && fabs(lhs[2] - rhs[2]) <= epsilon && fabs(lhs[3] - rhs[3]) <= epsilon;
}