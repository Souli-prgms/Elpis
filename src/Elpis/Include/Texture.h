#pragma once

#include "RendererAPI.h"

namespace Elpis
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getRendererID() const = 0;

		virtual void setData(void* data, uint32_t size) = 0;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		static Ref<Texture> create(uint32_t width, uint32_t height);
		static Ref<Texture> create(const std::string& path);
	};
}