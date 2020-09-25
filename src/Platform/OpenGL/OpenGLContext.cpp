#include "OpenGLContext.h"

namespace Elpis {

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
