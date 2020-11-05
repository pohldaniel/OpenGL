#ifndef ENTITY3D_3D2_H
#define ENTITY3D_3D2_H

#include "Vector.h"

//-----------------------------------------------------------------------------
// The Entity3D class tracks an object's position and orientation in 3D space.
//
// An entity can be rotated and oriented. By default all heading changes will
// be about the entity's local Y axis. The constrainToWorldYAxis() method
// forces all heading changes to be about the world Y axis rather that the
// entity's local Y axis.
//
// Changing the entity's orientation using the orient() method will update the
// direction the entity is facing. This will also update the entity's right,
// up, and forward vectors.
//
// Rotating an entity using the rotate() method will not update the direction
// the entity is facing. The rotate() method is provided as a means to animate
// the entity.
//
// For example, consider an Entity3D object that represents an asteroid. The
// rotate() method would be used to tumble the asteroid as it moves through
// space. The orient() method would be used to alter the direction the asteroid
// is moving.
//-----------------------------------------------------------------------------

class Entity3D{

public:
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

private:
	Quaternion eulerToQuaternion(const Matrix4f &m, float headingDegrees, float pitchDegrees, float rollDegrees) const;

	void extractAxes();

	Matrix4f m_worldMatrix;
	Quaternion m_orientation;
	Quaternion m_rotation;

	Vector3f m_right;
	Vector3f m_up;
	Vector3f m_forward;

	Vector3f m_position;
	Vector3f m_velocity;
	Vector3f m_eulerOrient;
	Vector3f m_eulerRotate;

	bool m_constrainedToWorldYAxis;
};

#endif