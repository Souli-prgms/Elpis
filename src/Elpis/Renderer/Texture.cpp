#include "Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Elpis
{
	Ref<Texture> Texture::create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:    EL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return createRef<OpenGLTexture>(width, height);
		}

		EL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::create(const std::string& path)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:    EL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return createRef<OpenGLTexture>(path);
		}

		EL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}