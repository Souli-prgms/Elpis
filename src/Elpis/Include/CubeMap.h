#pragma once

#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"

namespace Elpis 
{
	class CubeMap
	{
	public:
		CubeMap(const std::string& filepath);
		~CubeMap();

		void render(const Ref<Camera>& cam);
		uint32_t getEnvironmentMap() const { return m_envCubemap; }
		uint32_t getIrradianceMap() const { return m_irradianceMap; }
		uint32_t getPreFilterMap() const { return m_preFilterMap; }
		uint32_t getBRDFTexture() const { return m_brdfTexture; }

		int getMaxMipLevels() const { return m_maxMipLevel; }
		float& getExposure() { return m_exposure; }
		float& getLod() { return m_lod; }

		void setExposure(const float exposure) { m_exposure = exposure; }
		void setLod(const float lod) { m_exposure = lod; }

	private:
		Ref<Mesh> m_cube;
		Ref<Mesh> m_quad;
		Scope<Texture> m_texture;

		Ref<Shader> m_cubemapShader;
		Ref<Shader> m_backgroundShader;
		Ref<Shader> m_irradianceShader;
		Ref<Shader> m_preFilterShader;
		Ref<Shader> m_brdfShader;

		uint32_t m_envCubemap;
		uint32_t m_irradianceMap;
		uint32_t m_preFilterMap;
		uint32_t m_brdfTexture;

		uint32_t m_captureFBO;
		uint32_t m_captureRBO;

		int m_maxMipLevel;
		float m_exposure;
		float m_lod;

		void load();
		void computeIrradianceMap();
		void computepreFilteredMap();
		void computeBRDF();
	};
}