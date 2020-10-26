#include "Renderer.h"

namespace Elpis
{
	Renderer::Renderer()
	{
		m_window = Window::create(WindowProperties("Elpis Engine"));
		m_window->setEventCallback(EL_BIND_EVENT_FN(Renderer::onEvent));

		m_scene = createRef<Scene>();

		// Cubemap
		m_scene->setCubemap(EL_RESOURCE_PATH("cubemaps/birchwood_4k.hdr"));

		// Lights
		m_scene->addLight(createRef<Light>(LightType::Point, Vec3(-10.0, 10.0, 10.0)));
		m_scene->addLight(createRef<Light>(LightType::Point, Vec3(10.0, 10.0, 10.0)));
		m_scene->addLight(createRef<Light>(LightType::Point, Vec3(-10.0, -10.0, 10.0)));
		m_scene->addLight(createRef<Light>(LightType::Point, Vec3(10.0, -10.0, 10.0)));

		// Shaders
		m_scene->addShader(createRef<Shader>(EL_RESOURCE_PATH("shaders/PBR.shader")), "PBR");

		// Materials
		std::string textureName = "basic";
		Ref<MaterialManager> matManager = MaterialManager::getInstance();
		matManager->addMaterial(textureName);

		// Meshes
		Ref<Mesh> mesh = Mesh::createMesh(EL_RESOURCE_PATH("models/Cerberus_LP.FBX"));

		// Entities
		m_scene->addEntity(mesh, "mesh", "PBR", textureName, Vec3(0, 0, 0));
		m_scene->getEntity(0)->rotate(-M_PI_2, Vec3(1, 0, 0));

		// Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.width = m_window->getWidth();
		fbSpec.height = m_window->getHeight();
		m_framebuffer = Framebuffer::create(fbSpec);
	}

	Renderer::~Renderer()
	{
		glfwTerminate();
	}

	void Renderer::run()
	{
		initInterface();
		glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
		while (!glfwWindowShouldClose((GLFWwindow*)m_window->getNativeWindow()))
		{
			m_framebuffer->bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_scene->render();
			m_framebuffer->unbind();

			setInterface();

			glfwSwapBuffers((GLFWwindow*)m_window->getNativeWindow());
			glfwPollEvents();
			
		}
		glfwMakeContextCurrent((GLFWwindow*)m_window->getNativeWindow());
	}

	void Renderer::initInterface()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.DisplaySize = ImVec2(m_window->getWidth(), m_window->getHeight());
		std::string fontPath = EL_RESOURCE_PATH("fonts/Arial.ttf");
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f);
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_window->getNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	}

	void Renderer::setInterface()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
		uint64_t textureID = m_framebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		Ref<Material> mat = MaterialManager::getInstance()->getMaterial("basic");
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

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backupCurrentContext);
		}
	}

	void Renderer::onEvent(Event& e)
	{
		if (!m_blockEvents)
			m_scene->onEvent(e);
	}
}