#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Scene.h"
#include "Window.h"
#include "Event.h"
#include "ApplicationEvent.h"
#include "FileDialog.h"
#include "Framebuffer.h"

namespace Elpis 
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void run();
		void onEvent(Event& e);

	private:
		void initInterface(); //TO MOVE
		void setInterface();
		Scope<Window> m_window;
		Ref<Scene> m_scene;
		Ref<Framebuffer> m_framebuffer;
		bool m_blockEvents;
	};
}