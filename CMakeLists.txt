cmake_minimum_required(VERSION 3.5)
project(CopyingFile)
add_executable(copyingFile main.cpp)
set_target_properties(
    copyingFile PROPERTIES 
    CXX_STANDARD 14 
    CXX_STANDARD_REQUIRED ON
)