cmake_minimum_required(VERSION 3.14)

project(LETItGo)

enable_testing()

# Define module and include paths
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/")
set(LETITGO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Choose the build type (DEBUG or RELEASE)" FORCE)
endif()

# Compiler flags for each configuration
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -Wall -Wextra -Werror -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -D__RELEASE_MODE__=1")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

