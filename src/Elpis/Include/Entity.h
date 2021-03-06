#pragma once

#include "Mesh.h"

namespace Elpis 
{
	class Entity
	{
	public:
		Entity(const Ref<Mesh>& mesh, const std::string& name, const Vec3& position, const std::string& material, const std::string shader);
		~Entity();

		void display(const Ref<Shader>& shader);

		const Eigen::Affine3f& getTransformationMatrix() const { return m_transformation; }
		void setTransformationMatrix(const Eigen::Affine3f& transfo) { m_transformation = transfo; }
		const Vec3 getPosition() const { return m_transformation.translation(); }
		void translate(float dx, float dy, float dz);
		void rotate(float angle, const Vec3& axis);
		void scale(float sx, float sy, float sz);
		std::string getName() const { return m_name; }
		void setMesh(const Ref<Mesh>& mesh) { m_mesh = mesh; }

		const std::string& getMaterial() { return m_material; }
		const std::string& getShader() { return m_shader; }

	private:
		std::string m_name, m_material, m_shader;
		Eigen::Affine3f m_transformation;
		Ref<Mesh> m_mesh;
	};
}