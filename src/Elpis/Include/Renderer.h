#pragma once

#include "Scene.h"
#include "Window.h"
#include "Event.h"
#include "ApplicationEvent.h"
#include "FileDialog.h"
#include "Framebuffer.h"
#include "InterfaceLayer.h"

namespace Elpis 
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void run();
		void onEvent(Event& e);

		Scene& getScene() { return *m_scene; }
		Window& getWindow() { return *m_window; }
		Framebuffer& getFramebuffer() { return *m_framebuffer; }

		static Renderer* getInstance();

	private:
		Scope<Window> m_window;
		Ref<Scene> m_scene;
		Ref<Framebuffer> m_framebuffer;
		Ref<InterfaceLayer> m_interface;

		static Renderer* s_instance;
	};

#define RENDERER Renderer::getInstance()
}