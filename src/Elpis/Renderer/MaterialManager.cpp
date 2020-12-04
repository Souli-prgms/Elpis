#include "MaterialManager.h"

namespace Elpis
{
	Ref<MaterialLibrary> MaterialLibrary::s_instance = 0;

	const Ref<MaterialLibrary>& MaterialLibrary::getInstance()
	{
		if (!s_instance)
			s_instance = createRef<MaterialLibrary>();
		return s_instance;
	}

	Ref<Material>& MaterialLibrary::addMaterial(const std::string& materialName)
	{
		Ref<Material> newMat = createRef<Material>();
		m_materials.insert(std::make_pair(materialName, newMat));
		return newMat;
	}
}