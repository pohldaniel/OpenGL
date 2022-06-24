#ifndef _VECTOR_H
#define _VECTOR_H

#include <cfloat>
#include <cmath>
#include <algorithm>
//-----------------------------------------------------------------------------
// Common math functions and constants.
//-----------------------------------------------------------------------------
#define PI  3.1415926535897932384
#define HALF_PI  1.57079632679
#define TWO_PI  6.2831853071795864769 
#define PI_ON_180  0.0174532925199432957
#define _180_ON_PI  57.2957795131
#define invPI  0.3183098861837906715
#define	invTWO_PI  0.1591549430918953358

class Vector2f {

	friend Vector2f operator-(const Vector2f &v);
	friend bool operator ==(Vector2f lhs, Vector2f rhs);
public:
	Vector2f();
	Vector2f(float x_, float y_);
	Vector2f(const Vector2f &rhs);
	~Vector2f();

	float &operator[](int index);
	const float operator[](int index) const;

	const float* getVec()const;

	//Vector2f &operator^(const Vector2f &rhs);
	Vector2f& operator= (const Vector2f& rhs);
	

	Vector2f &operator+=(const Vector2f &rhs);
	Vector2f &operator-=(const Vector2f &rhs);

	Vector2f operator+(const Vector2f &rhs) const;
	Vector2f operator-(const Vector2f &rhs) const;

	Vector2f operator*(float scalar) const;
	Vector2f operator/(float scalar) const;

private:

	float vec[2];

};

class Vector3f {

	friend Vector3f operator-(const Vector3f &v);
	friend Vector3f operator*(float lhs, const Vector3f &rhs);
	friend bool operator ==(Vector3f lhs, Vector3f rhs);
public:

	Vector3f();
	Vector3f(float x_, float y_, float z_);
	~Vector3f();

	static Vector3f Cross(const Vector3f &p, const Vector3f &q);
	static float Dot(const Vector3f &p, const Vector3f &q);
	static void Normalize(Vector3f &p);
	static Vector3f Min(const Vector3f &p, const Vector3f &q);
	static Vector3f Max(const Vector3f &p, const Vector3f &q);

	Vector3f normalize();
	float length() const;
	float lengthSq() const;

	void set(float x_, float y_, float z_);

	float &operator[](int index);
	const float operator[](int index) const;

	const float* getVec()const;

	Vector3f &operator+=(const Vector3f &rhs);
	Vector3f &operator-=(const Vector3f &rhs);

	Vector3f &operator+(const Vector3f &rhs) const;
	Vector3f operator-(const Vector3f &rhs) const;

	Vector3f operator*(float scalar) const;
	Vector3f operator/(float scalar) const;


	bool zero();

private:
	float vec[3];
};

class Vector4f {

	friend Vector4f operator*(float lhs, const Vector4f &rhs);
	friend Vector4f operator*(const Vector4f &lhs, float rhs);
public:
	Vector4f();
	Vector4f(float x_, float y_, float z_, float w_);
	Vector4f(const Vector3f &rhs);
	~Vector4f();

	float &operator[](int index);
	const float operator[](int index) const;

	Vector4f &operator+=(const Vector4f &rhs);
	Vector4f &operator+(const Vector4f &rhs) const;

private:

	float vec[4];
};


class Matrix4f {

	friend Vector3f operator*(const Vector4f &lhs, const Matrix4f &rhs);
	friend Vector3f operator*(const Matrix4f &rhs, const Vector4f &lhs);
	//friend Vector3f operator&(const Matrix4f &rhs, const Vector4f &lhs);
	friend Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs);
	friend Vector3f operator*(const Matrix4f &lhs, const Vector3f &rhs);
	friend Vector4f operator^(const Vector4f &rhs, const Matrix4f &lhs);
	friend Vector4f operator^(const Matrix4f &lhs, const Vector4f &rhs);
	friend Matrix4f operator*(float scalar, const Matrix4f &rhs);

