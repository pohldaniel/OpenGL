#include <cmath>
#include <iostream>
#include "camera.h"
#include "Entity3D.h"
Camera::Camera(){
	
	WORLD_XAXIS = Vector3f(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = Vector3f(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = Vector3f(0.0f, 0.0f, 1.0f);

	m_fovx = 45.0f;
	m_znear = 0.1f;
	m_zfar = 1000.0f;
    m_aspectRatio = 0.0f;
	m_accumPitchDegrees = 0.0f;

    m_eye.set(0.0f, 0.0f, 0.0f);
    m_xAxis.set(1.0f, 0.0f, 0.0f);
    m_yAxis.set(0.0f, 1.0f, 0.0f);
    m_zAxis.set(0.0f, 0.0f, 1.0f);
    m_viewDir.set(0.0f, 0.0f, -1.0f);
    
    m_projMatrix.identity();
	m_orthMatrix.identity();
	
	updateViewMatrix(false);
}

Camera::Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {
	m_accumPitchDegrees = 0.0f;
	m_eye = eye;
	m_projMatrix.identity();
	m_orthMatrix.identity();
	updateViewMatrix(eye, target, up);
}

Camera::~Camera() {}

void Camera::updateViewMatrix(bool orthogonalizeAxes){

	// Regenerate the camera's local axes to orthogonalize them.
	if (orthogonalizeAxes){

		Vector3f::Normalize(m_zAxis);

		m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
		Vector3f::Normalize(m_yAxis);

		m_xAxis = Vector3f::Cross(m_yAxis, m_zAxis);
		Vector3f::Normalize(m_xAxis);

		m_viewDir = -m_zAxis;
	}

	// Reconstruct the view matrix.
	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[3][0] = 0.0f;

	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[3][1] = 0.0f;

	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;


	m_invViewMatrix[0][0] = m_xAxis[0];
	m_invViewMatrix[1][0] = m_xAxis[1];
	m_invViewMatrix[2][0] = m_xAxis[2];
	m_invViewMatrix[3][0] = 0.0f;

	m_invViewMatrix[0][1] = m_yAxis[0];
	m_invViewMatrix[1][1] = m_yAxis[1];
	m_invViewMatrix[2][1] = m_yAxis[2];
	m_invViewMatrix[3][1] = 0.0f;

	m_invViewMatrix[0][2] = m_zAxis[0];
	m_invViewMatrix[1][2] = m_zAxis[1];
	m_invViewMatrix[2][2] = m_zAxis[2];
	m_invViewMatrix[3][2] = 0.0f;

	m_invViewMatrix[0][3] = m_eye[0];
	m_invViewMatrix[1][3] = m_eye[1];
	m_invViewMatrix[2][3] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::updateViewMatrix(const Vector3f &eye, const Vector3f &target, const Vector3f &up){

	m_zAxis = eye - target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	WORLD_YAXIS = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(WORLD_YAXIS);

	m_viewDir = -m_zAxis;

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[3][0] = 0.0f;

	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[3][1] = 0.0f;

	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, eye);
	m_viewMatrix[3][3] = 1.0f;

	Matrix4f invView;
	invView.invLookAt(eye, target, up);
	m_invViewMatrix = invView;
}

void Camera::perspective(float fovx, float aspect, float znear, float zfar){
	// Construct a projection matrix based on the horizontal field of view
	// 'fovx' rather than the more traditional vertical field of view 'fovy'.

	float e = 1.0f /tanf(PI*fovx / 360.0f);
	float xScale = e / aspect;
	float yScale = e;

	m_projMatrix[0][0] = xScale;
	m_projMatrix[1][0] = 0.0f;
	m_projMatrix[2][0] = 0.0f;
	m_projMatrix[3][0] = 0.0f;

	m_projMatrix[0][1] = 0.0f;
	m_projMatrix[1][1] = yScale;
	m_projMatrix[2][1] = 0.0f;
	m_projMatrix[3][1] = 0.0f;

	m_projMatrix[0][2] = 0.0f;
	m_projMatrix[1][2] = 0.0f;
	m_projMatrix[2][2] = (zfar + znear) / (znear - zfar);
	m_projMatrix[3][2] = -1.0f;

	m_projMatrix[0][3] = 0.0f;
	m_projMatrix[1][3] = 0.0f;
	m_projMatrix[2][3] = (2.0f * zfar * znear) / (znear - zfar);
	m_projMatrix[3][3] = 0.0f;

	m_fovx = fovx;
	m_aspectRatio = aspect;
	m_znear = znear;
	m_zfar = zfar;	
}

void Camera::orthographic(float left, float right, float bottom, float top, float znear, float zfar){

	m_orthMatrix[0][0] = 2 / (right - left);
	m_orthMatrix[1][0] = 0.0f;
	m_orthMatrix[2][0] = 0.0f;
	m_orthMatrix[3][0] = 0.0f;

	m_orthMatrix[0][1] = 0.0f;
	m_orthMatrix[1][1] = 2 / (top - bottom);
	m_orthMatrix[2][1] = 0.0f;
	m_orthMatrix[3][1] = 0.0f;

	m_orthMatrix[0][2] = 0.0f;
	m_orthMatrix[1][2] = 0.0f;
	m_orthMatrix[2][2] = 2 / (znear - zfar);
	m_orthMatrix[3][2] = 0.0f;

	m_orthMatrix[0][3] = (right + left) / (left - right);
	m_orthMatrix[1][3] = (top + bottom) / (bottom - top);
	m_orthMatrix[2][3] = (zfar + znear) / (znear - zfar);
	m_orthMatrix[3][3] = 1.0f;

	m_znear = znear;
	m_zfar = zfar;
}

void Camera::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up){
	
	m_zAxis = eye - target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	WORLD_YAXIS = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(WORLD_YAXIS);

	m_viewDir = -m_zAxis;


	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[3][0] = 0.0f;

	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[3][1] = 0.0f;

	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, eye);
	m_viewMatrix[3][3] = 1.0f;

	// Extract the pitch angle from the view matrix.
	m_accumPitchDegrees = -asinf(m_viewMatrix[1][2])*180.f / PI;
}

