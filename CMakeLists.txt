cmake_minimum_required(VERSION 3.12.4)

add_compile_options("/std:c++latest")

project("Elpis")
set(TARGET "Elpis")

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

set(HEADERS
	include/Renderer.h
	include/Mesh.h
	include/Shader.h
	include/Camera.h
	include/MeshLoader.h
	include/Light.h
	include/Texture.h
	include/CubeMap.h
	include/Scene.h
	include/Entity.h
	include/MaterialManager.h
)

set(SOURCES
	src/main.cpp
	src/Renderer.cpp
	src/Mesh.cpp
	src/Shader.cpp
	src/Camera.cpp
	src/MeshLoader.cpp
	src/Texture.cpp
	src/CubeMap.cpp
	src/Scene.cpp
	src/Entity.cpp
	src/MaterialManager.cpp
)

set(IMGUI_SOURCES 
	 ${IMGUI_DIR}/imgui_impl_glfw.cpp
	 ${IMGUI_DIR}/imgui_impl_opengl3.cpp 
	 ${IMGUI_DIR}/imgui.cpp 
	 ${IMGUI_DIR}/imgui_draw.cpp 
	 ${IMGUI_DIR}/imgui_demo.cpp 
	 ${IMGUI_DIR}/imgui_widgets.cpp
)

set(STBI_SOURCES
	thirdparty/stb_image/stb_image.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/eigen3
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/include
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb_image
					${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/include
					${IMGUI_DIR}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/lib/Release/x64
				 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/lib-vc2017
				 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/lib
)

add_executable(${TARGET} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${STBI_SOURCES})

target_link_libraries(${TARGET} glew32 glfw3 opengl32 assimp-vc141-mtd)

set(COPY_DIRECTORY_IF_DIFFERENT ${CMAKE_CURRENT_SOURCE_DIR}/update_directory.cmake)

set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(OutDir))

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll $(OutDir))
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/bin/assimp-vc141-mtd.dll $(OutDir))
#add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res $(OutDir)/res)

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND}
    -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/res"
    -Ddst_dir="$(OutDir)/res"
    -P "${COPY_DIRECTORY_IF_DIFFERENT}"
)