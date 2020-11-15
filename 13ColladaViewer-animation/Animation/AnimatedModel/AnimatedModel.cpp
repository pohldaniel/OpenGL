#include <iostream>
#include <fstream>

#include "Texture.h"
#include "AnimatedModel.h"



AnimatedModel::AnimatedModel() : m_shader("./Animation/animationShader.vert", "./Animation/animationShader.frag") {
	m_animator = std::make_shared<Animator>(this);
	m_texture = std::make_shared<Texture>();

	m_modelMatrix = ModelMatrix();
}

void AnimatedModel::loadModel(const std::string &filename, const std::string &texture){
	
	ColladaLoader loader(filename);

	m_meshes.push_back(std::make_shared<AnimatedMesh>(loader));
	
	m_animator->addAnimation(loader);
	m_texture->setTexture(texture);	
}


void AnimatedModel::update(double elapsedTime){
	m_animator->update(elapsedTime);
}

void AnimatedModel::draw(Camera camera){
	
	m_shader.bind();
	for (auto mesh : m_meshes){
		
		m_texture->bind(0);
		m_shader.update(*this, camera, mesh->getBoneArray());

		mesh->draw();
	}
}

void AnimatedModel::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {

	m_modelMatrix.setRotPos(axis, degrees, dx, dy, dz);
}

void AnimatedModel::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz) {

	m_modelMatrix.setRotXYZPos(axisX, degreesX, axisY, degreesY, axisZ, degreesZ, dx, dy, dz);
}

void AnimatedModel::rotate(const Vector3f &axis, float degrees) {

	m_modelMatrix.rotate(axis, degrees);
}

void AnimatedModel::translate(float dx, float dy, float dz) {

	m_modelMatrix.translate(dx, dy, dz);
}

void AnimatedModel::scale(float a, float b, float c) {

	m_modelMatrix.scale(a, b, c);
}

const Matrix4f &AnimatedModel::getTransformationMatrix() const {

	return m_modelMatrix.getTransformationMatrix();
}

const Matrix4f &AnimatedModel::getInvTransformationMatrix() const {

	return m_modelMatrix.getInvTransformationMatrix();
}
