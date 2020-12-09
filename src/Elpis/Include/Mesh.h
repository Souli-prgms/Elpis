#pragma once

#include "MeshLoader.h"
#include "Shader.h"
#include "RendererAPI.h"

namespace Elpis
{
	class Mesh {
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& faceIds, const Box3& bbox);
		~Mesh();

		static Ref<Mesh> createMesh(const std::string& filepath);
		static Ref<Mesh> createSphere(const float radius, const int nU, const int nV);
		static Ref<Mesh> createQuad();

		virtual void display(const Ref<Shader>& shader) = 0;
		Box3 boundingBox() const { return m_bbox; }

	protected:
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_faceIds;

		bool m_ready;

		virtual void init() = 0;

	private:
		Box3 m_bbox;

		static Ref<Mesh> create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& faceIds, const Box3& bbox);
	};
}