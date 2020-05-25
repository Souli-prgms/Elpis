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

void MeshLoader::loadMesh(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Eigen::AlignedBox3f& bbox)
{
	vertices.clear(); indices.clear();

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate/* | aiProcess_FlipUVs*/);
	aiMesh* mesh = scene->mMeshes[0];
	bool hasUVs = mesh->mTextureCoords[0] ? true : false;
	for (int v = 0; v < mesh->mNumVertices; v++)
	{
		Eigen::Vector3f position(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
		Eigen::Vector2f texcoords = hasUVs ? Eigen::Vector2f(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y) : Eigen::Vector2f(0.0, 0.0);
		Eigen::Vector3f normal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
		
		vertices.push_back(Vertex(position, normal, texcoords));
		bbox.extend(position);
	}

	for (int f = 0; f < mesh->mNumFaces; f++)
	{
		aiFace face = mesh->mFaces[f];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}