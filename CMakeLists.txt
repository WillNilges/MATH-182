cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MATH-182)

add_executable(triangle) # TODO: This engine should become a library eventually

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(triangle
  PRIVATE
    ${SRC_FILES}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB BUTTERFLIES_SOURCES_C ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(triangle glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(triangle OpenGL::GL -lm)

# Copy shaders over
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_DEST_DIR "${CMAKE_BINARY_DIR}")
file(COPY ${SHADER_SOURCE_DIR} DESTINATION ${SHADER_DEST_DIR})
