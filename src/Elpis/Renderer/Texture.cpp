#include "Texture.h"

namespace Elpis
{
	Texture::Texture(const std::string& filename)
	{
		int res = std::filesystem::path(filename).extension() == ".hdr" ? loadHDR(filename) : loadImage(filename);
		if (res != EL_OK)	EL_CORE_ERROR("Failed to load texture: {0}", filename);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	int Texture::loadImage(const std::string& filename)
	{
		int res = EL_KO;
		m_data = stbi_load(filename.c_str(), &m_width, &m_height, &m_nbChannels, 0);

		if (m_data)
		{
			glGenTextures(1, &m_id);
			GLenum format;

			if (m_nbChannels == 1)
				format = GL_RED;
			else if (m_nbChannels == 3)
				format = GL_RGB;
			else if (m_nbChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, m_id);

			glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, m_data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			res = EL_OK;
		}	

		stbi_image_free(m_data);
		return res;
	}
	int Texture::loadHDR(const std::string& filename)
	{
		int res = EL_KO;
		stbi_set_flip_vertically_on_load(true);
		m_floatData = stbi_loadf(filename.c_str(), &m_width, &m_height, &m_nbChannels, 0);

		if (m_floatData)
		{
			glGenTextures(1, &m_id);

			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, m_floatData);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			res = EL_OK;
		}

		stbi_image_free(m_floatData);
		return res;
	}
}