#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Elpis
{
	static void OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char*  message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:			EL_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:			EL_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:				EL_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION:	EL_CORE_TRACE(message); return;
		}
	}

	void OpenGLRendererAPI::init()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const Vec4& color)
	{
		glClearColor(color.x(), color.y(), color.z(), color.w());
	}

	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}