#include "MaterialManager.h"

Ref<MaterialManager> MaterialManager::s_instance = 0;

const Ref<MaterialManager>& MaterialManager::getInstance()
{
	if (!s_instance)
		s_instance = createRef<MaterialManager>();
	return s_instance;
}


void MaterialManager::addMaterial(const std::string& materialName)
{
	Ref<Material> newMat = createRef<Material>();
	m_materials.insert(std::make_pair(materialName, newMat));
}