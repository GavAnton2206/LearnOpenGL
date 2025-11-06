#include "input.h"

#include <vector>
#include <string>

bool ActionBinding::operator==(const ActionBinding& other) const {
	return key == other.key && type == other.type;
}

bool KeyState::justPressed() const { return current && !previous; }
bool KeyState::justReleased() const { return !current && previous; }
bool KeyState::pressed() const { return current; }

void Input::Process(GLFWwindow* window) {
	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
		KeyState& state = keyStates[key];
		state.previous = state.current;
		state.current = (glfwGetKey(window, key) == GLFW_PRESS);
	}

	for (auto& [actionBinding, action] : actions) {
		const KeyState& s = keyStates[actionBinding.key];

		switch (actionBinding.type) {
			case InputEventType::JUST_PRESSED:
				if (s.justPressed()) action();
				break;
			case InputEventType::JUST_RELEASED:
				if (s.justReleased()) action();
				break;
			case InputEventType::PRESSED:
				if (s.pressed()) action();
				break;
		}
	}
}

void Input::BindAction(int key, InputEventType type, std::function<void()> callback) {
	bool exists = false;

	for (auto& [actionBinding, action] : actions) {
		if (actionBinding.type == type && actionBinding.key == key) {
			actions[actionBinding] = std::move(callback);
			exists = true;
		}
	}

	if (!exists) {
		ActionBinding actionBinding;
		actionBinding.key = key;
		actionBinding.type = type;
		actions[actionBinding] = std::move(callback);
	}
}