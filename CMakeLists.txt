cmake_minimum_required(VERSION 3.1)
project(FLOCK VERSION 1.0)

add_executable(Flock flock.cpp)
target_link_libraries(Flock pthread)
