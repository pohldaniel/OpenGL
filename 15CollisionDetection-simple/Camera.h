#ifndef __cameraH__
#define __cameraH__

#include "Vector.h"


class Entity3D;

class Camera{

public:

	Camera();
	Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	~Camera();

	void perspective(float fovx, float aspect, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void move(float dx, float dy, float dz);
	void rotate(float pitch, float yaw, float roll);

	const Matrix4f &getViewMatrix() const;
	const Matrix4f &getInvViewMatrix() const;
	const Matrix4f &getProjectionMatrix() const;
	const Matrix4f &getInvProjectionMatrix() const;
	const Matrix4f &getOrthographicMatrix() const;
	const Vector3f &getPosition() const;
	const Vector3f &getCamX() const;
	const Vector3f &getCamY() const;
	const Vector3f &getCamZ() const;
	const Vector3f &getViewDirection() const;

	void setPosition(float x, float y, float z);
	void setPosition(const Vector3f &position);

protected:

    void rotateFirstPerson(float pitch, float yaw);
	void updateViewMatrix(bool orthogonalizeAxes);
	void updateViewMatrix(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

    Vector3f WORLD_XAXIS;
	Vector3f WORLD_YAXIS;
	Vector3f WORLD_ZAXIS;

    float			m_fovx;
    float			m_znear;
    float			m_zfar;
	float			m_aspectRatio;
	float			m_accumPitchDegrees;

	Vector3f		m_eye;
	Vector3f		m_xAxis;
    Vector3f		m_yAxis;
    Vector3f		m_zAxis;
	Vector3f		m_viewDir;

	Matrix4f		m_viewMatrix;
	Matrix4f		m_invViewMatrix;
	Matrix4f		m_projMatrix;
	Matrix4f		m_invProjMatrix;
	Matrix4f		m_orthMatrix;
};

class ThirdPersonCamera : public Camera{

public:
	ThirdPersonCamera();
	~ThirdPersonCamera();

	void lookAt(const Vector3f &target);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

	using Camera::rotate;
	void rotate(float longitudeDegrees, float latitudeDegrees);
	void update(float elapsedTimeSec);
	void update(float elapsedTimeSec, const Entity3D& entity);

	// getter methods.
	float getDampingConstant() const;
	float getOffsetDistance() const;
	const Quaternion &getOrientation() const;
	
	float getSpringConstant() const;
	bool springSystemIsEnabled() const;

	// setter methods.
	void enableSpringSystem(bool enableSpringSystem);
	void setOffsetDistance(float offsetDistance);
	void setSpringConstant(float springConstant);

	void resetCamera(const Entity3D& entity);

private:
	static const float DEFAULT_SPRING_CONSTANT;
	static const float DEFAULT_DAMPING_CONSTANT;

	void updateOrientation(float elapsedTimeSec);
	void updateViewMatrix();
	void updateViewMatrix(float elapsedTimeSecfloat, const Entity3D& entity);
	void updateViewMatrix(float elapsedTimeSecfloat);

	bool m_enableSpringSystem;
	float m_springConstant;
	float m_dampingConstant;
	float m_offsetDistance;
	float m_headingDegrees;
	float m_pitchDegrees;
	
	Vector3f m_target;
	Vector3f m_up;
	Vector3f m_velocity;
	Quaternion m_orientation;
};


#endif // __cameraH__