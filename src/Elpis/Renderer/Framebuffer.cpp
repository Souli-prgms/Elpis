#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Elpis
{
	Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
	{
		return createRef<OpenGLFramebuffer>(spec);
	}
}