void Camera::move(float dx, float dy, float dz){

	Vector3f eye = m_eye;
	eye += m_xAxis * dx;
	eye += WORLD_YAXIS * dy;
	eye += m_viewDir * dz;
	setPosition(eye);
}

void Camera::rotate(float yaw, float pitch, float roll){
	rotateFirstPerson(pitch, yaw);
	updateViewMatrix(true);
}

void Camera::rotateFirstPerson(float pitch, float yaw){

	m_accumPitchDegrees += pitch;

	if (m_accumPitchDegrees > 90.0f){

		pitch = 90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = 90.0f;
	}

	if (m_accumPitchDegrees < -90.0f){

		pitch = -90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = -90.0f;
	}

	Matrix4f rotMtx;

	// Rotate camera's existing x and z axes about the world y axis.
	if (yaw != 0.0f){

		rotMtx.rotate(WORLD_YAXIS, yaw);
		m_xAxis = rotMtx * m_xAxis;
		m_zAxis = rotMtx * m_zAxis;
	}

	// Rotate camera's existing y and z axes about its existing x axis.
	if (pitch != 0.0f){

		rotMtx.rotate(m_xAxis, pitch);
		m_yAxis = rotMtx * m_yAxis;
		m_zAxis = rotMtx * m_zAxis;
	}

	//std::cout << m_xAxis[0] << "  " << m_xAxis[1] << "  " << m_xAxis[2] << std::endl;
}

void Camera::setPosition(float x, float y, float z){
	m_eye.set(x, y, z);
	updateViewMatrix(true);
}

void Camera::setPosition(const Vector3f &position){
	m_eye = position;
	updateViewMatrix(true);
}

const Vector3f &Camera::getPosition() const{
	return m_eye;
}

const Vector3f &Camera::getCamX() const{
	return m_xAxis;
}

const Vector3f &Camera::getCamY() const{
	return m_yAxis;
}

const Vector3f &Camera::getCamZ() const {
	return m_zAxis;
}

const Vector3f &Camera::getViewDirection() const{
	return m_viewDir;
}

