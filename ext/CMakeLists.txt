if(LINUX OR MACOS OR WINDOWS)
	add_definitions(-DGLEW_STATIC)

	include_directories("glew-2.1.0/include")

	add_library(GLEW STATIC glew-2.1.0/src/glew.c glew-2.1.0/include/GL/glew.h)

	set(SYS_LIBS ${SYS_LIBS} GLEW)
endif()

if(WIN_BACKEND STREQUAL GLFW)
	include_directories("glfw-3.2.1/include")

	add_subdirectory(glfw-3.2.1)

	set(SYS_LIBS ${SYS_LIBS} glfw ${GLFW_LIBRARIES})
endif()

include_directories("imgui")

FILE(GLOB IMGUI_SRC imgui/*.cpp)

add_library(libImgui STATIC ${IMGUI_SRC})

set(SYS_LIBS ${SYS_LIBS} libImgui)

set(SYS_LIBS ${SYS_LIBS} PARENT_SCOPE)
