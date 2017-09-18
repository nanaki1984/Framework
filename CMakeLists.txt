CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(framework)

# Determin the system to build for. Do that first
if(WIN32)
	if(NOT WINDOWS)
		set(WINDOWS TRUE)
		message("++ Building for windows")
	endif()
elseif(UNIX AND NOT APPLE)
	if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
		if(NOT ANDROID)
			set(LINUX TRUE)
			message("++ Building for Linux")
		else()
			message("++ Building for Android")
		endif()
	else()
		message(FATAL_ERROR "Unknown unix")
	endif()
elseif(APPLE)
	if(CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		set(MACOS TRUE)
		message("++ Building for MacOS")
	else()
		message(FATAL_ERROR "Unknown apple")
	endif()
else()
	message(FATAL_ERROR "Unknown system")
endif()

# Take a wild guess on the windowing system
if(LINUX)
	set(WIN_BACKEND "GLFW")
elseif(WINDOWS)
	set(WIN_BACKEND "GLFW")
elseif(ANDROID)
	set(WIN_BACKEND "ANDROID")
elseif(MACOS)
	set(WIN_BACKEND "GLFW")
else()
	message(FATAL_ERROR "Couldn't determine the window backend.")
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_HAS_EXCEPTIONS=0)

	string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	set(CXX_FLAGS "/GR- ")
	set(COMPILER_FLAGS "")
	set(LINKER_FLAGS "")
else()
	set(CXX_FLAGS "-Wno-warn-absolute-paths -std=c++11 -stdlib=libc++ -fno-exceptions -fno-rtti ")
	set(COMPILER_FLAGS "")
	set(LINKER_FLAGS "")
endif()

# IDE/Platform related settings
if(MACOS)
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

# SSE
if(MSVC)
	# defaults to SSE2
else()
	if(LINUX OR MACOS OR WINDOWS)
		set(COMPILER_FLAGS "${COMPILER_FLAGS} -msse4 ")
	else()
		set(COMPILER_FLAGS "${COMPILER_FLAGS} -mfpu=neon ")
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-D_DEBUG)

	if(MSVC)
		set(COMPILER_FLAGS "${COMPILER_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 ")
		set(LINKER_FLAGS "${LINKER_FLAGS} /debug /INCREMENTAL ")
	else()
		set(COMPILER_FLAGS "${COMPILER_FLAGS} -g3 -O0 ")
	endif()
else() # MinSizeRel, RelWithDebInfo or Release
	if(MACOS)
		add_definitions(-D_DEBUG)
	endif()

	if(MSVC)
		set(COMPILER_FLAGS "${COMPILER_FLAGS} /MD /O2 /Ob2 ")
		set(LINKER_FLAGS "${LINKER_FLAGS} /INCREMENTAL:NO ")
	else()
		# -flto ?
		set(COMPILER_FLAGS "${COMPILER_FLAGS} -ffast-math -O4 ")

		# Add this because Android compiler complains
		if(ANDROID)
			set(COMPILER_FLAGS "${COMPILER_FLAGS} -fno-data-sections ")
		endif()
	endif()
endif()

# Set the flags to cmake now
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")

set(SYS_LIBS "")
set(LIBS "")

include_directories("src" "ext/glfw-3.2.1/include" "ext/glew-2.1.0/include" "ext/imgui")

add_subdirectory(ext)
add_subdirectory(src)

# Temp
if(WINDOWS)
	add_definitions(-DGLEW_STATIC)

	add_executable(test1 main.cc TestRotate.cc TestRotate.h CamInput.cc CamInput.h ext/glew-2.1.0/src/glew.c)
else()
	add_executable(test1 main.cc TestRotate.cc TestRotate.h CamInput.cc CamInput.h)
endif()
target_link_libraries(test1 ${LIBS} ${SYS_LIBS})
