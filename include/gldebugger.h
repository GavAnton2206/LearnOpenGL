#pragma once

#include <glad/glad.h>

static class GLDebugger {
public:
	void Setup();

private:
	static void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		unsigned int id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam);
};