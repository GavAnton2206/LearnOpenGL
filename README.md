# Learning OpenGL
This project was developed as part of an OpenGL study, guided by tutorials from LearnOpenGL and other resources.

# Screenshots
![Skybox and UI](/preview/1.png)
![PBR](/preview/2.png)
![Outline and Light](/preview/3.png)
## Demo
![PBR](/preview/pbr.gif)
![Physics](/preview/physics.gif)

# Features
- Lightweight physics system with collision detection (sphere–sphere, sphere–AABB, AABB–AABB) and impulse-based resolution
- Event-driven input system supporting ```justPressed```, ```justReleased```, and ```pressed``` states
- OpenGL debug callback for runtime error tracking
- Shader implementations including Blinn–Phong with normal mapping and a basic PBR pipeline

# Installation
## Windows
1) Use vcpkg to download glm and glfw
2) Launch with
```
cmake --preset windows
cmake --build build
build\Debug\LearnOpenGL.exe
```
3) **For Visual Studio**: use Solution Explorer - View to choose Targets View, then add the following line to Add Debug Configuration:
```
"currentDir": "${workspaceRoot}"
```
## MacOS
1) Use homebrew to download glm and glfw
2) Launch with
```
cmake --preset windows
cmake --build build
build\LearnOpenGL
```