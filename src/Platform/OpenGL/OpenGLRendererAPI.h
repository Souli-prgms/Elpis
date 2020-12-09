#pragma once

#include "RendererAPI.h"

namespace Elpis
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void setClearColor(const Vec4& color) override;
		virtual void clear() override;
	};
}