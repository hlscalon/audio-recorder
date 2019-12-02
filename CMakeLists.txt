cmake_minimum_required(VERSION 3.14.5)

project (audio-recorder)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(audio-recorder ${SOURCES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(audio-recorder Threads::Threads)

#TODO: improve this hardcoded libraries
target_link_libraries(audio-recorder sox)
target_link_libraries(audio-recorder pulse)
target_link_libraries(audio-recorder pulse-simple)