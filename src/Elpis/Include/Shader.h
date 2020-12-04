#pragma once

namespace Elpis
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setInt(const std::string& name, int value) = 0;
		virtual void setFloat(const std::string& name, float value) = 0;
		virtual void setVec3(const std::string& name, const Vec3& value) = 0;
		virtual void setVec4(const std::string& name, const Vec4& value) = 0;
		virtual void setMat3(const std::string& name, const Mat3& value) = 0;
		virtual void setMat4(const std::string& name, const Mat4& value) = 0;

		virtual int getAttribLocation(const std::string& attrib) const = 0;

		static Ref<Shader> create(const std::string& filepath);
	};

	class ShaderLibrary
	{
	public:
		void add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& name, const std::string& filepath);
		Ref<Shader> get(const std::string& name);
		bool exists(const std::string& name) const;
		static ShaderLibrary* getInstance();

	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaders;
		static ShaderLibrary* s_instance;
	};

#define SHADER_LIB ShaderLibrary::getInstance()
}