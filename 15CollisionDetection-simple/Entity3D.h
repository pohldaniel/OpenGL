#ifndef ENTITY3D_3D2_H
#define ENTITY3D_3D2_H

#include "Camera.h"
#include "Vector.h"

class Entity3D {

public:
	friend class  ThirdPersonCamera;

	enum DIRECTION {
		DIR_FORWARD = 1,
		DIR_BACKWARD = 2,
		DIR_LEFT = 4,
		DIR_RIGHT = 8,
		DIR_UP = 16,
		DIR_DOWN = 32,

		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	Entity3D();
	~Entity3D();

	void constrainToWorldYAxis(bool constrain);

	void orient(float headingDegrees, float pitchDegrees, float rollDegrees);
	void rotate(float headingDegrees, float pitchDegrees, float rollDegrees);
	void update(float elapsedTimeSec);

	void setPosition(float x, float y, float z) { m_position.set(x, y, z); }
	void setVelocity(float x, float y, float z) { m_velocity.set(x, y, z); }
	void setVelocity(const Vector3f &velocity) { m_velocity = velocity; }
	void setVelocityXZ(float x, float z) { m_velocity[0] = x;  m_velocity[2] = z; }
	void setVelocityY(float y) { m_velocity[1] = y; }
	void setCamOffset(float x, float y, float z) { m_camOffset[0] = x, m_camOffset[1] = y, m_camOffset[2] = z; }
	void setCamOffset(const Vector3f &camOffset) { m_camOffset = camOffset; }
	void setGrounded(bool grounded) { m_grounded = grounded; }
	void setWorldMatrix(const Matrix4f &worldMatrix);

	const Vector3f &getForwardVector() const;
	const Vector3f &getPosition() const;
	const Vector3f &getRightVector() const;
	const Vector3f &getUpVector() const;
	const Vector3f &getVelocity() const;
	const Matrix4f &getWorldMatrix() const;
	const bool isGrounded() const { return  m_grounded; }


private:

	Quaternion eulerToQuaternion(const Matrix4f &m, float headingDegrees, float pitchDegrees, float rollDegrees) const;
	void extractAxes();

	Quaternion m_orientation;
	Quaternion m_rotation;

	Vector3f m_right;
	Vector3f m_up;
	Vector3f m_forward;
	Vector3f m_position;
	Vector3f m_eulerOrient;
	Vector3f m_eulerRotate;

	Vector3f m_velocity;
	Vector3f m_camOffset;

	Matrix4f m_worldMatrix;

	bool m_grounded = true;
	float m_traction;
	float m_airResistance;
	float m_surfaceFriction;
	float m_mass;

	bool m_constrainedToWorldYAxis;
};

#endif