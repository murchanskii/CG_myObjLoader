cmake_minimum_required(VERSION 3.9)
project(CG_Lab)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(C:/mingw-w64/x86_64-7.2.0-posix-sjlj-rt_v5-rev1/mingw64/glfw-3.2.1 ${CMAKE_CURRENT_BINARY_DIR}/glfw-3.2.1)
find_package(OpenGL REQUIRED)

add_executable(CG_Lab main.cpp glad.c Program.h Program.cpp tiny_obj_loader.h stb_image.h Mesh.cpp Mesh.h Model.cpp Model.h)

target_include_directories(CG_Lab PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(CG_Lab glfw SOIL ${OPENGL_gl_LIBRARY} "-static")