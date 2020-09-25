#include "GraphicsContext.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Elpis {

	Scope<GraphicsContext> GraphicsContext::create(void* window)
	{
		return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
	}
}