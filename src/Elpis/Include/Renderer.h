#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Scene.h"

namespace Elpis 
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void run();

		void charPressed(int key);
		void keyPressed(int key, int action, int mods);
		void mouseScrolled(double x, double y);
		void mousePressed(GLFWwindow* window, int button, int action);
		void mouseMoved(double x, double y);
		void reshapeWindow(int width, int height);

	private:
		void initWindow();
		ImGuiWindowFlags initInterface();
		void setInterface(const ImGuiWindowFlags& windowFlags);
		void setCallbacks();
		std::string fileDialog();

		GLFWwindow* m_window;
		Ref<Scene> m_scene;

		int m_button = -1;
		Vec2 m_lastMousePos;

		int m_width;
		int m_height;
	};
}