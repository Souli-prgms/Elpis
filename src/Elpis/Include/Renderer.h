#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Scene.h"
#include "Window.h"
#include "Event.h"
#include "ApplicationEvent.h"
#include "FileDialog.h"

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
		ImGuiWindowFlags initInterface(); //TO MOVE
		void setInterface(const ImGuiWindowFlags& windowFlags); // TO MOVE

		Scope<Window> m_window;
		Ref<Scene> m_scene;
	};
}