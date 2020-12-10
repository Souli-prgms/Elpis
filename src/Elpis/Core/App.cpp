#include "App.h"

namespace Elpis
{
	App* App::s_instance = 0;

	App::App(const std::string& name)
	{
		EL_CORE_ASSERT(!s_instance, "App already exists!");
		s_instance = this;
		m_window = Window::create(WindowProperties(name));
		m_window->setEventCallback(EL_BIND_EVENT_FN(App::onEvent));

		RENDERER_API.init();

		m_interfaceLayer = new InterfaceLayer();
		pushOverlay(m_interfaceLayer);
	}

	App::~App()
	{

	}

	void App::pushLayer(Layer* layer)
	{
		m_layerStack.push_back(layer);
		layer->onAttach();
	}

	void App::pushOverlay(Layer* layer)
	{
		m_layerStack.emplace_back(layer);
		layer->onAttach();
	}

	void App::close()
	{
		m_running = false;
	}

	void App::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(EL_BIND_EVENT_FN(App::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(EL_BIND_EVENT_FN(App::onWindowResize));

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->onEvent(e);
		}
	}

	void App::run()
	{
		while (m_running)
		{
			float time = (float)glfwGetTime();
			Timestep ts = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized)
			{
				for (Layer* layer : m_layerStack)
					layer->onUpdate(ts);

				m_interfaceLayer->begin();
				
				for (Layer* layer : m_layerStack)
					layer->onInterfaceRender();

				m_interfaceLayer->end();
			}
			m_window->onUpdate();
		}
	}

	bool App::onWindowClose(WindowCloseEvent& e)
	{
		close();
		return true;
	}

	bool App::onWindowResize(WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		RENDERER_API.setViewport(0, 0, e.getWidth(), e.getHeight());

		return false;
	}
}