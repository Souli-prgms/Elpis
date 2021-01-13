#include "Scene.h"

namespace Elpis
{
	Scene::Scene()
	{
		initCamera();
	}

	Scene::~Scene()
	{
	}

	void Scene::addEntity(const Ref<Mesh>& mesh, const std::string& name, const std::string& shaderName, const std::string& materialName, const const Vec3& pos)
	{
		Ref<Entity> entity = createRef<Entity>(mesh, name, pos, materialName, shaderName);
		m_bbox.extend(mesh->boundingBox());
		updateCameraWithBbox();
		m_entities.push_back(entity);
	}

	void Scene::addLight(const Ref<Light>& light)
	{
		m_lights.push_back(light);
	}

	void Scene::onUpdate(Timestep ts)
	{
		for (Ref<Entity>& entity : m_entities)
			renderEntity(entity, SHADER_LIB->get(entity->getShader()), true);

		m_cubemap->render(m_cam);
	}

	void Scene::initCamera()
	{
		m_cam = createRef<Camera>();
		updateCameraWithBbox();
		m_cam->setScreenViewport(Eigen::AlignedBox2f(Vec2(0.0, 0.0), Vec2(1920, 1080)));
	}

	void Scene::updateCameraWithBbox()
	{
		m_cam->setSceneCenter(m_bbox.center());
		m_cam->setSceneRadius(m_bbox.sizes().maxCoeff());
		m_cam->setSceneDistance(m_cam->sceneRadius() * 3.f);
		m_cam->setMinNear(0.1f);
		m_cam->setNearFarOffsets(-m_cam->sceneRadius() * 100.f, m_cam->sceneRadius() * 100.f);
	}

#define PT_LIGHTS "point_lights["

	void Scene::passLights(const Ref<Shader>& shader)
	{
		shader->setInt("nb_lights", m_lights.size());
		for (int i = 0; i < m_lights.size(); i++)
		{
			Ref<Light> light = m_lights[i];

			if (light->getType() == LightType::Point)
			{
				Vec3& pos = light->getPosition();
				shader->setVec4((PT_LIGHTS + std::to_string(i) + "].position").c_str(), m_cam->computeViewMatrix() * Vec4(pos.x(), pos.y(), pos.z(), 1.0f));
				shader->setVec3((PT_LIGHTS + std::to_string(i) + "].ambient").c_str(), light->getAmbient());
				shader->setVec3((PT_LIGHTS + std::to_string(i) + "].diffuse").c_str(), light->getDiffuse());
				shader->setVec3((PT_LIGHTS + std::to_string(i) + "].specular").c_str(), light->getSpecular());
				shader->setFloat((PT_LIGHTS + std::to_string(i) + "].constant").c_str(), light->getConstant());
				shader->setFloat((PT_LIGHTS + std::to_string(i) + "].linear").c_str(), light->getLinear());
				shader->setFloat((PT_LIGHTS + std::to_string(i) + "].quadratic").c_str(), light->getQuadratic());
			}
		}
	}

	void Scene::passWorldMatrices(const Ref<Shader>& shader, const Eigen::Affine3f& transformation)
	{
		shader->setMat4("projection_matrix", m_cam->computeProjectionMatrix());
		shader->setMat4("view_matrix", m_cam->computeViewMatrix());
		shader->setMat4("model_matrix", transformation.matrix());
		shader->setVec3("camera_position", m_cam->getPosition());
	}

	void Scene::passTextures(const Ref<Shader>& shader, const Ref<Material>& material)
	{
		glBindTextureUnit(0, m_cubemap->getIrradianceMap());
		shader->setInt("irradiance_map", 0);

		glBindTextureUnit(1, m_cubemap->getPreFilterMap());
		shader->setInt("prefilter_map", 1);

		glBindTextureUnit(2, m_cubemap->getBRDFTexture());
		shader->setInt("brdf_map", 2);

		int index = 3;
		passPBRMaterial(material, shader, index);
	}

	void Scene::passPBRMaterial(const Ref<Material>& mat, const Ref<Shader>& shader, int& index)
	{
		// ALBEDO
		if (mat->useBasecolorMap)
		{
			mat->basecolorMap->bind(index);
			shader->setInt("albedo_map", index);
			index++;
		}
		else
			shader->setVec3("albedo_val", mat->basecolor);
		shader->setInt("albedo_bool", mat->useBasecolorMap);

		// METALLIC
		if (mat->useMetallicMap)
		{
			mat->metallicMap->bind(index);
			shader->setInt("metallic_map", index);
			index++;
		}
		else
			shader->setFloat("metallic_val", mat->metallic);
		shader->setInt("metallic_bool", mat->useMetallicMap);

		// ROUGHNESS
		if (mat->useRoughnessMap)
		{
			mat->roughnessMap->bind(index);
			shader->setInt("roughness_map", index);
			index++;
		}
		else
			shader->setFloat("roughness_val", mat->roughness);
		shader->setInt("roughness_bool", mat->useRoughnessMap);

		// AO
		if (mat->useAoMap)
		{
			mat->aoMap->bind(index);
			shader->setInt("ao_map", index);
			index++;
		}
		else
			shader->setFloat("ao_val", mat->ao);
		shader->setInt("ao_bool", mat->useAoMap);

		// NORMALS
		if (mat->useNormalMap)
		{
			mat->normalMap->bind(index);
			shader->setInt("normal_map", index);
			index++;
		}
		shader->setInt("normal_bool", mat->useNormalMap);
	}

	void Scene::renderEntity(const Ref<Entity>& entity, const Ref<Shader>& shader, bool isPBR)
	{
		shader->bind();
		passLights(shader);
		shader->setFloat("exposure", m_cubemap->getExposure());
		shader->setFloat("lod", m_cubemap->getLod());
		shader->setInt("max_mip_level", m_cubemap->getMaxMipLevels());
		passWorldMatrices(shader, entity->getTransformationMatrix());
		passTextures(shader, MATERIAL_LIB->getMaterial(entity->getMaterial()));
		entity->display(shader);
		shader->unbind();
	}

	void Scene::setCubemap(const std::string& filepath)
	{
		m_cubemap = createRef<CubeMap>(filepath);
	}

	void Scene::onEvent(Event& e)
	{
		if (!e.handled)
			m_cam->onEvent(e);
	}
}