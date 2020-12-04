#include "Platform/OpenGL/OpenGLShader.h"

namespace Elpis
{
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		parseShader(filepath);
		createShader();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_id);
	}

	void OpenGLShader::parseShader(const std::string& filepath)
	{
		std::ifstream stream(filepath);

		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;

		while (getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			}
			else ss[(int)type] << line << '\n';
		}

		m_vertexSource = ss[0].str();
		m_fragmentSource = ss[1].str();
	}

	uint32_t OpenGLShader::compileShader(uint32_t typeconst, const std::string& source) {
		uint32_t id = glCreateShader(typeconst);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) {
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			char* message = (char*)alloca(lenght * sizeof(char));
			glGetShaderInfoLog(id, lenght, &lenght, message);
			EL_CORE_ERROR("Failed to compile {0}: {1} {2}", typeconst == GL_VERTEX_SHADER ? "vertex" : "fragment", source, message);
			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	void OpenGLShader::createShader() {
		m_id = glCreateProgram();
		uint32_t vs = compileShader(GL_VERTEX_SHADER, m_vertexSource);
		uint32_t fs = compileShader(GL_FRAGMENT_SHADER, m_fragmentSource);

		glAttachShader(m_id, vs);
		glAttachShader(m_id, fs);
		glLinkProgram(m_id);
		glValidateProgram(m_id);

		glDeleteShader(fs);
		glDeleteShader(vs);
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(m_id);
	}

	void OpenGLShader::unbind() const
	{
		glUseProgram(0);
	}

	int OpenGLShader::getAttribLocation(const std::string& attrib) const
	{
		return glGetAttribLocation(m_id, attrib.c_str());
	}

	void OpenGLShader::setVec3(const std::string& name, const Vec3& value)
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), value(0), value(1), value(2));
	}

	void OpenGLShader::setVec4(const std::string& name, const Vec4& value)
	{
		glUniform4f(glGetUniformLocation(m_id, name.c_str()), value(0), value(1), value(2), value(3));
	}

	void OpenGLShader::setFloat(const std::string& name, float f)
	{
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), f);
	}

	void OpenGLShader::setMat3(const std::string& name, const Mat3& mat)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, mat.data());
	}

	void OpenGLShader::setMat4(const std::string& name, const Mat4& mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, mat.data());
	}

	void OpenGLShader::setInt(const std::string& name, int i)
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), i);
	}
}