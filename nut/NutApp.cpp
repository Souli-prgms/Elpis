#pragma once

#include "App.h"
#include "NutLayer.h"

namespace Elpis
{
	class NutApp : public App
	{
	public:
		NutApp() : App("Elpis")
		{
			pushLayer(new NutLayer());
		}

		~NutApp()
		{
		}
	};

	App* createApp()
	{
		return new NutApp();
	}
}