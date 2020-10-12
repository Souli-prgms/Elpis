#include "OpenGLContext.h"

namespace Elpis {
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

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle): m_windowHandle(windowHandle)
	{
	}

	void OpenGLContext::init()
	{
		glfwMakeContextCurrent(m_windowHandle);

		if (glewInit() != GLEW_OK)
			EL_CORE_ERROR("Failed to load initialize GLEW");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

		EL_CORE_INFO("OpenGL Info:");
		EL_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		EL_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		EL_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::swapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}
