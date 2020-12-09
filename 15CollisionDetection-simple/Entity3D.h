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

	const Vector3f &getForwardVector() const;
	const Vector3f &getPosition() const;
	const Vector3f &getRightVector() const;
	const Vector3f &getUpVector() const;
	const Vector3f &getVelocity() const;
	const Matrix4f &getWorldMatrix() const;

	void setPosition(float x, float y, float z);
	void setVelocity(float x, float y, float z);
	void setWorldMatrix(const Matrix4f &worldMatrix);
	void update(float elapsedTimeSec);

	void applyForce(unsigned long direction, float force);


	void setTraction(float traction) { m_traction = traction; }
	void setSurfaceFriction(float friction) { m_surfaceFriction = friction; }
	void setVelocityCol(float x, float y, float z) { m_velocityCol[0] = x, m_velocityCol[1] = y, m_velocityCol[2] = z; }
	void setVelocityCol(const Vector3f &velocity) { m_velocityCol = velocity; }
	void setVelocityColXZ(float x, float z) { m_velocityCol[0] = x;  m_velocityCol[2] = z; }
	void setVelocityColY(float y) { m_velocityCol[1] = y; }
	void setCamOffset(float x, float y, float z) { m_camOffset[0] = x, m_camOffset[1] = y, m_camOffset[2] = z; }
	void setCamOffset(const Vector3f &camOffset) { m_camOffset = camOffset; }
	void setGrounded(bool grounded) { m_grounded = grounded; }

	const Vector3f &getVelocityCol() const { return m_velocityCol; }
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

	Vector3f m_appliedForce;
	Vector3f m_gravity;
	Vector3f m_velocityCol;

	bool m_grounded = true;
	float m_traction;
	float m_airResistance;
	float m_surfaceFriction;
	float m_mass;

	bool m_constrainedToWorldYAxis;
};

#endif