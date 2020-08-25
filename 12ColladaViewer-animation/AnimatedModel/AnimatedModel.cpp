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
	_rootJoint = std::make_shared<Joint>();
	_texture = std::make_shared<Texture>();

	modelMatrix = new ModelMatrix();
}

void AnimatedModel::LoadModel(const std::string &filename, const std::string &texture){
	
	_meshes.push_back(std::make_shared<AnimatedMesh>(filename));
	
	LoadJointHierarchy(filename);
	_animator->addAnimation(filename, _rootJoint.get()->getName());
	_texture->setTexture(texture);
	
	
}


void AnimatedModel::LoadJointHierarchy(const std::string &filename) {
	
	TiXmlDocument doc(filename.c_str());
	doc.LoadFile();
	TiXmlElement* rootNode = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node")->FirstChildElement("node");
	TiXmlElement* firstChild = (TiXmlElement*)rootNode->FirstChild();
	
	_rootJoint->setName(std::string(rootNode->Attribute("name")));
	while (firstChild != NULL) {
		if (std::string(firstChild->Value()).compare("node") == 0) {
			_rootJoint->addChild(LoadJoint(firstChild));
		}
		firstChild = firstChild->NextSiblingElement();
	}
	
}

std::shared_ptr<Joint> AnimatedModel::LoadJoint(TiXmlNode* node) {
	TiXmlElement* firstChild = (TiXmlElement*)node->FirstChild();

	std::shared_ptr<Joint> j = std::make_shared<Joint>();
	j->setName(std::string(((TiXmlElement*)node)->Attribute("name")));

	while (firstChild != NULL) {
		if (std::string(firstChild->Value()).compare("node") == 0) {
			j->addChild(LoadJoint(firstChild));
		}
		firstChild = firstChild->NextSiblingElement();
	}
	return j;
}



void AnimatedModel::InitJointHierarchy(std::shared_ptr<Joint> root, const glm::mat4 & parentTransform){
	

	glm::mat4 globalTransform = parentTransform * root->getTransform();
	
	//Insert configure bones final transforms for each mesh
	UpdateMeshBone(root->getName(), globalTransform);
	
	for (std::shared_ptr<Joint> j : root->getChildren()){
		InitJointHierarchy(j, globalTransform);
	}
}

void AnimatedModel::UpdateMeshBone(std::string jointName, const glm::mat4 & globalTransform){
	
	for (auto mesh : _meshes){

		if (mesh->_boneIdMap.find(jointName) == mesh->_boneIdMap.end()) continue;
		unsigned int id = mesh->_boneIdMap[jointName];
		mesh->_boneDataMap[id].finalTransformation = globalTransform * mesh->_boneDataMap[id].offsetMatrix;

	}
}

void AnimatedModel::Update(double elapsedTime){
	_animator->Update(elapsedTime);
}

void AnimatedModel::Draw(Camera camera){
	
	_shader.Bind();
	for (auto mesh : _meshes){
		
		_texture->Bind(0);
		_shader.Update(*this, camera, mesh->GetBoneArray2());

		mesh->Draw();
	}
}

std::shared_ptr<Joint> AnimatedModel::FindJoint(const std::string& jointName){

	return _rootJoint->Find(jointName);
}

