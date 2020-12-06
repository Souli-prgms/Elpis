#include "Mesh.h"
#include "Platform/OpenGL/OpenGLMesh.h"

namespace Elpis
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& faceIds, const Box3& bbox) : m_ready(false), m_vertices(vertices), m_faceIds(faceIds), m_bbox(bbox)
	{
	}

	Mesh::~Mesh()
	{

	}

	Ref<Mesh> Mesh::createMesh(const std::string& filepath)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Box3 bbox;
		MeshLoader::loadMesh(filepath, vertices, indices, bbox);
		return createRef<OpenGLMesh>(vertices, indices, bbox);
	}

	Ref<Mesh> Mesh::createSphere(const float radius, const int nU, const int nV)
	{
		int nVertices = (nU + 1) * (nV + 1);
		std::vector<Vertex> vertices(nVertices);
		std::vector<uint32_t> faceIds;
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
		return createRef<OpenGLMesh>(vertices, faceIds, bbox);
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
		std::vector<uint32_t> faceIds = { 1, 0, 3, 1, 3, 2 };
		return createRef<OpenGLMesh>(vertices, faceIds, bbox);
	}
}