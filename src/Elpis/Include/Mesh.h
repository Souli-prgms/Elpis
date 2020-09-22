#pragma once

#include "MeshLoader.h"
#include "Shader.h"

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& faceIds, const Box3& bbox);
	~Mesh();

	static Ref<Mesh> createMesh(const std::string& filepath);
	static Ref<Mesh> createSphere(const float radius, const int nU, const int nV);
	static Ref<Mesh> createQuad();

	void display(const Ref<Shader>& shader);
	Box3 boundingBox() const { return m_bbox; }

private:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_faceIds;

	Box3 m_bbox;

	GLuint m_vao;
	GLuint m_vbo[2];

	bool m_ready;

	
	void init();
};