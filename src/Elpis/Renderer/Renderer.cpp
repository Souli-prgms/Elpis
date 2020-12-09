#include "Renderer.h"

namespace Elpis
{
	Renderer* Renderer::s_instance = 0;

	Renderer* Renderer::getInstance()
	{
		if (!s_instance)
			s_instance = new Renderer;
		return s_instance;
	}

	Renderer::Renderer()
	{
		m_window = Window::create(WindowProperties("Elpis Engine"));
		m_window->setEventCallback(EL_BIND_EVENT_FN(Renderer::onEvent));

		m_scene = createRef<Scene>();

		// Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.width = m_window->getWidth();
		fbSpec.height = m_window->getHeight();
		m_framebuffer = Framebuffer::create(fbSpec);

		// Interface
		m_interface = createRef<InterfaceLayer>();
	}

	Renderer::~Renderer()
	{
		glfwTerminate();
	}

	void Renderer::run()
	{
		m_interface->onAttach();
		glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
		while (!glfwWindowShouldClose((GLFWwindow*)m_window->getNativeWindow()))
		{
			m_framebuffer->bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_scene->render();
			m_framebuffer->unbind();

			m_interface->begin();
			m_interface->set();
			m_interface->end();

			glfwSwapBuffers((GLFWwindow*)m_window->getNativeWindow());
			glfwPollEvents();
		}
		glfwMakeContextCurrent((GLFWwindow*)m_window->getNativeWindow());
	}

	void Renderer::onEvent(Event& e)
	{
		m_interface->onEvent(e);
		m_scene->onEvent(e);
	}
}