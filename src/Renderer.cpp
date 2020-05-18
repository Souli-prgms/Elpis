#include "Renderer.h"

Renderer::Renderer()
{
	initWindow();
	m_scene = new Scene();

	// Lights
	m_scene->addLight(new Light(LightType::Point, Eigen::Vector3f(-10.0, 10.0, 10.0)));
	m_scene->addLight(new Light(LightType::Point, Eigen::Vector3f(10.0, 10.0, 10.0)));
	m_scene->addLight(new Light(LightType::Point, Eigen::Vector3f(-10.0, -10.0, 10.0)));
	m_scene->addLight(new Light(LightType::Point, Eigen::Vector3f(10.0, -10.0, 10.0)));

	// Shaders
	m_scene->addShader(new Shader("../../res/shaders/PBR.shader"), "PBR");

	// Textures
	m_scene->addTexture(new Texture("../../res/materials/rusted_iron_2/basecolor.png"), "rusted_iron/basecolor");
	m_scene->addTexture(new Texture("../../res/materials/rusted_iron_2/metallic.png"), "rusted_iron/metallic");
	m_scene->addTexture(new Texture("../../res/materials/rusted_iron_2/roughness.png"), "rusted_iron/roughness");

	m_scene->addTexture(new Texture("../../res/materials/gold/basecolor.png"), "gold/basecolor");
	m_scene->addTexture(new Texture("../../res/materials/gold/metallic.png"), "gold/metallic");
	m_scene->addTexture(new Texture("../../res/materials/gold/roughness.png"), "gold/roughness");

	// Texture Maps
	std::map<std::string, std::string> rustedIron = { {"albedo_map", "rusted_iron/basecolor"},
	{"metallic_map", "rusted_iron/metallic"}, 
	{"roughness_map", "rusted_iron/roughness"}, 
	};
	std::map<std::string, std::string> gold = { {"albedo_map", "gold/basecolor"},
	{"metallic_map", "gold/metallic"},
	{"roughness_map", "gold/roughness"},
	};
	std::map<std::string, std::string> cubemapTextureMap = { {"equirectangular_map", "cubemap"}};

	// Meshes
	Mesh* sphere = Mesh::createSphere(2, 64, 64);

	// Entities
	m_scene->addEntity(sphere, "sphere", "PBR", rustedIron, Eigen::Vector3f(0, 5, 0));
}

Renderer::~Renderer()
{
	glfwTerminate();
}

void Renderer::initWindow()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	m_window = glfwCreateWindow(1280, 720, "Elpis", NULL, NULL);

	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);

	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(m_window, this);
	setCallbacks();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_MULTISAMPLE);
}

void Renderer::run()
{
	m_scene->computeCubemap();
	glViewport(0, 0, 1280, 720);
	while (!glfwWindowShouldClose(m_window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderScene();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
	glfwMakeContextCurrent(m_window);
}

void Renderer::renderScene()
{
	m_scene->render();
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
}

void Renderer::mouseScrolled(double x, double y)
{
	m_scene->getCamera()->zoom((y > 0) ? 1.1 : 1. / 1.1);
}

void Renderer::mousePressed(GLFWwindow* window, int button, int action)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_scene->getCamera()->startRotation(m_lastMousePos);
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			m_scene->getCamera()->startTranslation(m_lastMousePos);
		}
		m_button = button;
	}
	else if (action == GLFW_RELEASE) {
		if (m_button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_scene->getCamera()->endRotation();
		}
		else if (m_button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			m_scene->getCamera()->endTranslation();
		}
		m_button = -1;
	}
}

void Renderer::mouseMoved(double x, double y)
{
	if (m_button == GLFW_MOUSE_BUTTON_LEFT)
	{
		m_scene->getCamera()->dragRotate(Eigen::Vector2f(x, y));
	}
	else if (m_button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		m_scene->getCamera()->dragTranslate(Eigen::Vector2f(x, y));
	}
	m_lastMousePos = Eigen::Vector2f(x, y);
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