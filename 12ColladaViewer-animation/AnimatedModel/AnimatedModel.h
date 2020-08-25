#ifndef JOINT_H
#define JOINT_H

#include <glm\glm.hpp>
#include <vector>
#include <memory>

class Joint : public std::enable_shared_from_this<Joint>{

public:
	inline glm::mat4					getTransform() { return _jointTransform; }
	inline void							setTransform(glm::mat4 transform) { _jointTransform = transform; }
	inline void							setName(std::string name) { _name = name; }
	inline std::string					getName() { return _name; }
	std::vector<std::shared_ptr<Joint>>	getChildren() { return _children; }
	void								addChild(std::shared_ptr<Joint> j) { _children.push_back(j); }

	Joint() {}

	std::shared_ptr<Joint> Find(const std::string& name){
		if (_name == name) return shared_from_this();
		for (auto joint : _children){
			std::shared_ptr<Joint> j = joint->Find(name);
			if (j != nullptr) return j;
		}
		return nullptr;
	}

private:
	glm::mat4 _jointTransform;
	std::vector<std::shared_ptr<Joint>> _children;
	std::string _name;
};

#endif

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
	void LoadJointHierarchy(const std::string &filename);

	void InitJointHierarchy(std::shared_ptr<Joint> root, const glm::mat4 &parentTransform);
	void UpdateMeshBone(std::string jointName, const glm::mat4& globalTransform);

	
	void Update(double elapsedTime);
	void Draw(Camera camera);

	std::shared_ptr<Animator> getAnimator() { return _animator; }
	std::shared_ptr<Joint> getRootJoint() { return _rootJoint; }

	std::shared_ptr<Joint> FindJoint(const std::string& jointName);


	AnimationShader						_shader;
	glm::mat4							_inverseModelMatrix;

	std::vector<std::shared_ptr<AnimatedMesh>>	_meshes;
	std::shared_ptr<Texture>			_texture;	
	std::shared_ptr<Animator>			_animator;

	std::shared_ptr<Joint>				_rootJoint;

	std::shared_ptr<Joint>				LoadJoint(TiXmlNode* node);

private:
	ModelMatrix *modelMatrix;
};
#endif