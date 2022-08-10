#include <iostream>
#include "Entity3D.h"

Entity3D::Entity3D(){
	m_worldMatrix.identity();
	m_orientation.identity();
	m_rotation.identity();

	m_right.set(1.0f, 0.0f, 0.0f);
	m_up.set(0.0f, 1.0f, 0.0f);
	m_forward.set(0.0f, 0.0f, -1.0f);

	m_position.set(0.0f, 0.0f, 0.0f);
	m_velocity.set(0.0f, 0.0f, 0.0f);
	m_eulerOrient.set(0.0f, 0.0f, 0.0f);
	m_eulerRotate.set(0.0f, 0.0f, 0.0f);

	m_constrainedToWorldYAxis = false;
}

Entity3D::~Entity3D(){

}

void Entity3D::constrainToWorldYAxis(bool constrain){
	// Constraining rotations to the world Y axis means that all heading
	// changes are applied to the world Y axis rather than the entity's
	// local Y axis.
	m_constrainedToWorldYAxis = constrain;
}

void Entity3D::orient(float headingDegrees, float pitchDegrees, float rollDegrees){
	// orient() changes the direction the entity is facing. This directly
	// affects the orientation of the entity's right, up, and forward vectors.
	// orient() is usually called in response to the user's input if the entity
	// is able to be moved by the user.
	m_eulerOrient[0] += pitchDegrees;
	m_eulerOrient[1] += headingDegrees;
	m_eulerOrient[2] += rollDegrees;

	if (m_eulerOrient[0] > 360.0f)
		m_eulerOrient[0] -= 360.0f;

	if (m_eulerOrient[0] < -360.0f)
		m_eulerOrient[0] += 360.0f;

	if (m_eulerOrient[1] > 360.0f)
		m_eulerOrient[1] -= 360.0f;

	if (m_eulerOrient[1] < -360.0f)
		m_eulerOrient[1] += 360.0f;

	if (m_eulerOrient[2] > 360.0f)
		m_eulerOrient[2] -= 360.0f;

	if (m_eulerOrient[2] < -360.0f)
		m_eulerOrient[2] += 360.0f;
}

void Entity3D::rotate(float headingDegrees, float pitchDegrees, float rollDegrees){
	// rotate() does not change the direction the entity is facing. This method
	// allows the entity to freely spin around without affecting its orientation
	// and its right, up, and forward vectors. For example, if this entity is
	// a planet, then rotate() is used to spin the planet on its y axis. If this
	// entity is an asteroid, then rotate() is used to tumble the asteroid as
	// it moves in space.
	m_eulerRotate[0] += pitchDegrees;
	m_eulerRotate[1] += headingDegrees;
	m_eulerRotate[2] += rollDegrees;

	if (m_eulerRotate[0] > 360.0f)
		m_eulerRotate[0] -= 360.0f;

	if (m_eulerRotate[0] < -360.0f)
		m_eulerRotate[0] += 360.0f;

	if (m_eulerRotate[1] > 360.0f)
		m_eulerRotate[1] -= 360.0f;

	if (m_eulerRotate[1] < -360.0f)
		m_eulerRotate[1] += 360.0f;

	if (m_eulerRotate[2] > 360.0f)
		m_eulerRotate[2] -= 360.0f;

	if (m_eulerRotate[2] < -360.0f)
		m_eulerRotate[2] += 360.0f;
}

const Vector3f &Entity3D::getForwardVector() const{
	return m_forward;
}

const Vector3f &Entity3D::getPosition() const{
	return m_position;
}

const Vector3f &Entity3D::getRightVector() const{
	return m_right;
}

const Vector3f &Entity3D::getUpVector() const{
	return m_up;
}

const Vector3f &Entity3D::getVelocity() const{
	return m_velocity;
}

const Matrix4f &Entity3D::getWorldMatrix() const{
	return m_worldMatrix;
}

void Entity3D::setPosition(float x, float y, float z){
	m_position.set(x, y, z);
}

void Entity3D::setVelocity(float x, float y, float z){
	m_velocity.set(x, y, z);
}

