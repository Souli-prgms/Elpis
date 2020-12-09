#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Elpis
{
	ShaderLibrary* ShaderLibrary::s_instance = 0;

	Ref<Shader> Shader::create(const std::string& filepath)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None: EL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShader>(filepath);
		}

		EL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
	{
		EL_CORE_ASSERT(!exists(name), "Shader already exists!");
		m_shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::create(filepath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name)
	{
		EL_CORE_ASSERT(exists(name), "Shader not found!");
		return m_shaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const
	{
		return m_shaders.find(name) != m_shaders.end();
	}

	ShaderLibrary* ShaderLibrary::getInstance()
	{
		if (!s_instance)
			s_instance = new ShaderLibrary;
		return s_instance;
	}
}