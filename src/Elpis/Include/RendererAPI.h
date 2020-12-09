#pragma once

#include "Core.h"

namespace Elpis
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

		virtual ~RendererAPI() = default;

		virtual void init() = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const Vec4& color) = 0;
		virtual void clear() = 0;

		static API getAPI() { return s_API; }
		static Scope<RendererAPI> create();

		static RendererAPI& getInstance();

	private:
		static API s_API;
		static Scope<RendererAPI> s_instance;
	};

#define RENDERER_API RendererAPI::getInstance()
}