#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Input
{

private:
	static Input* instance;

	GLFWwindow* window;

	bool keys[GLFW_KEY_LAST];
	bool keysDown[GLFW_KEY_LAST];
	bool keysUp[GLFW_KEY_LAST];

	bool mouseButtons[GLFW_MOUSE_BUTTON_LAST];
	bool mouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
	bool mouseButtonsUp[GLFW_MOUSE_BUTTON_LAST];

	glm::vec2 mousePosition;
	glm::vec2 mouseDelta;

	Input() : window(nullptr), mousePosition(0.0f), mouseDelta(0.0f) {
		for (int i = 0; i < GLFW_KEY_LAST; ++i) {
			keys[i] = false;
			keysDown[i] = false;
			keysUp[i] = false;
		}
	}

public:
	void SetWindow(GLFWwindow* window) {
		this->window = window;
	}

	static Input* GetInstance() {
		if (instance == nullptr) {
			instance = new Input();
		}

		return instance;
	}

	static void SetMouseVisibility(bool isVisible) {
		if (isVisible) {
			glfwSetInputMode(GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(GetInstance()->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}

	void Update() {
		for (int i = 0; i < GLFW_KEY_LAST; ++i) {
			keysDown[i] = false;
			keysUp[i] = false;
		}

		for (int i = 0; i < GLFW_KEY_LAST; ++i) {
			if (glfwGetKey(window, i) == GLFW_PRESS) {
				if (!keys[i]) {
					keysDown[i] = true;
				}
				keys[i] = true;
			}
			else {
				if (keys[i]) {
					keysUp[i] = true;
				}
				keys[i] = false;
			}
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
			mouseButtonsDown[i] = false;
			mouseButtonsUp[i] = false;
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
			if (glfwGetMouseButton(window, i)) {
				if (!mouseButtons[i]) {
					mouseButtonsDown[i] = true;
				}
				mouseButtons[i] = true;
			}
			else {
				if (mouseButtons[i]) {
					mouseButtonsUp[i] = true;
				}
				mouseButtons[i] = false;
			}
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glm::vec2 newMousePosition = glm::vec2(mouseX, mouseY);
		mouseDelta = newMousePosition - mousePosition;
		mousePosition = newMousePosition;
	}

	static bool GetKey(int key) {
		return GetInstance()->keys[key];
	}

	static bool GetKeyDown(int key) {
		return GetInstance()->keysDown[key];
	}

	static bool GetKeyUp(int key) {
		return GetInstance()->keysUp[key];
	}

	static glm::vec2 GetMousePosition() {
		return GetInstance()->mousePosition;
	}

	static glm::vec2 GetMouseDelta() {
		return GetInstance()->mouseDelta;
	}

	static bool GetMouseButton(int button)
	{
		return GetInstance()->mouseButtons[button];
	}

	static bool GetMouseButtonDown(int button)
	{
		return GetInstance()->mouseButtonsDown[button];
	}

	static bool GetMouseButtonUp(int button)
	{
		return GetInstance()->mouseButtonsUp[button];
	}
};