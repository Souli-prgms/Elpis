#pragma once

#include <map>
#include <Eigen/Eigen> 

#include "Texture.h"

struct Material
{
	Texture* basecolorMap;
	Texture* metallicMap;
	Texture* roughnessMap;
	Texture* normalMap;
	Texture* aoMap;

	bool useBasecolorMap = false;
	bool useMetallicMap = false;
	bool useRoughnessMap = false;
	bool useNormalMap = false;
	bool useAoMap = false;

	Eigen::Vector3f basecolor = Eigen::Vector3f(1.0, 1.0, 1.0);
	float metallic = 1.0;
	float roughness = 0.15;
	float ao = 1.0;
};

class MaterialManager
{
public:
	static MaterialManager* getInstance();
	
	void addMaterial(const std::string& materialName);
	Material* getMaterial(const std::string& materialName) { return m_materials[materialName]; }

private:
	std::map<std::string, Material*> m_materials;

	MaterialManager() {}
	~MaterialManager() {}

	static MaterialManager* s_instance;
};

