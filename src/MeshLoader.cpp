#include "MeshLoader.h"

void MeshLoader::loadObj(const std::string& filepath, std::vector<Eigen::Vector3f>& positions, std::vector<Eigen::Vector3i>& faceIds, std::vector<Eigen::Vector3f>& normals, std::vector<Eigen::Vector2f>& texcoords, std::vector<Eigen::Vector3i> normalIds, std::vector<Eigen::Vector3i> texIds)
{
	std::ifstream input(filepath.c_str());

	if (!input)
		std::cout << "MeshLoader::loadObj cannot load: " + filepath << std::endl;

	char line[1024];

	while (!input.eof()) {

		if (!input.getline(line, 1024, '\n').good())
			continue;

		std::istringstream line_input(line);
		std::string keyword;

		line_input >> keyword;

		if (keyword == "v") { // vertex position
			Eigen::Vector3f p;
			line_input >> p[0] >> p[1] >> p[2];
			positions.push_back(p);
		}

		else if (keyword == "vt") { // vertex texture coordinate
			Eigen::Vector2f p;
			line_input >> p[0] >> p[1];
			texcoords.push_back(p);
		}
		else if (keyword == "vn") { // vertex normal
			Eigen::Vector3f n;
			line_input >> n[0] >> n[1] >> n[2];
			normals.push_back(n);
		}
		else if (keyword == "f") { // face
			// vertex_id / vertex texcoord / vertex normal
			Eigen::Vector3i fId;
			Eigen::Vector3i tId;
			Eigen::Vector3i nId;

			if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&fId[0], &tId[0], &nId[0],
				&fId[1], &tId[1], &nId[1],
				&fId[2], &tId[2], &nId[2]) == 9) {
				for (int i = 0; i < 3; ++i) {
					fId[i] -= 1;
					tId[i] -= 1;
					nId[i] -= 1;
				}
				faceIds.push_back(fId);
				texIds.push_back(tId);
				normalIds.push_back(nId);

			}
			else if (sscanf(line, "f %d//%d %d//%d %d//%d",
				&fId[0], &nId[0],
				&fId[1], &nId[1],
				&fId[2], &nId[2]) == 6) {
				for (int i = 0; i < 3; ++i) {
					fId[i] -= 1;
					nId[i] -= 1;
				}
				faceIds.push_back(fId);
				normalIds.push_back(nId);

			}
			else if (sscanf(line, "f %d// %d// %d//",
				&fId[0],
				&fId[1],
				&fId[2]) == 3) {
				for (int i = 0; i < 3; ++i) {
					fId[i] -= 1;
				}
				faceIds.push_back(fId);
			}
			else if (sscanf(line, "f %d %d %d",
				&fId[0],
				&fId[1],
				&fId[2]) == 3) {
				for (int i = 0; i < 3; ++i) {
					fId[i] -= 1;
				}
				faceIds.push_back(fId);
			}
			else { std::cerr << "error reading line -> \"" << line << "\"" << std::endl; }
		}
	}

	input.close();
}