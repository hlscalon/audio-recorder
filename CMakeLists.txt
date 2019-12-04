cmake_minimum_required(VERSION 3.12.4)

project (audio-recorder)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(audio-recorder ${SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(audio-recorder Threads::Threads)

set(CMAKE_CXX_FLAGS "-Wall -pedantic -Werror -std=c11")

#TODO: improve this hardcoded libraries
target_link_libraries(audio-recorder sox)
target_link_libraries(audio-recorder pulse)
target_link_libraries(audio-recorder pulse-simple)
