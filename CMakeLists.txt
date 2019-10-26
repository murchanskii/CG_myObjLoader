cmake_minimum_required(VERSION 3.14)
project(CG_Lab)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(dependencies/glad/include dependencies/GLFW/include dependencies/glm dependencies/SOIL/include)
link_directories(dependencies/SOIL/lib dependencies/GLFW/lib)

add_executable(CG_Lab main.cpp dependencies/glad/src/glad.c Program.h Program.cpp tiny_obj_loader.h stb_image.h Mesh.cpp Mesh.h Model.cpp Model.h additional_structs.h)

target_link_libraries(CG_Lab SOIL ${OPENGL_gl_LIBRARY} glfw3 "-static")