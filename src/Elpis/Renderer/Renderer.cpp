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
		RENDERER_API.init();
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
		RENDERER_API.setViewport(0, 0, m_window->getWidth(), m_window->getHeight());
		glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
		while (m_running)
		{
			m_framebuffer->bind();
			RENDERER_API.setClearColor(Vec4(0.1f, 0.1f, 0.1f, 0.1f));
			RENDERER_API.clear();

			m_scene->render();
			m_framebuffer->unbind();

			m_interface->begin();
			m_interface->set();
			m_interface->end();

			m_window->onUpdate();
		}
	}

	void Renderer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(EL_BIND_EVENT_FN(Renderer::onWindowClose));

		m_interface->onEvent(e);
		m_scene->onEvent(e);
	}

	bool Renderer::onWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

}