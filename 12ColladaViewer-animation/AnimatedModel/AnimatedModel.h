#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include <vector>
#include "AnimatedMesh.h"
#include "..\Animator\Animator.h"
#include "..\Camera.h"
#include "..\ModelMatrix.h"
#include "..\Render\AnimationShader.h"
#include "Texture.h"
#include <memory>

class AnimatedModel : public std::enable_shared_from_this<AnimatedModel>{

public:

	AnimatedModel();
	virtual ~AnimatedModel() {}

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);
	void setRotXYZPos(const Vector3f &axisX, float degreesX,
		const Vector3f &axisY, float degreesY,
		const Vector3f &axisZ, float degreesZ,
		float dx, float dy, float dz);

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void LoadModel(const std::string &filename, const std::string &texture);
	
	void Update(double elapsedTime);
	void Draw(Camera camera);

	std::shared_ptr<Animator> getAnimator() { return _animator; }

	AnimationShader						_shader;

	std::vector<std::shared_ptr<AnimatedMesh>>	_meshes;
	std::shared_ptr<Texture>			_texture;	
	std::shared_ptr<Animator>			_animator;

private:
	ModelMatrix *modelMatrix;



};
#endif