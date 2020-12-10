#pragma once

#include "App.h"
#include "Framebuffer.h"
#include "Scene.h"
#include "FileDialog.h"

namespace Elpis
{
	class NutLayer : public Layer
	{
	public:
		NutLayer();
		virtual ~NutLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		void onInterfaceRender() override;
		void onEvent(Event& e) override;

	private:
		Ref<Framebuffer> m_framebuffer;
		Ref<Scene> m_scene;
	};
}