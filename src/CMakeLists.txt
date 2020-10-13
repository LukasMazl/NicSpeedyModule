cmake_minimum_required(VERSION 3.17)
project(python_test_module C)

set(CMAKE_C_STANDARD C99)
SET(PYTHON_LIBRARY "/usr/lib/python3.4/config-3.4m-x86_64-linux-gnu/libpython3.4m.so.1")
set(PYTHON_INCLUDE_DIRS "/usr/include/python2.7")

add_library(python_test_module my_module.c)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(_your_target_name_ ${PYTHON_LIBRARIES})