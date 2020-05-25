#pragma once

#include <map>

#include "MaterialManager.h"
#include "Light.h"
#include "Entity.h"
#include "CubeMap.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addShader(Shader* shader, const std::string& name);
	void addEntity(Mesh* mesh, const std::string& name, const std::string& shaderName, const std::string& materialName, const const Eigen::Vector3f& pos);
	void addLight(Light* light);
	
	void computeCubemap();
	void render();

	Camera* getCamera() const { return m_cam; }
	Entity* getEntity(int index) const { return m_entities[index]; }
	CubeMap* getCubemap() const { return m_cubemap; }

private:
	Camera* m_cam;
	CubeMap* m_cubemap;
	Eigen::AlignedBox3f m_bbox;

	std::map<std::string, Shader*> m_shaders;
	std::vector<Entity*> m_entities;
	
	std::vector<Light*> m_lights;
	
	std::map<std::string, std::string> m_entity2Shader;
	std::map<std::string, std::string> m_entity2Material;
	
	void initCamera();
	void passLights(Shader* shader);
	void passWorldMatrices(Shader* shader, const Eigen::Affine3f& transformation);
	void passTextures(Shader* shader, Material* material);
	void renderEntity(Entity* entity, Shader* shader, bool isPBR);
	void updateCameraWithBbox();
	void passPBRMaterial(Material* mat, Shader* shader, int& index);
};