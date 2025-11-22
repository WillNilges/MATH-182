cmake_minimum_required(VERSION 3.23)

project(MATH-182)

add_executable(triangle) # TODO: This engine should become a library eventually

target_sources(triangle
  PRIVATE
    src/main.c
)
