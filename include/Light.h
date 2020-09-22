#pragma once

#include "Core.h"

enum LightType
{
	Directional, Point
};

class Light
{
public:
	Light(LightType type, const Vec3& vector) : m_type(type), m_ambient(Vec3(1.0, 1.0, 1.0)), m_diffuse(Vec3(1.0, 1.0, 1.0)), m_specular(Vec3(1.0, 1.0, 1.0))
	{
		if (type == LightType::Directional)
			m_direction = vector;
		else if (type == LightType::Point)
			m_position = vector;
	}
	~Light() {};

	LightType getType() const { return m_type; }
	Vec3 getAmbient() const { return m_ambient; }
	Vec3 getDiffuse() const { return m_diffuse; }
	Vec3 getSpecular() const { return m_specular; }
	Vec3 getPosition() const { return m_position; }
	Vec3 getDirection() const { return m_direction; }
	float getConstant() const { return m_constant; }
	float getLinear() const { return m_linear; }
	float getQuadratic() const { return m_quadratic; }

private:
	LightType m_type;

	float m_constant = 1.0f;
	float m_linear = 0.14f;
	float m_quadratic = 0.07f;

	Vec3 m_position;
	Vec3 m_direction;

	Vec3 m_ambient;
	Vec3 m_diffuse;
	Vec3 m_specular;
};