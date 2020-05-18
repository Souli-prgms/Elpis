#pragma once

#include <iostream>

#include "Scene.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void run();

	void charPressed(int key);
	void keyPressed(int key, int action, int mods);
	void mouseScrolled(double x, double y);
	void mousePressed(GLFWwindow* window, int button, int action);
	void mouseMoved(double x, double y);
	void reshapeWindow(int width, int height);

private:
	void initWindow();
	void renderScene();
	void setCallbacks();

	GLFWwindow* m_window;
	Scene* m_scene;

	int m_button = -1;
	Eigen::Vector2f m_lastMousePos;
};