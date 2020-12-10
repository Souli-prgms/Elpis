#pragma once

#include "Event.h"
#include "Layer.h"
#include "Window.h"
#include "InterfaceLayer.h"
#include "ApplicationEvent.h"
#include "RendererAPI.h"

int main(int argc, char** argv);

namespace Elpis
{
	class App
	{
	public:
		App(const std::string& name = "Elpis App");
		virtual ~App();

		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		Window& getWindow() { return *m_window; }

		void close();

		InterfaceLayer* getInterfaceLayer() { return m_interfaceLayer; }
		static App& getInstance() { return *s_instance; }

	private:
		void run();
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

	private:
		Scope<Window> m_window;
		InterfaceLayer* m_interfaceLayer;
		bool m_running = true;
		bool m_minimized = false;
		std::vector<Layer*> m_layerStack;
		float m_lastFrameTime = 0.0f;

	private:
		static App* s_instance;
		friend int ::main(int argc, char** argv);
	};

	App* createApp();

#define APP App::getInstance()
}