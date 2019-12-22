cmake_minimum_required(VERSION 3.12.4)

project (audio-recorder)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(audio-recorder ${SOURCES})

# pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(audio-recorder Threads::Threads)

# GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

add_definitions(${GTK4_CFLAGS_OTHER})

# Glib compile resources
add_custom_command(
	OUTPUT ui/resources.c
    DEPENDS ui/gui.gresource.xml ui/window.ui
    # TODO: correct this path
    COMMAND glib-compile-resources ../ui/gui.gresource.xml --sourcedir=../ui --target=../ui/resources.c --generate-source
    VERBATIM
)

add_custom_target(
	resources
	DEPENDS ui/resources.c
)

add_dependencies(audio-recorder resources)

# to generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C flags
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Werror -std=c11")

#TODO: improve this hardcoded libraries
target_link_libraries(audio-recorder sox)
target_link_libraries(audio-recorder pulse)
target_link_libraries(audio-recorder pulse-simple)
target_link_libraries(audio-recorder ${GTK4_LIBRARIES})
