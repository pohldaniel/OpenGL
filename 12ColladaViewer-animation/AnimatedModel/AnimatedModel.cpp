#include <iostream>
#include <fstream>

#include "Texture.h"
#include "AnimatedModel.h"

void AnimatedModel::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {

	modelMatrix->setRotPos(axis, degrees, dx, dy, dz);
}

void AnimatedModel::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz) {

	modelMatrix->setRotXYZPos(axisX, degreesX,
		axisY, degreesY,
		axisZ, degreesZ,
		dx, dy, dz);
}

void AnimatedModel::rotate(const Vector3f &axis, float degrees) {

	modelMatrix->rotate(axis, degrees);
}

void AnimatedModel::translate(float dx, float dy, float dz) {

	modelMatrix->translate(dx, dy, dz);
}

void AnimatedModel::scale(float a, float b, float c) {

	modelMatrix->scale(a, b, c);
}

const Matrix4f &AnimatedModel::getTransformationMatrix() const {

	return modelMatrix->getTransformationMatrix();
}

const Matrix4f &AnimatedModel::getInvTransformationMatrix() const {

	return modelMatrix->getInvTransformationMatrix();
}


AnimatedModel::AnimatedModel() : _shader(".\\res\\animationShader") {
	_animator = std::make_shared<Animator>(this);
	_texture = std::make_shared<Texture>();

	modelMatrix = new ModelMatrix();
}

void AnimatedModel::LoadModel(const std::string &filename, const std::string &texture){
	
	ColladaLoader loader(filename);


	_meshes.push_back(std::make_shared<AnimatedMesh>(loader));
	
	_animator->addAnimation(loader);
	_texture->setTexture(texture);	
}


void AnimatedModel::Update(double elapsedTime){
	_animator->Update(elapsedTime);
}

void AnimatedModel::Draw(Camera camera){
	
	_shader.Bind();
	for (auto mesh : _meshes){
		
		_texture->Bind(0);
		_shader.Update(*this, camera, mesh->GetBoneArray());

		mesh->Draw();
	}
}
