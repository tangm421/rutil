project(test)
cmake_minimum_required(VERSION 2.8)

set(HEADERS "./src/header.h")

set(SOURCES "./src/main.cpp")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
