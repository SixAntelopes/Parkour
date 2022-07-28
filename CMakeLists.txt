cmake_minimum_required(VERSION 3.0.0)
project(Parkout VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

set(GLAD_H ${PROJECT_SOURCE_DIR}/include)
set(GLFW_H D:/Software/GLFW-tdm/include)
set(IRRKLANG_H D:/Software/irrKlang/include)
set(FREETYPE_H D:/Software/freetype-tdm/include/freetype2)
include_directories(${GLAD_H} ${GLFW_H} ${IRRKLANG_H} ${FREETYPE_H})

# set(SOURCES glad.c main.cpp shader.cpp texture.cpp resource_manager.cpp
#             sprite_renderer.cpp game_object.cpp game_level.cpp ball_object.cpp
#             particle_generator.cpp post_processor.cpp text_renderer.cpp pipe_server.cpp)
set(SOURCES glad.c main.cpp game.cpp shader.cpp texture.cpp resource_manager.cpp
            sprite_renderer.cpp game_object.cpp game_level.cpp particle_generator.cpp
            post_processor.cpp text_renderer.cpp pipe_server.cpp)
add_executable(Parkout ${SOURCES})

set(GLFW_LINK D:/Software/GLFW-tdm/lib/libglfw3.a)
set(IRRKLANG_LINK D:/Software/irrKlang/lib/Win32-gcc/libirrKlang.a)
set(FREETYPE_LINK D:/Software/freetype-tdm/lib/libfreetype.a)
target_link_libraries(Parkout ${GLFW_LINK} ${IRRKLANG_LINK} ${FREETYPE_LINK})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
