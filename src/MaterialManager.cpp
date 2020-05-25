#include "MaterialManager.h"

MaterialManager* MaterialManager::s_instance = 0;

MaterialManager* MaterialManager::getInstance()
{
	if (!s_instance)
		s_instance = new MaterialManager();
	return s_instance;
}


void MaterialManager::addMaterial(const std::string& materialName)
{
	Material* newMat = new Material();
	m_materials.insert(std::make_pair(materialName, newMat));
}