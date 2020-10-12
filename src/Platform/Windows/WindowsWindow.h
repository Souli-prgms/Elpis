#pragma once

#include "Window.h"
#include "ApplicationEvent.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Elpis
{
	class WindowsWindow: public Window
	{
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		uint32_t getWidth() const override { return m_data.width; }
		uint32_t getHeight() const override { return m_data.height; }

		void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		virtual void* getNativeWindow() const { return m_window; }
	private:
		virtual void init(const WindowProperties& props);
		virtual void shutdown();
	private:
		GLFWwindow* m_window;
		Scope<GraphicsContext> m_context;

		struct WindowData
		{
			std::string title;
			uint32_t width, height;
			bool vSync;

			EventCallbackFn eventCallback;
		};

		WindowData m_data;
	};

}