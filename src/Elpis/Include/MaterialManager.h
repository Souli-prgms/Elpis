#pragma once

#include "Texture.h"

namespace Elpis
{
	struct Material
	{
		Ref<Texture> basecolorMap;
		Ref<Texture> metallicMap;
		Ref<Texture> roughnessMap;
		Ref<Texture> normalMap;
		Ref<Texture> aoMap;

		bool useBasecolorMap = false;
		bool useMetallicMap = false;
		bool useRoughnessMap = false;
		bool useNormalMap = false;
		bool useAoMap = false;

		Vec3 basecolor = Vec3(1.0, 1.0, 1.0);
		float metallic = 1.0;
		float roughness = 0.15;
		float ao = 1.0;
	};

	class MaterialManager
	{
	public:
		MaterialManager() {}
		~MaterialManager() {}

		static const Ref<MaterialManager>& getInstance();

		void addMaterial(const std::string& materialName);
		const Ref<Material>& getMaterial(const std::string& materialName) { return m_materials[materialName]; }

	private:
		std::map<std::string, Ref<Material>> m_materials;

		static Ref<MaterialManager> s_instance;
	};
}