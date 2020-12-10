#include "NutLayer.h"

namespace Elpis
{
	NutLayer::NutLayer()
	{

	}

	void NutLayer::onAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.width = APP.getWindow().getWidth();
		fbSpec.height = APP.getWindow().getHeight();
		m_framebuffer = Framebuffer::create(fbSpec);

		m_scene = createRef<Scene>();

		// Cubemap
		m_scene->setCubemap(EL_RESOURCE_PATH("cubemaps/birchwood_4k.hdr"));

		// Lights
		m_scene->addLight(createRef<Light>(LightType::Point, Vec3(-10.0, 10.0, 10.0)));

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
		m_scene->addEntity(mesh, "mesh", "PBR", "basic", Vec3(0, 50, -50));
		m_scene->getEntity(0)->rotate(-M_PI_2, Vec3(1, 0, 0));

		m_scene->addEntity(quad, "quad", "PBR", "ground", Vec3(0, 0, 0));
		m_scene->getEntity(1)->scale(150., 1., 150.);
	}

	void NutLayer::onDetach()
	{

	}

	void NutLayer::onUpdate(Timestep ts)
	{
		m_framebuffer->bind();
		RENDERER_API.setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RENDERER_API.clear();
		m_scene->render();
		m_framebuffer->unbind();
	}

	void NutLayer::onInterfaceRender()
	{
		static bool dockspaceOpen = true;
		static bool optFullscreenPersistant = true;
		bool optFullscreen = optFullscreenPersistant;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (optFullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspaceFlags && ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar();

		if (optFullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		style.WindowMinSize.x = minWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		APP.getInterfaceLayer()->blockEvents(!ImGui::IsWindowHovered());
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		uint64_t textureID = m_framebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		Ref<Material> mat = MATERIAL_LIB->getMaterial("basic");
		ImGui::Begin("Parameters");
		{
			ImGui::BeginChild("Model", ImVec2(0, 520), true, ImGuiWindowFlags_None);
			static int e = 1;
			ImGui::RadioButton("Sphere", &e, 0); ImGui::SameLine(); ImGui::RadioButton("Model", &e, 1);
			if (ImGui::CollapsingHeader("Mesh"))
			{
				if (ImGui::Button("..."))
				{
					std::string filepath = FileDialog::getFile();
					if (!filepath.empty())
						m_scene->getEntity(0)->setMesh(Mesh::createMesh(filepath));
				}
			}
			if (ImGui::CollapsingHeader("Albedo"))
			{
				uint32_t id = mat->basecolorMap ? mat->basecolorMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = FileDialog::getFile();
					mat->basecolorMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use", &(mat->useBasecolorMap));
				ImGui::ColorEdit3("Color", mat->basecolor.data());
			}
			if (ImGui::CollapsingHeader("Normals"))
			{
				uint32_t id = mat->normalMap ? mat->normalMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = FileDialog::getFile();
					mat->normalMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use ", &(mat->useNormalMap));
			}
			if (ImGui::CollapsingHeader("Metalness"))
			{
				uint32_t id = mat->metallicMap ? mat->metallicMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = FileDialog::getFile();
					mat->metallicMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use  ", &(mat->useMetallicMap));
				ImGui::SliderFloat("Metallic", &(mat->metallic), 0.0, 1.0);
			}
			if (ImGui::CollapsingHeader("Roughness"))
			{
				uint32_t id = mat->roughnessMap ? mat->roughnessMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = FileDialog::getFile();
					mat->roughnessMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use   ", &(mat->useRoughnessMap));
				ImGui::SliderFloat("Roughness", &(mat->roughness), 0.0, 1.0);
			}
			if (ImGui::CollapsingHeader("Ao"))
			{
				uint32_t id = mat->aoMap ? mat->aoMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = FileDialog::getFile();
					mat->aoMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use    ", &(mat->useAoMap));
				ImGui::SliderFloat("Ao", &(mat->ao), 0.0, 1.0);
			}
			ImGui::EndChild();
		}
		{
			ImGui::BeginChild("Environment", ImVec2(0, 150), true, ImGuiWindowFlags_None);

			if (ImGui::Button("Load environment map"))
			{
				std::string filepath = FileDialog::getFile();
				if (!filepath.empty())
					m_scene->setCubemap(filepath);
			}
			ImGui::SliderFloat("Skybox LOD", &(m_scene->getCubemap()->getLod()), 0.0f, float(m_scene->getCubemap()->getMaxMipLevels() - 1));
			ImGui::SliderFloat("Exposure", &(m_scene->getCubemap()->getExposure()), 0.0f, 3.0f);
			ImGui::EndChild();
		}
		ImGui::End();
		ImGui::End();
	}

	void NutLayer::onEvent(Event& e)
	{
		m_scene->onEvent(e);
	}
}