const Matrix4f &Camera::getProjectionMatrix() const{
	return m_projMatrix;
}

const Matrix4f &Camera::getInvProjectionMatrix() const{
	return  m_invProjMatrix;
}

const Matrix4f &Camera::getOrthographicMatrix() const{
	return m_orthMatrix;
}

const Matrix4f &Camera::getViewMatrix() const{
	return m_viewMatrix;
}

const Matrix4f &Camera::getInvViewMatrix() const{
	return m_invViewMatrix;
}

////////////////////////////////////////////////////////////////////////////////////////
const float ThirdPersonCamera::DEFAULT_SPRING_CONSTANT = 32.0f;
const float ThirdPersonCamera::DEFAULT_DAMPING_CONSTANT = 8.0f;

ThirdPersonCamera::ThirdPersonCamera(){
	m_enableSpringSystem = true;
	m_springConstant = DEFAULT_SPRING_CONSTANT;
	m_dampingConstant = DEFAULT_DAMPING_CONSTANT;

	m_offsetDistance = 0.0f;
	m_headingDegrees = 0.0f;
	m_pitchDegrees = 0.0f;

	m_fovx = 80.0f;
	m_znear = 1000.0f;
	m_zfar = 1.0f;

	m_eye.set(0.0f, 0.0f, 0.0f);
	m_target.set(0.0f, 0.0f, 0.0f);
	m_up.set(0.0f, 1.0f, 0.0f);

	m_xAxis.set(1.0f, 0.0f, 0.0f);
	m_yAxis.set(0.0f, 1.0f, 0.0f);
	m_zAxis.set(0.0f, 0.0f, 1.0f);
	m_viewDir.set(0.0f, 0.0f, -1.0f);

	m_velocity.set(0.0f, 0.0f, 0.0f);

	m_viewMatrix.identity();
	m_projMatrix.identity();
	m_orientation.identity();
}

ThirdPersonCamera::~ThirdPersonCamera() {}

void ThirdPersonCamera::lookAt(const Vector3f &target){
	m_target = target;
}

void ThirdPersonCamera::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up){

	m_eye = eye;
	m_target = target;
	m_up = up;

	m_zAxis = eye - target;
	Vector3f::Normalize(m_zAxis);

	m_viewDir = -m_zAxis;

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	//build a transposed viewMatrix for initial converting to orientation
	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, eye);

	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, eye);

	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, eye);

	m_orientation.fromMatrix(m_viewMatrix);
	Vector3f offset = m_target - m_eye;

	m_offsetDistance = offset.length();
}

void ThirdPersonCamera::rotate(float headingDegrees, float pitchDegrees){
	m_headingDegrees = -headingDegrees;
	m_pitchDegrees = -pitchDegrees;
}

void ThirdPersonCamera::enableSpringSystem(bool enableSpringSystem){
	m_enableSpringSystem = enableSpringSystem;
}

void ThirdPersonCamera::setOffsetDistance(float offsetDistance){
	m_offsetDistance = offsetDistance;
}

void ThirdPersonCamera::setSpringConstant(float springConstant){
	// We're using a critically damped spring system where the damping ratio
	// is equal to one.
	//
	// damping ratio = m_dampingConstant / (2.0f * sqrtf(m_springConstant))
	m_springConstant = springConstant;
	m_dampingConstant = 2.0f * sqrtf(springConstant);
}

void ThirdPersonCamera::update(float elapsedTimeSec) {

	updateOrientation(elapsedTimeSec);

	if (m_enableSpringSystem)
		updateViewMatrix(elapsedTimeSec);
	else
		updateViewMatrix();
}

