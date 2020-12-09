#include "Renderer.h"

namespace Elpis
{
	void sampleScene()
	{
		Scene& scene = RENDERER->getScene();
		// Cubemap
		scene.setCubemap(EL_RESOURCE_PATH("cubemaps/birchwood_4k.hdr"));

		// Lights
		scene.addLight(createRef<Light>(LightType::Point, Vec3(-10.0, 10.0, 10.0)));

		// Shaders
		SHADER_LIB->load("PBR", EL_RESOURCE_PATH("shaders/PBR.shader"));

		// Materials
		Ref<Material>& basicMat = MATERIAL_LIB->addMaterial("basic");
		Ref<Material> groundMat = MATERIAL_LIB->addMaterial("ground");
		groundMat->metallic = 0.0;

		// Meshes
		Ref<Mesh> mesh = Mesh::createMesh(EL_RESOURCE_PATH("models/Cerberus_LP.FBX"));
		Ref<Mesh> quad = Mesh::createMesh(EL_RESOURCE_PATH("models/cube.obj"));

		// Entities
		scene.addEntity(mesh, "mesh", "PBR", "basic", Vec3(0, 50, -50));
		scene.getEntity(0)->rotate(-M_PI_2, Vec3(1, 0, 0));

		scene.addEntity(quad, "quad", "PBR", "ground", Vec3(0, 0, 0));
		scene.getEntity(1)->scale(150., 1., 150.);
	}
}


int main(int argc, char *argv[])
{
	Elpis::Log::Init();
	Elpis::sampleScene();
	Elpis::RENDERER->run();
}
