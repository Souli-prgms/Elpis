#include "InterfaceLayer.h"
#include "Renderer.h"

namespace Elpis
{
	void InterfaceLayer::onAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		std::string fontPath = EL_RESOURCE_PATH("fonts/Arial.ttf");
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f);
		
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		setColors();

		GLFWwindow* window = static_cast<GLFWwindow*>(RENDERER->getWindow().getNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void InterfaceLayer::onDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void InterfaceLayer::onEvent(Event& e)
	{
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.handled |= e.IsInCategory(EventCategory::Mouse) & io.WantCaptureMouse;
			e.handled |= e.IsInCategory(EventCategory::Keyboard) & io.WantCaptureKeyboard;
		}
	}

	void InterfaceLayer::begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void InterfaceLayer::set()
	{
		Scene& scene = RENDERER->getScene();
		Framebuffer& fb = RENDERER->getFramebuffer();

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
		m_blockEvents = !ImGui::IsWindowHovered();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		uint64_t textureID = fb.getColorAttachmentRendererID();
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
						scene.getEntity(0)->setMesh(Mesh::createMesh(filepath));
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
					scene.setCubemap(filepath);
			}
			ImGui::SliderFloat("Skybox LOD", &(scene.getCubemap()->getLod()), 0.0f, float(scene.getCubemap()->getMaxMipLevels() - 1));
			ImGui::SliderFloat("Exposure", &(scene.getCubemap()->getExposure()), 0.0f, 3.0f);
			ImGui::EndChild();
		}
		ImGui::End();
		ImGui::End();
	}

	void InterfaceLayer::end()
	{
		ImGuiIO& io = ImGui::GetIO();
		Renderer* app = RENDERER;
		io.DisplaySize = ImVec2((float)(app->getWindow().getWidth()), (float)app->getWindow().getHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* currentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(currentContext);
		}
	}

	void InterfaceLayer::setColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	}
}