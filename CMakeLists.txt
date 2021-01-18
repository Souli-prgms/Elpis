cmake_minimum_required(VERSION 3.12.4)

add_compile_options("/std:c++latest")

project("Elpis")
set(TARGET "Elpis")

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

# Using the "cmake_find_package" generator
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB HEADERS "src/*/*/*.h" "nut/*.h")
file(GLOB SOURCES "src/*/*/*.cpp" "nut/*.cpp")
file(GLOB IMGUI_SOURCES "${IMGUI_DIR}/*.cpp")
set(STB_SOURCES thirdparty/stb/stb_image.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/Elpis/Include ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

add_executable(${TARGET} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${STB_SOURCES})
target_precompile_headers(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/elpch.h)

target_link_libraries(${TARGET} GLEW::GLEW assimp::assimp Eigen3::Eigen3 glfw::glfw spdlog::spdlog)

set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(OutDir))

add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
add_definitions(-DRESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res")