void ThirdPersonCamera::updateViewMatrix(float elapsedTimeSec) {

	m_viewMatrix = m_orientation.toMatrix4f();

	m_xAxis.set(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]);
	m_yAxis.set(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]);
	m_zAxis.set(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]);

	// Calculate the new camera position. The 'idealPosition' is where the
	// camera should be position. The camera should be positioned directly
	// behind the target at the required offset distance. What we're doing here
	// is rather than have the camera immediately snap to the 'idealPosition'
	// we slowly move the camera towards the 'idealPosition' using a spring
	// system.
	//
	// References:
	//   Stone, Jonathan, "Third-Person Camera Navigation," Game Programming
	//     Gems 4, Andrew Kirmse, Editor, Charles River Media, Inc., 2004.

	Vector3f idealPosition = m_target + m_zAxis * m_offsetDistance;
	Vector3f displacement = m_eye - idealPosition;
	Vector3f springAcceleration = (-m_springConstant * displacement) - (m_dampingConstant * m_velocity);

	m_velocity += springAcceleration * elapsedTimeSec;
	m_eye += m_velocity * elapsedTimeSec;

	// The view matrix is always relative to the camera's current position
	// 'm_eye'. Since a spring system is being used here 'm_eye' will be
	// relative to 'idealPosition'. When the camera is no longer being
	// moved 'm_eye' will become the same as 'idealPosition'. The local
	// x, y, and z axes that were extracted from the camera's orientation
	// 'm_orienation' is correct for the 'idealPosition' only. We need
	// to recompute these axes so that they're relative to 'm_eye'. Once
	// that's done we can use those axes to reconstruct the view matrix.

	m_zAxis = m_eye - m_target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(m_up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_viewMatrix.identity();

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);

	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);

	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);

	m_viewMatrix[3][0] = 0.0f;
	m_viewMatrix[3][1] = 0.0f;
	m_viewMatrix[3][2] = 0.0f;
	m_viewMatrix[3][3] = 1.0f;

	m_viewDir = -m_zAxis;
}

void ThirdPersonCamera::update(float elapsedTimeSec, const Entity3D& entity) {

	updateOrientation(elapsedTimeSec);

	if (m_enableSpringSystem)
		updateViewMatrix(elapsedTimeSec, entity);
	else
		updateViewMatrix(entity);
}

void ThirdPersonCamera::updateViewMatrix(float elapsedTimeSec, const Entity3D& entity){
	
	m_orientation = entity.m_orientation.inverse();

	m_xAxis.set(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]);
	m_yAxis.set(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]);
	m_zAxis.set(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]);

	// Calculate the new camera position. The 'idealPosition' is where the
	// camera should be position. The camera should be positioned directly
	// behind the target at the required offset distance. What we're doing here
	// is rather than have the camera immediately snap to the 'idealPosition'
	// we slowly move the camera towards the 'idealPosition' using a spring
	// system.
	//
	// References:
	//   Stone, Jonathan, "Third-Person Camera Navigation," Game Programming
	//     Gems 4, Andrew Kirmse, Editor, Charles River Media, Inc., 2004.
	
	//Vector3f idealPosition = m_target + m_zAxis * m_offsetDistance;
	Vector3f idealPosition = entity.getPosition() - entity.getForwardVector() * m_offsetDistance + entity.getUpVector() * 150.f;

	Vector3f displacement = m_eye - idealPosition;
	Vector3f springAcceleration = (-m_springConstant * displacement) - (m_dampingConstant * m_velocity);

	m_velocity += springAcceleration * elapsedTimeSec;
	m_eye += m_velocity * elapsedTimeSec;

	// The view matrix is always relative to the camera's current position
	// 'm_eye'. Since a spring system is being used here 'm_eye' will be
	// relative to 'idealPosition'. When the camera is no longer being
	// moved 'm_eye' will become the same as 'idealPosition'. The local
	// x, y, and z axes that were extracted from the camera's orientation
	// 'm_orienation' is correct for the 'idealPosition' only. We need
	// to recompute these axes so that they're relative to 'm_eye'. Once
	// that's done we can use those axes to reconstruct the view matrix.

	m_zAxis = m_eye - m_target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(m_up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_viewMatrix.identity();

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);

	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);

	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);

	m_viewMatrix[3][0] = 0.0f;
	m_viewMatrix[3][1] = 0.0f;
	m_viewMatrix[3][2] = 0.0f;
	m_viewMatrix[3][3] = 1.0f;

	m_viewDir = -m_zAxis;
}

