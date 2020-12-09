#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Elpis
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
	Scope<RendererAPI> RendererAPI::s_instance = 0;

	RendererAPI& RendererAPI::getInstance() 
	{
		if (!s_instance)
			s_instance = create();
		return *s_instance;
	}

	Scope<RendererAPI> RendererAPI::create()
	{
		switch (s_API)
		{
			case RendererAPI::API::None:    EL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return createScope<OpenGLRendererAPI>();
		}

		EL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}