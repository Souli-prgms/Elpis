#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Eigen>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	Vertex() {}
	Vertex(const Eigen::Vector3f& pos, const Eigen::Vector3f& n, const Eigen::Vector2f& tex) : m_position(pos), m_normal(n), m_texcoord(tex)
	{}

	Eigen::Vector3f m_position;
	Eigen::Vector3f m_normal;
	Eigen::Vector2f m_texcoord;
};

namespace MeshLoader {
	void loadObj(const std::string& filepath, std::vector<Eigen::Vector3f>& positions, std::vector<Eigen::Vector3i>& faceIds, std::vector<Eigen::Vector3f>& normals, std::vector<Eigen::Vector2f>& texcoords, std::vector<Eigen::Vector3i> normalIds, std::vector<Eigen::Vector3i> texIds);
	void loadMesh(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Eigen::AlignedBox3f& bbox);
}