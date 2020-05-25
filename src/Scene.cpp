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

void Scene::addEntity(Mesh* mesh, const std::string& name, const std::string& shaderName, const std::string& materialName, const const Eigen::Vector3f& pos)
{
	Entity* entity = new Entity(mesh, name, pos);
	m_bbox.extend(mesh->boundingBox());
	updateCameraWithBbox();
	m_entities.push_back(entity);
	m_entity2Shader.insert(std::make_pair(name, shaderName));
	m_entity2Material.insert(std::make_pair(name, materialName));
}

void Scene::addLight(Light* light)
{
	m_lights.push_back(light);
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
	updateCameraWithBbox();
	m_cam->setScreenViewport(Eigen::AlignedBox2f(Eigen::Vector2f(0.0, 0.0), Eigen::Vector2f(1280, 720)));
}

void Scene::updateCameraWithBbox()
{
	m_cam->setSceneCenter(m_bbox.center());
	m_cam->setSceneRadius(m_bbox.sizes().maxCoeff());
	m_cam->setSceneDistance(m_cam->sceneRadius() * 3.f);
	m_cam->setMinNear(0.1f);
	m_cam->setNearFarOffsets(-m_cam->sceneRadius() * 100.f, m_cam->sceneRadius() * 100.f);
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

void Scene::passTextures(Shader* shader, Material* material)
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
	passPBRMaterial(material, shader, index);
}

void Scene::passPBRMaterial(Material* mat, Shader* shader, int& index)
{
	// ALBEDO
	if (mat->useBasecolorMap)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		shader->setInt("albedo_map", index);
		glBindTexture(GL_TEXTURE_2D, mat->basecolorMap->getId());
		index++;
	}
	else
		shader->setVec3("albedo_val", mat->basecolor);
	shader->setInt("albedo_bool", mat->useBasecolorMap);

	// METALLIC
	if (mat->useMetallicMap)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		shader->setInt("metallic_map", index);
		glBindTexture(GL_TEXTURE_2D, mat->metallicMap->getId());
		index++;
	}
	else
		shader->setFloat("metallic_val", mat->metallic);
	shader->setInt("metallic_bool", mat->useMetallicMap);

	// ROUGHNESS
	if (mat->useRoughnessMap)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		shader->setInt("roughness_map", index);
		glBindTexture(GL_TEXTURE_2D, mat->roughnessMap->getId());
		index++;
	}
	else
		shader->setFloat("roughness_val", mat->roughness);
	shader->setInt("roughness_bool", mat->useRoughnessMap);

	// AO
	if (mat->useAoMap)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		shader->setInt("ao_map", index);
		glBindTexture(GL_TEXTURE_2D, mat->aoMap->getId());
		index++;
	}
	else
		shader->setFloat("ao_val", mat->ao);
	shader->setInt("ao_bool", mat->useAoMap);

	// NORMALS
	if (mat->useNormalMap)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		shader->setInt("normal_map", index);
		glBindTexture(GL_TEXTURE_2D, mat->normalMap->getId());
		index++;
	}
	shader->setInt("normal_bool", mat->useNormalMap);
}

void Scene::renderEntity(Entity* entity, Shader* shader, bool isPBR)
{
	shader->bind();
	passLights(shader);
	shader->setFloat("exposure", m_cubemap->getExposure());
	shader->setFloat("lod", m_cubemap->getLod());
	shader->setInt("max_mip_level", m_cubemap->getMaxMipLevels());
	passWorldMatrices(shader, entity->getTransformationMatrix());
	passTextures(shader, MaterialManager::getInstance()->getMaterial(m_entity2Material[entity->getName()]));
	entity->display(shader);
	shader->unbind();
}

void Scene::computeCubemap()
{
	m_cubemap = new CubeMap();
}