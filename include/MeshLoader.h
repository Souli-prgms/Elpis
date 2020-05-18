#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Eigen>

namespace MeshLoader {
	void loadObj(const std::string& filepath, std::vector<Eigen::Vector3f>& positions, std::vector<Eigen::Vector3i>& faceIds, std::vector<Eigen::Vector3f>& normals, std::vector<Eigen::Vector2f>& texcoords, std::vector<Eigen::Vector3i> normalIds, std::vector<Eigen::Vector3i> texIds);
}