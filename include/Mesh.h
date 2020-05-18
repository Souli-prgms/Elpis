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

	void display(Shader* shader);

private:
	struct Vertex
	{
		Vertex() {}
		Vertex(const Eigen::Vector3f& pos, const Eigen::Vector3f& n, const Eigen::Vector2f& tex) : m_position(pos), m_normal(n), m_texcoord(tex)
		{}

		Eigen::Vector3f m_position;
		Eigen::Vector3f m_normal;
		Eigen::Vector2f m_texcoord;
	};

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_faceIds;

	GLuint m_vao;
	GLuint m_vbo[2];

	bool m_ready;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& faceIds);
	~Mesh();
	void init();
};