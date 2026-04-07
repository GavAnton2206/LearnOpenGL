cmake_minimum_required(VERSION 3.16)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${sourceDir}/build/vcpkg_installed/x64-windows/")
endif()

# Find packages
if(WIN32)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Collect sources
set(PLATFORM_SOURCES)

if (WIN32)
    list(APPEND PLATFORM_SOURCES vendor/glad/windows/src/glad.c)
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES vendor/glad/macos/src/glad.c)
endif()

file(GLOB_RECURSE SOURCES_ "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES "${CMAKE_SOURCE_DIR}/vendor/imgui/src/*.cpp")

add_executable(${PROJECT_NAME}
    vendor/stb_image/src/stb_image.cpp

    ${SOURCES_}
    ${IMGUI_SOURCES}
    ${PLATFORM_SOURCES}
)

# Collect includes
set(PLATFORM_INCLUDES)

if (WIN32)
    list(APPEND PLATFORM_INCLUDES ${CMAKE_SOURCE_DIR}/vendor/glad/windows/include/)
elseif(APPLE)
    list(APPEND PLATFORM_INCLUDES ${CMAKE_SOURCE_DIR}/vendor/glad/macos/include/)
    list(APPEND PLATFORM_INCLUDES /opt/homebrew/include)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/KHR/include/
    ${CMAKE_SOURCE_DIR}/vendor/imgui/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui/include/imgui
    ${CMAKE_SOURCE_DIR}/vendor/stb_image/include

    ${PLATFORM_INCLUDES}
)

# Link libraries
set(PLATFORM_LINK_LIBRARIES)

if(APPLE)
    list(APPEND PLATFORM_LINK_LIBRARIES "-framework Cocoa")
    list(APPEND PLATFORM_LINK_LIBRARIES "-framework IOKit")
    list(APPEND PLATFORM_LINK_LIBRARIES "-framework CoreVideo")
endif()

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glm::glm
    ${PLATFORM_LINK_LIBRARIES}
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Darwin>:GL_SILENCE_DEPRECATION>
    IMGUI_IMPL_OPENGL_LOADER_CUSTOM
)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()