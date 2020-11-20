cmake_minimum_required(VERSION 3.17)
project(TREE_DED)

set(CMAKE_CXX_STANDARD 14)

add_executable(TREE_DED main.cpp cmake-build-debug/IsE_Tree.cpp cmake-build-debug/IsE_text.cpp cmake-build-debug/IsE_Stack.cpp
        cmake-build-debug/Akinator.cpp)