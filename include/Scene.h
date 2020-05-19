#pragma once

#include <map>

#include "Light.h"
#include "Entity.h"
#include "CubeMap.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addShader(Shader* shader, const std::string& name);
	void addEntity(Mesh* mesh, const std::string& name, const std::string& shaderName, const std::map<std::string, std::string>& textureMap, const const Eigen::Vector3f& pos);
	void addLight(Light* light);
	void addTexture(Texture* texture, const std::string& name);
	
	void computeCubemap();
	void render();

	Camera* getCamera() const { return m_cam; }

private:
	Camera* m_cam;
	CubeMap* m_cubemap;

	std::map<std::string, Shader*> m_shaders;
	std::vector<Entity*> m_entities;
	std::map<std::string, Texture*> m_textures;
	std::vector<Light*> m_lights;
	
	std::map<std::string, std::string> m_entity2Shader;
	std::map<std::string, std::map<std::string, std::string>> m_entity2TextureMap;
	
	void initCamera();
	void passLights(Shader* shader);
	void passWorldMatrices(Shader* shader, const Eigen::Affine3f& transformation);
	void passTextures(Shader* shader, const std::map<std::string, std::string>& textureMap);
	void renderEntity(Entity* entity, Shader* shader, bool isPBR);
};