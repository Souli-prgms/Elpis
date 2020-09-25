#pragma once

namespace Elpis
{
	class Texture
	{
	public:
		Texture(const std::string& filename);
		~Texture();

		unsigned int getId() { return m_id; }

	private:
		int m_width, m_height, m_nbChannels;
		unsigned char* m_data;
		float* m_floatData;
		unsigned int m_id;

		int loadImage(const std::string& filename);
		int loadHDR(const std::string& filename);
	};
}