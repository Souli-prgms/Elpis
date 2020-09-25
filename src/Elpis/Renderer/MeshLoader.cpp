#include "MeshLoader.h"

namespace Elpis
{
	void MeshLoader::loadMesh(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Box3& bbox)
	{
		vertices.clear(); indices.clear();

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate/* | aiProcess_FlipUVs*/);
		aiMesh* mesh = scene->mMeshes[0];
		bool hasUVs = mesh->mTextureCoords[0] ? true : false;
		for (int v = 0; v < mesh->mNumVertices; v++)
		{
			Vec3 position(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
			Vec2 texcoords = hasUVs ? Vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y) : Vec2(0.0, 0.0);
			Vec3 normal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);

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
}