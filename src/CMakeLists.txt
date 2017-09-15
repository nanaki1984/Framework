if(NOT EMSCRIPTEN)
	if(MACOS)
		find_library(COCOA NAMES Cocoa)
		find_library(OGL NAMES OpenGL)
		find_library(IOKIT NAMES IOKit)

		set(SYS_LIBS ${SYS_LIBS} ${COCOA} ${OGL} ${IOKIT} PARENT_SCOPE)
	elseif(WINDOWS)
		find_package(OpenGL)

		set(SYS_LIBS ${SYS_LIBS} ${OPENGL_LIBRARY} PARENT_SCOPE)
	endif()
endif()

set(SUB_DIRS Components Core Game Managers Math Render)

foreach(DIR ${SUB_DIRS})
	add_subdirectory(${DIR})
	set(LIBS ${LIBS} lib${DIR})
endforeach()

set(LIBS ${LIBS} PARENT_SCOPE)
