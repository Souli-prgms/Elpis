#pragma once

#include "Event.h"
#include "MaterialManager.h"
#include "Light.h"
#include "Entity.h"
#include "CubeMap.h"
#include "Timestep.h"

namespace Elpis
{
	class Scene
	{
	public:
		using Entities = std::vector<Ref<Entity>>;
		using Lights = std::vector<Ref<Light>>;

		Scene();
		~Scene();

		void addEntity(const Ref<Mesh>& mesh, const std::string& name, const std::string& shaderName, const std::string& materialName, const const Vec3& pos);
		void addLight(const Ref<Light>& light);

		void setCubemap(const std::string& filepath);
		void onUpdate(Timestep ts);

		Ref<Camera> getCamera() const { return m_cam; }
		Ref<Entity> getEntity(int index) const { return m_entities[index]; }
		Ref<CubeMap> getCubemap() const { return m_cubemap; }

		void onEvent(Event& e);
	private:
		Ref<Camera> m_cam;
		Ref<CubeMap> m_cubemap;
		Box3 m_bbox;

		Entities m_entities;
		Lights m_lights;

		void initCamera();
		void passLights(const Ref<Shader>& shader);
		void passWorldMatrices(const Ref<Shader>& shader, const Eigen::Affine3f& transformation);
		void passTextures(const Ref<Shader>& shader, const Ref<Material>& material);
		void renderEntity(const Ref<Entity>& entity, const Ref<Shader>& shader, bool isPBR);
		void updateCameraWithBbox();
		void passPBRMaterial(const Ref<Material>& mat, const Ref<Shader>& shader, int& index);
	};
}