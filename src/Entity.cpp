#include "Entity.h"

Entity::Entity(Mesh* mesh, const std::string& name, const Eigen::Vector3f& position): m_mesh(mesh), m_name(name), m_transformation(Eigen::Matrix4f::Identity())
{
	translate(position.x(), position.y(), position.z());
}

Entity::~Entity()
{
}

void Entity::display(Shader* shader)
{
	m_mesh->display(shader);
}

void Entity::translate(float dx, float dy, float dz)
{
	m_transformation *= Eigen::Translation3f(dx, dy, dz);
}
void Entity::rotate(float angle, const Eigen::Vector3f& axis)
{
	m_transformation *= Eigen::AngleAxis(angle, axis);
}

void Entity::scale(float sx, float sy, float sz)
{
	m_transformation *= Eigen::Scaling(sx, sy, sz);
}