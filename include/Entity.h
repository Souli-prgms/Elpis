#pragma once

#include "Mesh.h"

class Entity
{
public:
	Entity(Mesh* mesh, const std::string& name, const Eigen::Vector3f& position);
	~Entity();

	void display(Shader* shader);

	const Eigen::Affine3f& getTransformationMatrix() const { return m_transformation; }
	void setTransformationMatrix(const Eigen::Affine3f& transfo) { m_transformation = transfo; }
	const Eigen::Vector3f getPosition() const { return m_transformation.translation(); }
	void translate(float dx, float dy, float dz);
	void rotate(float angle, const Eigen::Vector3f& axis);
	void scale(float sx, float sy, float sz);
	std::string getName() const { return m_name; }

private:
	std::string m_name;
	Eigen::Affine3f m_transformation;
	Mesh* m_mesh;
};