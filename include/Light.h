#pragma once

#include "Eigen/Eigen"

enum LightType
{
	Directional, Point
};

class Light
{
public:
	Light(LightType type, const Eigen::Vector3f& vector) : m_type(type), m_ambient(Eigen::Vector3f(1.0, 1.0, 1.0)), m_diffuse(Eigen::Vector3f(1.0, 1.0, 1.0)), m_specular(Eigen::Vector3f(1.0, 1.0, 1.0))
	{
		if (type == LightType::Directional)
			m_direction = vector;
		else if (type == LightType::Point)
			m_position = vector;
	}
	~Light() {};

	LightType getType() const { return m_type; }
	Eigen::Vector3f getAmbient() const { return m_ambient; }
	Eigen::Vector3f getDiffuse() const { return m_diffuse; }
	Eigen::Vector3f getSpecular() const { return m_specular; }
	Eigen::Vector3f getPosition() const { return m_position; }
	Eigen::Vector3f getDirection() const { return m_direction; }
	float getConstant() const { return m_constant; }
	float getLinear() const { return m_linear; }
	float getQuadratic() const { return m_quadratic; }

private:
	LightType m_type;

	float m_constant = 1.0f;
	float m_linear = 0.14f;
	float m_quadratic = 0.07f;

	Eigen::Vector3f m_position;
	Eigen::Vector3f m_direction;

	Eigen::Vector3f m_ambient;
	Eigen::Vector3f m_diffuse;
	Eigen::Vector3f m_specular;
};