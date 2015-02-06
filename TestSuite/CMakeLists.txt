# $Id: CMakeLists.txt 10 2009-08-23 03:22:45Z mimo $

# Tell CMake to build a shared library from 
# yo.c
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# link_libraries(dbi)
set(testsuite_SRCS Test.cpp Suite.cpp)
ADD_LIBRARY(testsuite ${testsuite_SRCS})