void ThirdPersonCamera::updateOrientation(float elapsedTimeSec) {
	m_pitchDegrees *= elapsedTimeSec;
	m_headingDegrees *= elapsedTimeSec;

	Quaternion rot;

	if (m_headingDegrees != 0.0f) {
		rot.fromAxisAngle(m_up, m_headingDegrees);
		m_orientation = rot * m_orientation;
	}

	if (m_pitchDegrees != 0.0f) {
		rot.fromAxisAngle(WORLD_XAXIS, m_pitchDegrees);
		m_orientation = m_orientation * rot;
	}
}

void ThirdPersonCamera::updateViewMatrix() {

	m_viewMatrix = m_orientation.toMatrix4f();

	m_xAxis.set(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]);
	m_yAxis.set(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]);
	m_zAxis.set(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]);
	m_viewDir = -m_zAxis;

	m_eye = m_target + m_zAxis * m_offsetDistance;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);
}

void ThirdPersonCamera::updateViewMatrix(const Entity3D& entity) {

	m_viewMatrix = m_orientation.toMatrix4f();

	m_xAxis.set(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]);
	m_yAxis.set(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]);
	m_zAxis.set(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]);
	m_viewDir = -m_zAxis;

	m_eye = entity.getPosition() - entity.getForwardVector() * m_offsetDistance + entity.getUpVector() * 150.f;
	m_target = entity.getPosition();

	m_zAxis = m_eye - m_target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(m_up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_viewMatrix.identity();

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);

	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);

	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);

	m_viewMatrix[3][0] = 0.0f;
	m_viewMatrix[3][1] = 0.0f;
	m_viewMatrix[3][2] = 0.0f;
	m_viewMatrix[3][3] = 1.0f;

	m_viewDir = -m_zAxis;
}

float ThirdPersonCamera::getDampingConstant() const{
	return m_dampingConstant;
}

float ThirdPersonCamera::getOffsetDistance() const{
	return m_offsetDistance;
}

const Quaternion &ThirdPersonCamera::getOrientation() const{
	return m_orientation;
}

float ThirdPersonCamera::getSpringConstant() const{
	return m_springConstant;
}

bool ThirdPersonCamera::springSystemIsEnabled() const{
	return m_enableSpringSystem;
}


void ThirdPersonCamera::resetCamera(const Entity3D& entity) {

	m_orientation = entity.m_orientation.inverse();

	m_viewMatrix = m_orientation.toMatrix4f();

	//m_eye = entity.getPosition() - entity.getForwardVector() * m_offsetDistance;

	m_xAxis.set(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]);
	m_yAxis.set(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]);
	m_zAxis.set(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]);
	//m_eye.set(m_viewMatrix[3][0], m_viewMatrix[3][1], m_viewMatrix[3][2]);

	

	m_eye = entity.getPosition() - entity.getForwardVector() * m_offsetDistance + entity.getUpVector() * 150.f;

	//std::cout << m_eye[0] << "  " << m_eye[1] << "  " << m_eye[2] << std::endl;

	/*Vector3f idealPosition = entity.getPosition() + entity.getForwardVector() * m_offsetDistance ;
	Vector3f displacement = m_eye - idealPosition;
	Vector3f springAcceleration = (-m_springConstant * displacement) - (m_dampingConstant * m_velocity);

	m_velocity += springAcceleration * 1.0;
	m_eye += m_velocity * 1.0;*/

	// The view matrix is always relative to the camera's current position
	// 'm_eye'. Since a spring system is being used here 'm_eye' will be
	// relative to 'idealPosition'. When the camera is no longer being
	// moved 'm_eye' will become the same as 'idealPosition'. The local
	// x, y, and z axes that were extracted from the camera's orientation
	// 'm_orienation' is correct for the 'idealPosition' only. We need
	// to recompute these axes so that they're relative to 'm_eye'. Once
	// that's done we can use those axes to reconstruct the view matrix.

	m_zAxis = m_eye - m_target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(m_up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_viewMatrix.identity();

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);

	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);

	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);

	m_viewMatrix[3][0] = 0.0f;
	m_viewMatrix[3][1] = 0.0f;
	m_viewMatrix[3][2] = 0.0f;
	m_viewMatrix[3][3] = 1.0f;

	m_viewDir = -m_zAxis;
}