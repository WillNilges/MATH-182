cmake_minimum_required(VERSION 3.23)

project(MATH-182)

add_executable(triangle) # TODO: This engine should become a library eventually

target_sources(triangle
  PRIVATE
    src/glad.c
    src/main.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB BUTTERFLIES_SOURCES_C ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(triangle glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(triangle OpenGL::GL)

