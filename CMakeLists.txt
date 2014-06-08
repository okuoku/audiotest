project(audiotest)
cmake_minimum_required(VERSION 2.8)

set(libs "c:/libs/portaudio-r1891-build")

set(libs_lib ${libs}/lib/x64/ReleaseMinDependency)
set(libs_inc ${libs}/include)

include_directories(${libs_inc})
link_directories(${libs_lib})

add_executable(audiotest
    audiotest.c)

target_link_libraries(audiotest
    portaudio_x64)


