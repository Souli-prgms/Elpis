#include "OpenGLTexture.h"

namespace Elpis
{
	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height)
	{

	}

	OpenGLTexture::OpenGLTexture(const std::string& path)
	{
		int res = std::filesystem::path(path).extension() == ".hdr" ? loadHDR(path) : loadImage(path);
		if (res != EL_OK)	EL_CORE_ERROR("Failed to load texture: {0}", path);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_rendererID);
	}

	int OpenGLTexture::loadHDR(const std::string& path)
	{
		int res = EL_KO;
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		float* data = nullptr;
		data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_width = width;
			m_height = height;
			m_internalFormat = GL_RGB16F;
			m_dataFormat = GL_RGB;

			glGenTextures(1, &m_rendererID);

			glBindTexture(GL_TEXTURE_2D, m_rendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_dataFormat, GL_FLOAT, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			res = EL_OK;
		}

		stbi_image_free(data);
		return res;
	}

	int OpenGLTexture::loadImage(const std::string& path)
	{
		int res = EL_KO;
		int width, height, channels;
		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_width = width;
			m_height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_internalFormat = internalFormat;
			m_dataFormat = dataFormat;

			EL_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
			glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

			glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

			res = EL_OK;
		}

		stbi_image_free(data);
		return res;
	}

	void OpenGLTexture::setData(void* data, uint32_t size)
	{
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		EL_CORE_ASSERT(size == m_width * m_height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}


	void OpenGLTexture::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_rendererID);
	}
}