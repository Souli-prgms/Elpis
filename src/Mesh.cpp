#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& faceIds): m_ready(false), m_vertices(vertices), m_faceIds(faceIds)
{
}

Mesh::~Mesh()
{

}

Mesh* Mesh::createMesh(const std::string& filepath)
{
	std::vector<Eigen::Vector3f> positionsTmp, normalsTmp;
	std::vector<Eigen::Vector2f> texcoordsTmp;
	std::vector<Eigen::Vector3i> faceIds, normalIds, texIds;

	MeshLoader::loadObj(filepath, positionsTmp, faceIds, normalsTmp, texcoordsTmp, normalIds, texIds);

	if (normalIds.size() == 0) // vertex normals not available, compute them from the faces normal
	{
		// pass 1: set the normal to 0
		normalsTmp.resize(positionsTmp.size(), Eigen::Vector3f(0.f, 0.f, 0.f));

		// pass 2: compute face normals and accumulate
		for (unsigned int i = 0; i < faceIds.size(); i++)
		{
			Eigen::Vector3f v0 = positionsTmp[(faceIds[i])[0]];
			Eigen::Vector3f v1 = positionsTmp[(faceIds[i])[1]];
			Eigen::Vector3f v2 = positionsTmp[(faceIds[i])[2]];

			Eigen::Vector3f n = (v1 - v0).cross(v2 - v0);

			normalsTmp[(faceIds[i])[0]] += n;
			normalsTmp[(faceIds[i])[1]] += n;
			normalsTmp[(faceIds[i])[2]] += n;
		}

		// pass 3: normalize
		for (std::vector<Eigen::Vector3f>::iterator n_iter = normalsTmp.begin(); n_iter != normalsTmp.end(); ++n_iter)
			(*n_iter).normalize();

		normalIds = faceIds;
	}

	Eigen::Vector3f pos;
	Eigen::Vector2f tex;
	Eigen::Vector3f norm;
	unsigned int size = faceIds.size();

	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;

	for (unsigned int i = 0; i < size; i++) 
	{
		for (unsigned int j = 0; j < 3; j++) {
			pos = positionsTmp[(faceIds[i])[j]];
			if (i < texIds.size())
				tex = texcoordsTmp[(texIds[i])[j]];
			if (i < normalIds.size())
				norm = normalsTmp[(normalIds[i])[j]];

			vertices.push_back(Vertex(pos, norm, tex));
			indices.push_back(3 * i + j);
		}
	}

	Mesh* mesh = new Mesh(vertices, indices);
	return mesh;
}

Mesh* Mesh::createSphere(const float radius, const int nU, const int nV)
{
	int nVertices = (nU + 1) * (nV + 1);
	std::vector<Vertex> vertices(nVertices);
	std::vector<unsigned int> faceIds;


	for (int v = 0; v <= nV; v++)
	{
		for (int u = 0; u <= nU; u++)
		{
			float theta = u / float(nU) * M_PI;
			float phi = v / float(nV) * M_PI * 2;

			int index = u + (nU + 1) * v;

			Eigen::Vector3f normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			normal.normalize();

			Eigen::Vector3f pos = normal * radius;

			vertices[index] = Vertex(pos, normal, Eigen::Vector2f(v / float(nV), u / float(nU)));
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

	Mesh* sphere = new Mesh(vertices, faceIds);
	return sphere;
}

Mesh* Mesh::createQuad()
{
	Eigen::Vector3f n(0.0, 0.0, 1.0);
	std::vector<Vertex> vertices = { Vertex(Eigen::Vector3f(-1.0f, 1.0f, 0.0f), n, Eigen::Vector2f(0.0f, 1.0f)),
	Vertex(Eigen::Vector3f(-1.0f, -1.0f, 0.0f), n, Eigen::Vector2f(0.0f, 0.0f)),
	Vertex(Eigen::Vector3f(1.0f, 1.0f, 0.0f), n, Eigen::Vector2f(1.0f, 1.0f)),
	Vertex(Eigen::Vector3f(1.0f, -1.0f, 0.0f), n, Eigen::Vector2f(1.0f, 0.0f)) };

	std::vector<unsigned int> faceIds = {1, 0, 3, 1, 3, 2};
	Mesh* quad = new Mesh(vertices, faceIds);
	return quad;
}

void Mesh::init() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(2, m_vbo);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_faceIds.size(), m_faceIds.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices[0].m_position.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	m_ready = true;
}

void Mesh::display(Shader* shader) {
	if (!m_ready)
		init();

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);

	std::size_t index = 0;

	// Positions
	int vertexLoc = shader->getAttribLocation("vtx_position");
	if (vertexLoc >= 0) {
		glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
		glEnableVertexAttribArray(vertexLoc);
	}

	index += sizeof(Eigen::Vector3f);

	// Normals
	int normalLoc = shader->getAttribLocation("vtx_normal");
	if (normalLoc >= 0) {
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
		glEnableVertexAttribArray(normalLoc);
	}

	index += sizeof(Eigen::Vector3f);

	// Texture coordinates
	int texcoordLoc = shader->getAttribLocation("vtx_texcoord");
	if (texcoordLoc >= 0) {
		glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)index);
		glEnableVertexAttribArray(texcoordLoc);
	}

	glDrawElements(GL_TRIANGLES, m_faceIds.size(), GL_UNSIGNED_INT, 0);

	if (vertexLoc >= 0)
		glDisableVertexAttribArray(vertexLoc);
	if (normalLoc >= 0)
		glDisableVertexAttribArray(normalLoc);
	if (texcoordLoc >= 0)
		glDisableVertexAttribArray(texcoordLoc);

	glBindVertexArray(0);
}