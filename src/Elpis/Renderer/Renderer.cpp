#include "Renderer.h"

Renderer::Renderer(): m_width(1280), m_height(720)
{
	initWindow();
	m_scene = createRef<Scene>();

	// Cubemap
	m_scene->setCubemap("../../res/cubemaps/birchwood_4k.hdr");

	// Lights
	m_scene->addLight(createRef<Light>(LightType::Point, Vec3(-10.0, 10.0, 10.0)));
	m_scene->addLight(createRef<Light>(LightType::Point, Vec3(10.0, 10.0, 10.0)));
	m_scene->addLight(createRef<Light>(LightType::Point, Vec3(-10.0, -10.0, 10.0)));
	m_scene->addLight(createRef<Light>(LightType::Point, Vec3(10.0, -10.0, 10.0)));

	// Shaders
	m_scene->addShader(createRef<Shader>("../../res/shaders/PBR.shader"), "PBR");

	// Materials
	std::string textureName = "basic";
	Ref<MaterialManager> matManager = MaterialManager::getInstance();
	matManager->addMaterial(textureName);

	// Meshes
	Ref<Mesh> mesh = Mesh::createMesh("../../res/models/Cerberus_LP.FBX");

	// Entities
	m_scene->addEntity(mesh, "mesh", "PBR", textureName, Vec3(0, 0, 0));
	m_scene->getEntity(0)->rotate(-M_PI_2, Vec3(1, 0, 0));
}

Renderer::~Renderer()
{
	glfwTerminate();
}

void Renderer::initWindow()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	m_window = glfwCreateWindow(m_width, m_height, "Elpis", NULL, NULL);

	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::run()
{
	ImGuiWindowFlags windowFlags = initInterface();
	glViewport(0, 0, m_width, m_height);
	while (!glfwWindowShouldClose(m_window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_scene->render();

		setInterface(windowFlags);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
	glfwMakeContextCurrent(m_window);
}

ImGuiWindowFlags Renderer::initInterface()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2(m_width, m_height);
	io.Fonts->AddFontFromFileTTF("../../res/fonts/Arial.ttf", 18.0f);
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
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
	ImGui::SetNextWindowSize(ImVec2(350, m_height));
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

void Renderer::charPressed(int key)
{

}

void Renderer::keyPressed(int key, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(m_window, true);
		break;
	}
}

void Renderer::reshapeWindow(int width, int height) {
	glViewport(0, 0, width, height);
	m_width = width;
	m_height = height;
}

void Renderer::mouseScrolled(double x, double y)
{
	m_scene->getCamera()->zoom((y > 0) ? 1.1 : 1. / 1.1);
}

void Renderer::mousePressed(GLFWwindow* window, int button, int action)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			m_scene->getCamera()->startRotation(m_lastMousePos);
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			m_scene->getCamera()->startTranslation(m_lastMousePos);
		m_button = button;
	}
	else if (action == GLFW_RELEASE) {
		if (m_button == GLFW_MOUSE_BUTTON_LEFT)
			m_scene->getCamera()->endRotation();
		else if (m_button == GLFW_MOUSE_BUTTON_RIGHT)
			m_scene->getCamera()->endTranslation();
		m_button = -1;
	}
}

void Renderer::mouseMoved(double x, double y)
{
	if (x > 350)
	{
		if (m_button == GLFW_MOUSE_BUTTON_LEFT)
			m_scene->getCamera()->dragRotate(Vec2(x, y));
		else if (m_button == GLFW_MOUSE_BUTTON_RIGHT)
			m_scene->getCamera()->dragTranslate(Vec2(x, y));
		m_lastMousePos = Vec2(x, y);
	}
}

void Renderer::setCallbacks()
{
	glfwSetWindowUserPointer(m_window, this);

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int key)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->charPressed(key);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->keyPressed(key, action, mods);
	});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->mouseScrolled(x, y);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->mousePressed(window, button, action);
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->mouseMoved(x, y);
	});

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
		Renderer* viewer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		viewer->reshapeWindow(width, height);
	});
}

std::string Renderer::fileDialog()
{
	std::string filepath;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

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