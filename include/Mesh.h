#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>

#include "MeshLoader.h"
#include "Shader.h"

class Mesh {
public:
	static Mesh* createMesh(const std::string& filepath);
	static Mesh* createSphere(const float radius, const int nU, const int nV);
	static Mesh* createQuad();

	void display(Shader* shader);
	Eigen::AlignedBox3f boundingBox() const { return m_bbox; }

private:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_faceIds;

	Eigen::AlignedBox3f m_bbox;

	GLuint m_vao;
	GLuint m_vbo[2];

	bool m_ready;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& faceIds, const Eigen::AlignedBox3f& bbox);
	~Mesh();
	void init();
};