# $Id: CMakeLists.txt 72 2011-08-10 21:39:36Z mimo $
cmake_minimum_required(VERSION 2.6)
#
# 
# Name the project
PROJECT(greylist)

# official release version
set(RELEASE_VERSION "3")

set(PACKAGE_NAME "greylist")
set(PACKAGE_MAJOR_VERSION "1")
set(PACKAGE_MINOR_VERSION "2")
set(PACKAGE_VERSION "${PACKAGE_MAJOR_VERSION}.${PACKAGE_MINOR_VERSION}")
set(PACKAGE_INFO "(C)Michael Moritz http://mimo.gn.apc.org/greylist")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

OPTION(GREYLIST_DEBUG 
"Build the project with tracing code on stdout"
OFF)
OPTION(DBI_DEBUG 
"Build the database library with tracing code on stdout"
OFF)

include(CheckIncludeFiles)

check_include_files("pthread.h" HAVE_PTHREAD_H)
# check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
# check_include_files("sys/wait.h" HAVE_SYS_WAIT_H)
# check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
# check_include_files("stdio.h" HAVE_STDIO_H)
# check_include_files("stdlib.h" HAVE_STDLIB_H)
# check_include_files("string.h" HAVE_STRING_H)
# check_include_files("unistd.h" HAVE_UNISTD_H)
# check_include_files("fcntl.h" HAVE_FCNTL_H)
# check_include_files("locale.h" HAVE_LOCALE_H)
# check_include_files("assert.h" HAVE_ASSERT_H)
# check_include_files("errno.h" HAVE_ERRNO_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Tell CMake to process the sub-directories
add_subdirectory(src)
# add_subdirectory(tests)
# add_subdirectory(TestSuite)
# add_subdirectory(lib)
# enable_testing()
# add_subdirectory(include)
install(PROGRAMS greylist DESTINATION etc/init.d/)
