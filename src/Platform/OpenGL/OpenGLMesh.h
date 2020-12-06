#pragma once

#include "Mesh.h"

namespace Elpis
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& faceIds, const Box3& bbox);
		virtual ~OpenGLMesh();

		virtual void display(const Ref<Shader>& shader) override;

	private:
		GLuint m_vao;
		GLuint m_vbo[2];

		virtual void init() override;
	};
}