void Entity3D::setWorldMatrix(const Matrix4f &worldMatrix){
	m_worldMatrix = worldMatrix;
	m_orientation.fromMatrix(worldMatrix);
	m_position.set(worldMatrix[0][3], worldMatrix[1][3], worldMatrix[2][3]);
	extractAxes();
}

void Entity3D::update(float elapsedTimeSec){

	Vector3f velocityElapsed, eulerOrientElapsed, eulerRotateElapsed;
	Vector3f oldPos, heading;
	Quaternion temp;

	velocityElapsed = m_velocity * elapsedTimeSec;
	eulerOrientElapsed = m_eulerOrient * elapsedTimeSec;
	eulerRotateElapsed = m_eulerRotate * elapsedTimeSec;

	// Update the entity's position.
	extractAxes();

	oldPos = m_position;

	m_position += m_right * velocityElapsed[0];
	m_position += m_up * velocityElapsed[1];
	m_position += m_forward * velocityElapsed[2];

	heading = m_position - oldPos;
	heading.normalize();

	// Update the entity's orientation.
	Matrix4f tmp = m_orientation.toMatrix4f();
	temp = eulerToQuaternion(m_orientation.toMatrix4f(), eulerOrientElapsed[1],eulerOrientElapsed[0], eulerOrientElapsed[2]);

	// When moving backwards invert rotations to match direction of travel.
	if (Vector3f::Dot(heading, m_forward) < 0.0f)
		temp = Quaternion::Inverse(temp);

	m_orientation *= temp;
	m_orientation.normalize();

	// Update the entity's free rotation.
	temp = eulerToQuaternion(m_rotation.toMatrix4f(), eulerRotateElapsed[1],
		eulerRotateElapsed[0], eulerRotateElapsed[2]);

	m_rotation *= temp;
	m_rotation.normalize();

	// Update the entity's world matrix.
	temp = m_rotation * m_orientation;
	temp.normalize();

	m_worldMatrix = temp.toMatrix4f();
	m_worldMatrix[0][3] = m_position[0];
	m_worldMatrix[1][3] = m_position[1];
	m_worldMatrix[2][3] = m_position[2];

	// Clear the entity's cached euler rotations and velocity for this frame.
	m_velocity.set(0.0f, 0.0f, 0.0f);
	m_eulerOrient.set(0.0f, 0.0f, 0.0f);
	m_eulerRotate.set(0.0f, 0.0f, 0.0f);
}

Quaternion Entity3D::eulerToQuaternion(const Matrix4f &m, float headingDegrees, float pitchDegrees, float rollDegrees) const{
	// Construct a quaternion from an euler transformation. We do this rather
	// than use Quaternion::fromHeadPitchRoll() to support constraining heading
	// changes to the world Y axis.

	Quaternion result = Quaternion::IDENTITY;
	Quaternion rotation = Quaternion::IDENTITY;
	Vector3f localXAxis(m[0][0], m[1][0], m[2][0]);
	Vector3f localYAxis(m[0][1], m[1][1], m[2][1]);
	Vector3f localZAxis(m[0][2], m[1][2], m[2][2]);

	if (headingDegrees != 0.0f) {
		if (m_constrainedToWorldYAxis)
		  rotation.fromAxisAngle(Vector3f(0.0f, 1.0f, 0.0f), headingDegrees);
		else 
		  rotation.fromAxisAngle(localYAxis, headingDegrees);
		
		result *= rotation;
	}

	if (pitchDegrees != 0.0f){
		rotation.fromAxisAngle(localXAxis, pitchDegrees);
		result *= rotation;
	}

	if (rollDegrees != 0.0f){
		rotation.fromAxisAngle(localZAxis, rollDegrees);
		result *= rotation;
	}

	return result;
}

void Entity3D::extractAxes(){
	Matrix4f m = m_orientation.toMatrix4f();

	m_right.set(m[0][0], m[1][0], m[2][0]);
	m_right.normalize();

	m_up.set(m[0][1], m[1][1], m[2][1]);
	m_up.normalize();

	m_forward.set(-m[0][2], -m[1][2], -m[2][2]);
	m_forward.normalize();
}