#pragma once

namespace Elpis
{
	struct Vertex
	{
		Vertex() {}
		Vertex(const Vec3& pos, const Vec3& n, const Vec2& tex) : m_position(pos), m_normal(n), m_texcoord(tex)
		{}

		Vec3 m_position;
		Vec3 m_normal;
		Vec2 m_texcoord;
	};

	namespace MeshLoader {
		void loadMesh(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, Box3& bbox);
	}
}