#pragma once

#include "Shader.h"

namespace Elpis
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setInt(const std::string& name, int value) override;
		virtual void setFloat(const std::string& name, float value) override;
		virtual void setVec3(const std::string& name, const Vec3& value) override;
		virtual void setVec4(const std::string& name, const Vec4& value) override;
		virtual void setMat3(const std::string& name, const Mat3& value) override;
		virtual void setMat4(const std::string& name, const Mat4& value) override;

		virtual int getAttribLocation(const std::string& attrib) const override;

	private:
		void parseShader(const std::string& filepath);
		uint32_t compileShader(uint32_t typeconst, const std::string& source);
		void createShader();

		std::string m_vertexSource;
		std::string m_fragmentSource;
		uint32_t m_id;
	};
}