#ifndef _VECTOR_H
#define _VECTOR_H

#include <cfloat>
#include <cmath>
#include <algorithm>
//-----------------------------------------------------------------------------
// Common math functions and constants.
//-----------------------------------------------------------------------------
#define PI  3.1415926535897932384
#define TWO_PI  6.2831853071795864769 
#define PI_ON_180  0.0174532925199432957
#define invPI  0.3183098861837906715
#define	invTWO_PI  0.1591549430918953358


class Vector2f{

	friend Vector2f operator-(const Vector2f &v);

public:
	Vector2f();
	Vector2f(float x_, float y_);
	~Vector2f();



	float &operator[](int index);
	const float operator[](int index) const;

	const float* getVec()const;

	Vector2f &operator+=(const Vector2f &rhs);
	Vector2f &operator-=(const Vector2f &rhs);

	Vector2f operator+(const Vector2f &rhs) const;
	Vector2f operator-(const Vector2f &rhs) const;

	Vector2f operator*(float scalar) const;
	Vector2f operator/(float scalar) const;

private:

	float vec[2];

};

class Vector3f
{

    friend Vector3f operator-(const Vector3f &v);
	friend bool operator ==(Vector3f lhs, Vector3f rhs);
public:
    
    Vector3f() ;
    Vector3f(float x_, float y_, float z_);
    ~Vector3f();

	static Vector3f cross(const Vector3f &p, const Vector3f &q);
    static float dot(const Vector3f &p, const Vector3f &q);
	static void normalize(Vector3f &p);
	static Vector3f Min(const Vector3f &p, const Vector3f &q);
	static Vector3f Max(const Vector3f &p, const Vector3f &q);

	Vector3f normalize();
	float magnitude() const;
	
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

class Vector4f{

public:
	Vector4f();
	Vector4f(float x_, float y_, float z_, float w_);
	Vector4f(const Vector3f &rhs);
	~Vector4f();

	float &operator[](int index);
	const float operator[](int index) const;

private:

	float vec[4];
};


class Matrix4f
{
	friend Vector3f operator*(const Vector4f &lhs, const Matrix4f &rhs);
	friend Vector3f operator*(const Matrix4f &rhs, const Vector4f &lhs);
	friend Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs);
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
	Matrix4f &operator*=(const Matrix4f &rhs);
	Matrix4f operator*(const Matrix4f &rhs) const;

	void identity();
	void rotate(const Vector3f &axis, float degrees);
	void invRotate(const Vector3f &axis, float degrees);
	void invTranslate(float dx, float dy, float dz);
	void invScale(float a, float b, float c);
	void perspective(float fovx, float aspect, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

private:
	float mtx[4][4];
};

#endif


