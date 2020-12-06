#include "OpenGLMesh.h"

namespace Elpis
{
	OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& faceIds, const Box3& bbox) : Mesh(vertices, faceIds, bbox)
	{

	}

	OpenGLMesh::~OpenGLMesh()
	{

	}

	void OpenGLMesh::init()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(2, m_vbo);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_faceIds.size(), m_faceIds.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices[0].m_position.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		m_ready = true;
	}

	void OpenGLMesh::display(const Ref<Shader>& shader)
	{
		if (!m_ready)
			init();

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);

		std::size_t index = 0;

		// Positions
		int vertexLoc = shader->getAttribLocation("vtx_position");
		if (vertexLoc >= 0) {
			glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
			glEnableVertexAttribArray(vertexLoc);
		}

		index += sizeof(Vec3);

		// Normals
		int normalLoc = shader->getAttribLocation("vtx_normal");
		if (normalLoc >= 0) {
			glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
			glEnableVertexAttribArray(normalLoc);
		}

		index += sizeof(Vec3);

		// Texture coordinates
		int texcoordLoc = shader->getAttribLocation("vtx_texcoord");
		if (texcoordLoc >= 0) {
			glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
			glEnableVertexAttribArray(texcoordLoc);
		}

		glDrawElements(GL_TRIANGLES, m_faceIds.size(), GL_UNSIGNED_INT, 0);

		if (vertexLoc >= 0)
			glDisableVertexAttribArray(vertexLoc);
		if (normalLoc >= 0)
			glDisableVertexAttribArray(normalLoc);
		if (texcoordLoc >= 0)
			glDisableVertexAttribArray(texcoordLoc);

		glBindVertexArray(0);
	}
}