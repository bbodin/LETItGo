cmake_minimum_required(VERSION 3.14.5)

project(LETItGo)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/")



SET(LETITGO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -D__RELEASE_MODE__=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -Wall -Wextra -Werror -g")


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