public:


	static const Matrix4f IDENTITY;

	Matrix4f();
	Matrix4f(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	~Matrix4f();

	float *operator[](int row);
	const float *operator[](int row) const;
	Matrix4f &operator+=(const Matrix4f &rhs);
	Matrix4f &operator*=(const Matrix4f &rhs);
	Matrix4f &operator^=(const Matrix4f &rhs);
	Matrix4f operator+(const Matrix4f &rhs) const;
	Matrix4f operator*(const Matrix4f &rhs) const;
	Matrix4f operator^(const Matrix4f &rhs) const;
	Matrix4f transpose();
	Matrix4f inverse() const;
	float determinant() const;

	void identity();
	void rotate(const Vector3f &axis, float degrees);
	void invRotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void invTranslate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	void invScale(float a, float b, float c);
	void perspective(float fovx, float aspect, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void perspectiveD3D(float fovx, float aspect, float znear, float zfar);
	void linearPerspectiveD3D(float fovx, float aspect, float znear, float zfar);
	void invPerspective(float fovx, float aspect, float znear, float zfar);
	void invOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void invLookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

	void fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees);
	void toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const;

	void print();

	static Matrix4f &GetNormalMatrix(const Matrix4f &modelViewMatrix);
	static Matrix4f &GetNormalMatrix(Matrix4f &mtx, const Matrix4f &modelViewMatrix);

	static Matrix4f &Scale(float x, float y, float z);
	static Matrix4f &Scale(Matrix4f &mtx, float x, float y, float z);

	static Matrix4f &Translate(const float dx, const float dy, const float dz);
	static Matrix4f &Translate(Matrix4f &mtx, const float dx, const float dy, const float dz);

	static Matrix4f &GetPerspective(float fovx, float aspect, float znear, float zfar);
	static Matrix4f &GetPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar);

	static Matrix4f &GetInvPerspective(float fovx, float aspect, float znear, float zfar);
	static Matrix4f &GetInvPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar);

	static Matrix4f &GetOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix4f &GetOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar);

	static Matrix4f &GetInvOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix4f &GetInvOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar);

	static void Transpose(Matrix4f &p);

private:
	float mtx[4][4];
};


class Quaternion {

	friend Quaternion operator*(float lhs, const Quaternion &rhs);
	friend bool operator ==(const Quaternion &lhs, const Quaternion &rhs);

public:
	static const Quaternion IDENTITY;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(float headDegrees, float pitchDegrees, float rollDegrees);
	Quaternion(const Vector3f &axis, float degrees);
	explicit Quaternion(const Matrix4f &m);
	~Quaternion() {}

	float &operator[](int index);
	const float operator[](int index) const;
	bool operator==(const Quaternion &rhs) const;
	bool operator!=(const Quaternion &rhs) const;

	Quaternion &operator+=(const Quaternion &rhs);
	Quaternion &operator-=(const Quaternion &rhs);
	Quaternion &operator*=(const Quaternion &rhs);
	Quaternion &operator*=(float scalar);
	Quaternion &operator/=(float scalar);

	Quaternion operator+(const Quaternion &rhs) const;
	Quaternion operator-(const Quaternion &rhs) const;
	Quaternion operator*(const Quaternion &rhs) const;
	Quaternion operator*(float scalar) const;
	Quaternion operator/(float scalar) const;

	void identity();
	float length() const;
	void normalize();
	void set(float x, float y, float z, float w);
	Quaternion conjugate() const;
	Quaternion inverse() const;

	void fromAxisAngle(const Vector3f &axis, float degrees);
	void fromMatrix(const Matrix4f &m);

	void fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees);

	void toAxisAngle(Vector3f &axis, float &degrees) const;
	Matrix4f toMatrix4f() const;
	void toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const;

	static Quaternion &FromMatrix(const Matrix4f &m);
	static Quaternion &FromMatrix(Quaternion &quat, const Matrix4f &m);
	static void Normalize(Quaternion &p);
private:
	float quat[4];
};
#endif


