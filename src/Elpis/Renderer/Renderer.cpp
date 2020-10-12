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
	}

	Renderer::~Renderer()
	{
		glfwTerminate();
	}

	void Renderer::run()
	{
		ImGuiWindowFlags windowFlags = initInterface();
		glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
		while (!glfwWindowShouldClose((GLFWwindow*)m_window->getNativeWindow()))
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_scene->render();

			setInterface(windowFlags);

			glfwSwapBuffers((GLFWwindow*)m_window->getNativeWindow());
			glfwPollEvents();
		}
		glfwMakeContextCurrent((GLFWwindow*)m_window->getNativeWindow());
	}

	ImGuiWindowFlags Renderer::initInterface()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = ImVec2(m_window->getWidth(), m_window->getHeight());
		std::string fontPath = EL_RESOURCE_PATH("fonts/Arial.ttf");
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f);
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_window->getNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoResize;

		return windowFlags;
	}

	void Renderer::setInterface(const ImGuiWindowFlags& windowFlags)
	{
		Ref<Material> mat = MaterialManager::getInstance()->getMaterial("basic");

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(350, m_window->getHeight()));
		ImGui::Begin("Parameters", NULL, windowFlags);

		{
			ImGui::BeginChild("Model", ImVec2(0, 520), true, ImGuiWindowFlags_None);
			static int e = 1;
			ImGui::RadioButton("Sphere", &e, 0); ImGui::SameLine(); ImGui::RadioButton("Model", &e, 1);
			if (ImGui::CollapsingHeader("Mesh"))
			{
				if (ImGui::Button("..."))
				{
					std::string filepath = fileDialog();
					if (!filepath.empty())
						m_scene->getEntity(0)->setMesh(Mesh::createMesh(filepath));
				}
			}
			if (ImGui::CollapsingHeader("Albedo"))
			{
				unsigned int id = mat->basecolorMap ? mat->basecolorMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = fileDialog();
					mat->basecolorMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use", &(mat->useBasecolorMap));
				ImGui::ColorEdit3("Color", mat->basecolor.data());
			}
			if (ImGui::CollapsingHeader("Normals"))
			{
				unsigned int id = mat->normalMap ? mat->normalMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = fileDialog();
					mat->normalMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use ", &(mat->useNormalMap));
			}
			if (ImGui::CollapsingHeader("Metalness"))
			{
				unsigned int id = mat->metallicMap ? mat->metallicMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = fileDialog();
					mat->metallicMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use  ", &(mat->useMetallicMap));
				ImGui::SliderFloat("Metallic", &(mat->metallic), 0.0, 1.0);
			}
			if (ImGui::CollapsingHeader("Roughness"))
			{
				unsigned int id = mat->roughnessMap ? mat->roughnessMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = fileDialog();
					mat->roughnessMap = createRef<Texture>(filepath);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use   ", &(mat->useRoughnessMap));
				ImGui::SliderFloat("Roughness", &(mat->roughness), 0.0, 1.0);
			}
			if (ImGui::CollapsingHeader("Ao"))
			{
				unsigned int id = mat->aoMap ? mat->aoMap->getId() : 0;
				if (ImGui::ImageButton((void*)id, ImVec2(50, 50))) {
					std::string filepath = fileDialog();
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
				std::string filepath = fileDialog();
				if (!filepath.empty())
					m_scene->setCubemap(filepath);
			}
			ImGui::SliderFloat("Skybox LOD", &(m_scene->getCubemap()->getLod()), 0.0f, float(m_scene->getCubemap()->getMaxMipLevels() - 1));
			ImGui::SliderFloat("Exposure", &(m_scene->getCubemap()->getExposure()), 0.0f, 3.0f);
			ImGui::EndChild();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Renderer::onEvent(Event& e)
	{
		m_scene->onEvent(e);
	}

	std::string Renderer::fileDialog()
	{
		std::string filepath;
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						std::wstring ws(pszFilePath);
						filepath = std::string(ws.begin(), ws.end());
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		return filepath;
	}
}