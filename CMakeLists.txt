cmake_minimum_required(VERSION 3.20)

set(SOURCES
	main.cpp
	spng/spng.h
	spng/spng.c
	miniz/miniz.h
	miniz/miniz.c
)	

project(sdl_test)

add_executable(sdl_test
	${SOURCES}
)

set_target_properties(sdl_test PROPERTIES
	CXX_STANDARD 23
)

set(SDL_DIR C:/libs/SDL3-3.2.16/)

target_include_directories(sdl_test PUBLIC
	./
	${SDL_DIR}/include
	./miniz/
)

target_link_directories(sdl_test PUBLIC
	# ${SDL_DIR}/lib/arm64
	${SDL_DIR}/lib/x64
)

target_link_libraries(sdl_test PUBLIC
	SDL3.lib
)

add_definitions(-D SPNG_USE_MINIZ)

if(MSVC)
	add_definitions("/MP")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT sdl_test)
	#set_target_properties(sdl_test PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${SDL_DIR}/lib/arm64")
	set_target_properties(sdl_test PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${SDL_DIR}/lib/x64")
endif()
