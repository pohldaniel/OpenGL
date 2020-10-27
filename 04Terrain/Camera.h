#ifndef __cameraH__
#define __cameraH__

#include "Vector.h"

class Camera{

public:
	enum CameraBehavior{CAMERA_BEHAVIOR_FIRST_PERSON, CAMERA_BEHAVIOR_FLIGHT, CAMERA_BEHAVIOR_SPACECRAFT};

	Camera();
	~Camera();

	void perspective(float fovx, float aspect, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void move(float dx, float dy, float dz);
	void rotate(float pitch, float yaw, float roll);

	const Matrix4f &getViewMatrix() const;
	const Matrix4f &getProjectionMatrix() const;
	const Vector3f &getPosition() const;

	void setPosition(float x, float y, float z);
	void setPosition(const Vector3f &position);

private:

	void rotateFlight(float pitch, float yaw, float roll);
    void rotateFirstPerson(float pitch, float yaw);
	void updateViewMatrix(bool orthogonalizeAxes);

	static const float DEFAULT_FOVX;
    static const float DEFAULT_ZFAR;
    static const float DEFAULT_ZNEAR;
    static const Vector3f WORLD_XAXIS;
    static const Vector3f WORLD_YAXIS;
    static const Vector3f WORLD_ZAXIS;

	CameraBehavior  m_behavior;
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
	Matrix4f		m_projMatrix;
};
#endif // __cameraH__