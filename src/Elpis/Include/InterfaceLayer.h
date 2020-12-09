#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Layer.h"

namespace Elpis
{
	class InterfaceLayer : public Layer
	{
	public:
		InterfaceLayer() {}
		~InterfaceLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onEvent(Event& e) override;

		void begin();
		void set();
		void end();

		void blockEvents(bool block) { m_blockEvents = block; }

	private:
		void setColors();

		bool m_blockEvents = true;
		float m_time = 0.0f;
	};
}