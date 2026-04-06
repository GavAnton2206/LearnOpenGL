cmake_minimum_required(VERSION 3.16)
project(MyOpenGLProject)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Collect sources
file(GLOB_RECURSE IMGUI_SOURCES "${CMAKE_SOURCE_DIR}/vendor/src/imgui/*.cpp")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/camera.cpp
    src/gldebugger.cpp
    src/input.cpp
    src/light.cpp
    src/objects.cpp
    src/shader.cpp
    src/utils.cpp

    vendor/glad/glad.c
    vendor/stb_image/stb_image.cpp

    ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/
    ${CMAKE_SOURCE_DIR}/vendor/GLFW
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/KHR
    ${CMAKE_SOURCE_DIR}/vendor/src
    ${CMAKE_SOURCE_DIR}/vendor/stb_image
    /opt/homebrew/include
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glm::glm
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)

# macOS: silence OpenGL deprecation warnings
target_compile_definitions(${PROJECT_NAME} PRIVATE
    GL_SILENCE_DEPRECATION
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)