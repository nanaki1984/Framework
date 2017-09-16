if(LINUX OR MACOS OR WINDOWS)
	add_definitions(-DGLEW_STATIC)

	include_directories("glew-2.1.0/include")

	add_library(GLEW STATIC glew-2.1.0/src/glew.c glew-2.1.0/include/GL/glew.h)

	set(SYS_LIBS ${SYS_LIBS} GLEW)
endif()

if(WIN_BACKEND STREQUAL GLFW)
	include_directories("glfw-2.7.7/include")
	include_directories("glfw-2.7.7/lib")

	FILE(GLOB GLFW_SRC glfw-2.7.7/lib/*.c)

	if(LINUX)
		include_directories("glfw-2.7.7/lib/x11")
		FILE(GLOB GLFW_PLATFORM_SRC glfw-2.7.7/lib/x11/*.c)
	elseif(MACOS)
		include_directories("glfw-2.7.7/lib/cocoa")
		FILE(GLOB GLFW_PLATFORM_SRC glfw-2.7.7/lib/cocoa/*.m)
	elseif(WINDOWS)
		include_directories("glfw-2.7.7/lib/win32")
		FILE(GLOB GLFW_PLATFORM_SRC glfw-2.7.7/lib/win32/*.c)
	endif()

	add_library(GLFW STATIC ${GLFW_SRC} ${GLFW_PLATFORM_SRC})

	set(SYS_LIBS ${SYS_LIBS} GLFW)
endif()

include_directories("imgui")

FILE(GLOB IMGUI_SRC imgui/*.cpp)

add_library(libImgui STATIC ${IMGUI_SRC})

set(SYS_LIBS ${SYS_LIBS} libImgui)

set(SYS_LIBS ${SYS_LIBS} PARENT_SCOPE)
