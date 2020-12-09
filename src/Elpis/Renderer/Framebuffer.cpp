#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Elpis
{
	Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None: EL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLFramebuffer>(spec);
		}

		EL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

