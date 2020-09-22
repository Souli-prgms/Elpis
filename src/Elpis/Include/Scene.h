#pragma once

#include "MaterialManager.h"
#include "Light.h"
#include "Entity.h"
#include "CubeMap.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addShader(const Ref<Shader>& shader, const std::string& name);
	void addEntity(const Ref<Mesh>& mesh, const std::string& name, const std::string& shaderName, const std::string& materialName, const const Vec3& pos);
	void addLight(const Ref<Light>& light);
	
	void setCubemap(const std::string& filepath);
	void render();

	Ref<Camera> getCamera() const { return m_cam; }
	Ref<Entity> getEntity(int index) const { return m_entities[index]; }
	Ref<CubeMap> getCubemap() const { return m_cubemap; }

private:
	Ref<Camera> m_cam;
	Ref<CubeMap> m_cubemap;
	Box3 m_bbox;

	std::map<std::string, Ref<Shader>> m_shaders;
	std::vector<Ref<Entity>> m_entities;
	
	std::vector<Ref<Light>> m_lights;
	
	std::map<std::string, std::string> m_entity2Shader;
	std::map<std::string, std::string> m_entity2Material;
	
	void initCamera();
	void passLights(const Ref<Shader>& shader);
	void passWorldMatrices(const Ref<Shader>& shader, const Eigen::Affine3f& transformation);
	void passTextures(const Ref<Shader>& shader, const Ref<Material>& material);
	void renderEntity(const Ref<Entity>& entity, const Ref<Shader>& shader, bool isPBR);
	void updateCameraWithBbox();
	void passPBRMaterial(const Ref<Material>& mat, const Ref<Shader>& shader, int& index);
};