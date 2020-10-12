#pragma once

#include "Event.h"

namespace Elpis {
	class WindowResizeEvent: public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height): m_width(width), m_height(height) {}

		uint32_t getWidth() const { return m_width; }
		uint32_t getHeight() const { return m_height; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Application)

	private:
		uint32_t m_width, m_height;
	};

	class WindowCloseEvent: public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};

	class AppTickEvent: public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};

	class AppUpdateEvent: public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};

	class AppRenderEvent: public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};
}