cmake_minimum_required(VERSION 3.12.4)

add_compile_options("/std:c++latest")

project("Elpis")
set(TARGET "Elpis")

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

file(GLOB HEADERS "src/*/*/*.h")
file(GLOB SOURCES "src/*/*/*.cpp")
file(GLOB IMGUI_SOURCES "${IMGUI_DIR}/*.cpp")
set(STBI_SOURCES thirdparty/stb_image/stb_image.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
					${CMAKE_CURRENT_SOURCE_DIR}/src/Elpis/Include
					
                    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/include
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/include
					${IMGUI_DIR}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/lib/Release/x64
				 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/lib-vc2017
				 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/lib
)

add_definitions(-DRESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res")

add_executable(${TARGET} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${STBI_SOURCES})
target_precompile_headers(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/elpch.h)

target_link_libraries(${TARGET} glew32 glfw3 opengl32 assimp-vc141-mtd)

set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(OutDir))

add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll $(OutDir))
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/bin/assimp-vc141-mtd.dll $(OutDir))