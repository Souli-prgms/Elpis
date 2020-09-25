#include "Mesh.h"

namespace Elpis
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& faceIds, const Box3& bbox) : m_ready(false), m_vertices(vertices), m_faceIds(faceIds), m_bbox(bbox)
	{
	}

	Mesh::~Mesh()
	{

	}

	Ref<Mesh> Mesh::createMesh(const std::string& filepath)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Box3 bbox;
		MeshLoader::loadMesh(filepath, vertices, indices, bbox);
		return createRef<Mesh>(vertices, indices, bbox);
	}

	Ref<Mesh> Mesh::createSphere(const float radius, const int nU, const int nV)
	{
		int nVertices = (nU + 1) * (nV + 1);
		std::vector<Vertex> vertices(nVertices);
		std::vector<unsigned int> faceIds;
		Box3 bbox;

		for (int v = 0; v <= nV; v++)
		{
			for (int u = 0; u <= nU; u++)
			{
				float theta = u / float(nU) * M_PI;
				float phi = v / float(nV) * M_PI * 2;

				int index = u + (nU + 1) * v;

				Vec3 normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
				normal.normalize();

				Vec3 pos = normal * radius;
				bbox.extend(pos);

				vertices[index] = Vertex(pos, normal, Vec2(v / float(nV), u / float(nU)));
			}
		}

		for (int v = 0; v < nV; ++v)
		{
			for (int u = 0; u < nU; ++u)
			{
				int vindex = u + (nU + 1) * v;
				faceIds.push_back(vindex); faceIds.push_back(vindex + 1); faceIds.push_back(vindex + 1 + (nU + 1));
				faceIds.push_back(vindex); faceIds.push_back(vindex + 1 + (nU + 1)); faceIds.push_back(vindex + (nU + 1));
			}
		}
		return createRef<Mesh>(vertices, faceIds, bbox);
	}

	Ref<Mesh> Mesh::createQuad()
	{
		Vec3 n(0.0, 0.0, 1.0);
		std::vector<Vertex> vertices = { Vertex(Vec3(-1.0f, 1.0f, 0.0f), n, Vec2(0.0f, 1.0f)),
		Vertex(Vec3(-1.0f, -1.0f, 0.0f), n, Vec2(0.0f, 0.0f)),
		Vertex(Vec3(1.0f, 1.0f, 0.0f), n, Vec2(1.0f, 1.0f)),
		Vertex(Vec3(1.0f, -1.0f, 0.0f), n, Vec2(1.0f, 0.0f)) };
		Box3 bbox;
		bbox.extend(Vec3(-1.0f, -1.0f, 0.0f)); bbox.extend(Vec3(1.0f, 1.0f, 0.0f));
		std::vector<unsigned int> faceIds = { 1, 0, 3, 1, 3, 2 };
		return createRef<Mesh>(vertices, faceIds, bbox);
	}

	void Mesh::init() {
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

	void Mesh::display(const Ref<Shader>& shader) {
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