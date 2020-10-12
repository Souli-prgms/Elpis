#include "CubeMap.h"

namespace Elpis
{
	Mat4 captureProjection = Camera::perspective(M_PI / 2, 1.0f, 0.1f, 10.0f);
	Mat4 captureViews[] =
	{
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
	   Camera::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
	};

	CubeMap::CubeMap(const std::string& filepath) : m_exposure(1.0), m_lod(0.0), m_maxMipLevel(6)
	{
		m_texture = createScope<Texture>(filepath);
		m_cubemapShader = createRef<Shader>(EL_RESOURCE_PATH("shaders/Cubemap.shader"));
		m_backgroundShader = createRef<Shader>(EL_RESOURCE_PATH("shaders/Background.shader"));
		m_irradianceShader = createRef<Shader>(EL_RESOURCE_PATH("shaders/Irradiance.shader"));
		m_preFilterShader = createRef<Shader>(EL_RESOURCE_PATH("shaders/PreFilter.shader"));
		m_brdfShader = createRef<Shader>(EL_RESOURCE_PATH("shaders/BRDF.shader"));
		m_cube = Mesh::createMesh(EL_RESOURCE_PATH("models/cube.obj"));
		m_quad = Mesh::createQuad();

		load();
		computeIrradianceMap();
		computepreFilteredMap();
		computeBRDF();
	}

	CubeMap::~CubeMap()
	{
		glDeleteTextures(1, &m_envCubemap);
		glDeleteTextures(1, &m_irradianceMap);
		glDeleteTextures(1, &m_preFilterMap);
		glDeleteTextures(1, &m_brdfTexture);
	}

	void CubeMap::render(const Ref<Camera>& cam)
	{
		m_backgroundShader->bind();
		m_backgroundShader->setMat4("view_matrix", cam->computeViewMatrix());
		m_backgroundShader->setMat4("projection_matrix", cam->computeProjectionMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
		m_backgroundShader->setInt("environment_map", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_preFilterMap);
		m_backgroundShader->setInt("prefilter_map", 1);
		m_backgroundShader->setFloat("exposure", m_exposure);
		m_backgroundShader->setFloat("lod", m_lod);
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
		m_irradianceShader->unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CubeMap::computepreFilteredMap()
	{
		int size = 1024;
		glGenTextures(1, &m_preFilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_preFilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		m_preFilterShader->bind();
		m_preFilterShader->setInt("environment_map", 0);
		m_preFilterShader->setMat4("projection_matrix", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);

		for (unsigned int mip = 0; mip < m_maxMipLevel; ++mip)
		{
			unsigned int mipWidth = size * std::pow(0.5, mip);
			unsigned int mipHeight = size * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(m_maxMipLevel - 1);
			m_preFilterShader->setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				m_preFilterShader->setMat4("view_matrix", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_preFilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_cube->display(m_preFilterShader);
			}
		}
		m_preFilterShader->unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CubeMap::computeBRDF()
	{
		int size = 512;
		glGenTextures(1, &m_brdfTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, m_brdfTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfTexture, 0);

		glViewport(0, 0, size, size);
		m_brdfShader->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_quad->display(m_brdfShader);
		m_brdfShader->unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}