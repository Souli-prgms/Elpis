#pragma once

class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void bind();
	void unbind();

	int getAttribLocation(const char* name) const;

	void setFloat(const char* name, float f) const;
	void setInt(const char* name, unsigned int i) const;
	void setVec3(const char* name, const Vec3& vector) const;
	void setVec4(const char* name, const Vec4& vector) const;
	void setMat3(const char* name, const Mat3& mat) const;
	void setMat4(const char* name, const Mat4& mat) const;

private:
	void parseShader(const std::string& filepath);
	unsigned int compileShader(unsigned int typeconst, const std::string& source);
	void createShader();

	std::string m_vertexSource;
	std::string m_fragmentSource;
	unsigned int m_id;
};