cmake_minimum_required(VERSION 3.7)
project(Lab_5)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -g -std=gnu11)

set(SOURCE_FILES libmyls.c libmyls.h myls.c)
add_executable(myls ${SOURCE_FILES})