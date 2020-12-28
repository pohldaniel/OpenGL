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
	//m_meshes.push_back(std::make_shared<AnimatedMesh>(loader));
	
	std::map<std::string, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>> geometry;
	loader.loadGeometryAndController(geometry);

	std::map<std::string, std::tuple<std::vector<Vector3f>, std::vector<Vector3f>, std::vector<Vector2f>, std::vector<unsigned int>, short>>::iterator it;

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> texCoords;
	std::vector<unsigned int> indices;
	short vertexLayout;

	std::vector<std::array<unsigned int, 4>> jointIds;
	std::vector<Vector4f> jointWeights;
	std::vector<std::string> jointsList;
	std::vector<VertexSkinData> skinningData;
	std::string name;

	SkeletonData skeletonData;
	Matrix4f modelMatrix;
	Joint rootJoint;

	for (it = geometry.begin(); it != geometry.end(); it++){
		name = it->first;
		if (loader.loadController(name, jointIds, jointWeights, jointsList, skinningData)) {
			std::cout << name << std::endl;
		}else {
			std::cout << name << std::endl;
		}

		vertexLayout = std::get<4>(it->second);
		positions = std::get<0>(it->second);
		normals = std::get<1>(it->second);
		texCoords = std::get<2>(it->second);
		indices = std::get<3>(it->second);

		loader.createIndexBuffer(vertexLayout, positions, normals, texCoords, indices, jointIds, jointWeights, skinningData);

		loader.loadVisualScene(name, jointsList, skeletonData, modelMatrix);
		loader.createJoints(skeletonData, rootJoint);

		m_meshes.push_back(std::make_shared<SkeletonAnimatedMesh >(positions, normals, texCoords, indices, jointIds, jointWeights, rootJoint, jointsList, loader));
	}


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
		//m_shader.update(*this, camera, dynamic_cast<SkeletonAnimatedMesh*>(mesh.get())->getBoneArray());
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
