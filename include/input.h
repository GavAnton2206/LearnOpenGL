#pragma once
#include <GLFW/glfw3.h>

#include <functional>

#include <array>
#include <unordered_map>

enum InputEventType {
	JUST_PRESSED,
	JUST_RELEASED,
	PRESSED
};

struct KeyState {
	bool current = false;
	bool previous = false;

	bool justPressed() const;
	bool justReleased() const;
	bool pressed() const;
};

struct ActionBinding {
	int key;
	InputEventType type;

	bool operator==(const ActionBinding& other) const;
};

namespace std {
	template<>
	struct hash<ActionBinding> {
		std::size_t operator()(const ActionBinding& a) const noexcept {
			std::size_t h1 = std::hash<int>{}(a.key);
			std::size_t h2 = std::hash<int>{}(static_cast<int>(a.type));

			return h1 ^ (h2 << 1);
		}
	};
}

class Input {
public:
	void Initialize();
	void Process(GLFWwindow* window);
	void BindAction(int key, InputEventType type, std::function<void()> callback);

private:
	std::array<KeyState, GLFW_KEY_LAST + 1> keyStates;
	std::unordered_map<ActionBinding, std::function<void()>> actions;
};