cmake_minimum_required(VERSION 3.20)

set(SOURCES
	main.cpp
)	

project(sdl_test)

add_executable(sdl_test
	${SOURCES}
)

set_target_properties(sdl_test PROPERTIES
	CXX_STANDARD 23
)

set(SDL_DIR C:/libs/SDL3-3.2.14/)

target_include_directories(sdl_test PUBLIC
	./
	${SDL_DIR}/include
)

target_link_directories(sdl_test PUBLIC
	${SDL_DIR}/lib/arm64
)

target_link_libraries(sdl_test PUBLIC
	SDL3.lib
)

if(MSVC)
	add_definitions("/MP")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT sdl_test)
	set_target_properties(sdl_test PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${SDL_DIR}/lib/arm64")
endif()
