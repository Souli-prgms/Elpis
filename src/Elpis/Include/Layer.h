#pragma once

#include "Event.h"
#include "Timestep.h"

namespace Elpis
{
	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}

		virtual void onUpdate(Timestep ts) {}
		virtual void onInterfaceRender() {}
		virtual void onEvent(Event& e) {}
	};
}