#include "Texture.h"

Texture::Texture(const std::string& filename)
{
	if (std::filesystem::path(filename).extension() == ".hdr")
		loadHDR(filename);
	else
		loadImage(filename);
}

Texture::~Texture() 
{
	glDeleteTextures(1, &m_id);
}

void Texture::loadImage(const std::string& filename)
{
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
	}

	else
		std::cout << "Failed to load texture: " << filename << std::endl;

	stbi_image_free(m_data);
}
void Texture::loadHDR(const std::string& filename)
{
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
	}

	else
		std::cout << "Failed to load HDR: " << filename << std::endl;

	stbi_image_free(m_floatData);
}