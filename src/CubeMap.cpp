#include "CubeMap.h"

Eigen::Matrix4f captureProjection = Camera::perspective(M_PI / 2, 1.0f, 0.1f, 10.0f);
Eigen::Matrix4f captureViews[] =
{
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(1.0f,  0.0f,  0.0f), Eigen::Vector3f(0.0f, -1.0f,  0.0f)),
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(-1.0f,  0.0f,  0.0f), Eigen::Vector3f(0.0f, -1.0f,  0.0f)),
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f,  1.0f,  0.0f), Eigen::Vector3f(0.0f,  0.0f,  1.0f)),
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, -1.0f,  0.0f), Eigen::Vector3f(0.0f,  0.0f, -1.0f)),
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f,  0.0f,  1.0f), Eigen::Vector3f(0.0f, -1.0f,  0.0f)),
   Camera::lookAt(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f,  0.0f, -1.0f), Eigen::Vector3f(0.0f, -1.0f,  0.0f))
};

CubeMap::CubeMap()
{
	m_texture = new Texture("../../res/cubemaps/MonValley_DirtRoad/MonValley_G_DirtRoad_3k.hdr");
	m_cubemapShader = new Shader("../../res/shaders/Cubemap.shader");
	m_backgroundShader = new Shader("../../res/shaders/Background.shader");
	m_irradianceShader = new Shader("../../res/shaders/Irradiance.shader");
	m_cube = Mesh::createMesh("../../res/models/cube.obj");

	load();
	computeIrradianceMap();
}

CubeMap::~CubeMap()
{
}

void CubeMap::render(Camera* cam)
{
	m_backgroundShader->bind();
	m_backgroundShader->setMat4("view_matrix", cam->computeViewMatrix());
	m_backgroundShader->setMat4("projection_matrix", cam->computeProjectionMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	m_cube->display(m_backgroundShader);
	m_backgroundShader->unbind();
}

void CubeMap::load()
{
	int bufferSize = 1024;

	glGenFramebuffers(1, &m_captureFBO);
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferSize, bufferSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);

	glGenTextures(1, &m_envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, bufferSize, bufferSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// convert HDR equirectangular environment map to cubemap equivalent
	m_cubemapShader->bind();
	m_cubemapShader->setInt("equirectangular_map", 0);
	m_cubemapShader->setMat4("projection_matrix", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getId());

	glViewport(0, 0, bufferSize, bufferSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_cubemapShader->setMat4("view_matrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cube->display(m_cubemapShader);
	}
	m_cubemapShader->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::computeIrradianceMap()
{
	int bufferSize = 32;

	glGenTextures(1, &m_irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, bufferSize, bufferSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferSize, bufferSize);

	m_irradianceShader->bind();
	m_irradianceShader->setInt("environment_map", 0);
	m_irradianceShader->setMat4("projection_matrix", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_irradianceShader->setMat4("view_matrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cube->display(m_irradianceShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::computepreFilteredMap()
{
	/*
	glGenTextures(1, &m_preFilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_preFilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	prefilterShader.use();
	prefilterShader.setInt("environmentMap", 0);
	prefilterShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void CubeMap::computeBRDF()
{
	/*
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}