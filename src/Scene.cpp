#include "Scene.h"

Scene::Scene()
{
	initCamera();
}

Scene::~Scene()
{
}

void Scene::addShader(Shader* shader, const std::string& name)
{
	m_shaders.insert(std::make_pair(name, shader));
}

void Scene::addEntity(Mesh* mesh, const std::string& name, const std::string& shaderName, const std::map<std::string, std::string>& textureMap, const const Eigen::Vector3f& pos)
{
	Entity* entity = new Entity(mesh, name, pos);
	m_entities.push_back(entity);
	m_entity2Shader.insert(std::make_pair(name, shaderName));
	m_entity2TextureMap.insert(std::make_pair(name, textureMap));
}

void Scene::addLight(Light* light)
{
	m_lights.push_back(light);
}

void Scene::addTexture(Texture* texture, const std::string& name)
{
	m_textures.insert(std::make_pair(name, texture));
}

void Scene::render()
{
	for (std::vector<Entity*>::iterator it = m_entities.begin(); it < m_entities.end(); it++)
	{
		renderEntity((*it), m_shaders[m_entity2Shader[(*it)->getName()]], true);
	}
	m_cubemap->render(m_cam);
}

void Scene::initCamera() 
{
	m_cam = new Camera;
	m_cam->setSceneCenter(Eigen::Vector3f(0, 0, 0));
	m_cam->setSceneRadius(3.0);
	m_cam->setSceneDistance(m_cam->sceneRadius() * 3.f);
	m_cam->setMinNear(0.1f);
	m_cam->setNearFarOffsets(-m_cam->sceneRadius() * 100.f, m_cam->sceneRadius() * 100.f);
	m_cam->setScreenViewport(Eigen::AlignedBox2f(Eigen::Vector2f(0.0, 0.0), Eigen::Vector2f(1280, 720)));
}

void Scene::passLights(Shader* shader)
{
	shader->setInt("nb_lights", m_lights.size());
	for (int i = 0; i < m_lights.size(); i++)
	{
		Light* light = m_lights[i];

		if (light->getType() == LightType::Point) {
			Eigen::Vector3f pos = light->getPosition();
			shader->setVec4(("point_lights[" + std::to_string(i) + "].position").c_str(), m_cam->computeViewMatrix() * Eigen::Vector4f(pos.x(), pos.y(), pos.z(), 1.0f));
			shader->setVec3(("point_lights[" + std::to_string(i) + "].ambient").c_str(), light->getAmbient());
			shader->setVec3(("point_lights[" + std::to_string(i) + "].diffuse").c_str(), light->getDiffuse());
			shader->setVec3(("point_lights[" + std::to_string(i) + "].specular").c_str(), light->getSpecular());
			shader->setFloat(("point_lights[" + std::to_string(i) + "].constant").c_str(), light->getConstant());
			shader->setFloat(("point_lights[" + std::to_string(i) + "].linear").c_str(), light->getLinear());
			shader->setFloat(("point_lights[" + std::to_string(i) + "].quadratic").c_str(), light->getQuadratic());
		}
	}
}

void Scene::passWorldMatrices(Shader* shader, const Eigen::Affine3f& transformation)
{
	shader->setMat4("projection_matrix", m_cam->computeProjectionMatrix());
	shader->setMat4("view_matrix", m_cam->computeViewMatrix());
	shader->setMat4("model_matrix", transformation.matrix());
	shader->setVec3("camera_position", m_cam->getPosition());
}

void Scene::passTextures(Shader* shader, const std::map<std::string, std::string>& textureMap)
{
	glActiveTexture(GL_TEXTURE0);
	shader->setInt("irradiance_map", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap->getIrradianceMap());
	glActiveTexture(GL_TEXTURE1);
	shader->setInt("prefilter_map", 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap->getPreFilterMap());
	glActiveTexture(GL_TEXTURE2);
	shader->setInt("brdf_map", 2);
	glBindTexture(GL_TEXTURE_2D, m_cubemap->getBRDFTexture());
	int index = 3;
	for (auto const& x : textureMap)
	{
		shader->setInt(x.first.c_str(), index);
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, m_textures[x.second]->getId());
		index++;
	}
}

void Scene::renderEntity(Entity* entity, Shader* shader, bool isPBR)
{
	shader->bind();
	passLights(shader);
	passWorldMatrices(shader, entity->getTransformationMatrix());
	passTextures(shader, m_entity2TextureMap[entity->getName()]);

	if (isPBR)
		shader->setFloat("ao", 1.0);

	entity->display(shader);
	shader->unbind();
}

void Scene::computeCubemap()
{
	m_cubemap = new CubeMap();
}