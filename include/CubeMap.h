#pragma once

#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"

class CubeMap
{
public:
	CubeMap();
	~CubeMap();

	void render(Camera* cam);
	unsigned int getEnvironmentMap() const { return m_envCubemap; }
	unsigned int getIrradianceMap() const { return m_irradianceMap; }
	unsigned int getPreFilterMap() const { return m_preFilterMap; }
	unsigned int getBRDFTexture() const { return m_brdfTexture; }

private:
	Mesh* m_cube;
	Mesh* m_quad;
	Texture* m_texture;
	
	Shader* m_cubemapShader;
	Shader* m_backgroundShader;
	Shader* m_irradianceShader;
	Shader* m_preFilterShader;
	Shader* m_brdfShader;
	
	unsigned int m_envCubemap;
	unsigned int m_irradianceMap;
	unsigned int m_preFilterMap;
	unsigned int m_brdfTexture;

	unsigned int m_captureFBO;
	unsigned int m_captureRBO;

	void load();
	void computeIrradianceMap();
	void computepreFilteredMap();
	void computeBRDF();
};
