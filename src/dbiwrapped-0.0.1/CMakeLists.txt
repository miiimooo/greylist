# $Id: CMakeLists.txt 53 2009-10-11 21:34:25Z mimo $

# Tell CMake to build a shared library from 
# yo.c
find_package( Boost 1.35 COMPONENTS thread)
find_library(PTHREADS_LIB pthread)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_libraries(dbi ${Boost_LIBRARIES} ${PTHREADS_LIB})
set(dbiwrapped_SRCS Database.cpp enum_t.cpp Query.cpp set_t.cpp StderrLog.cpp SysLog.cpp)
if(DBI_DEBUG)
  set_source_files_properties(${dbiwrapped_SRCS} COMPILE_FLAGS -DDEBUG)
endif(DBI_DEBUG)
  
ADD_CUSTOM_COMMAND(
  OUTPUT dbiwrapped.h
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/IError.h  
  ${CMAKE_CURRENT_SOURCE_DIR}/SysLog.h ${CMAKE_CURRENT_SOURCE_DIR}/enum_t.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/set_t.h ${CMAKE_CURRENT_SOURCE_DIR}/Database.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/StderrLog.h ${CMAKE_CURRENT_SOURCE_DIR}/Query.h 
  > ${CMAKE_CURRENT_SOURCE_DIR}/dbiwrapped.h
  DEPENDS IError.h StderrLog.h SysLog.h enum_t.h set_t.h Database.h Query.h
)
ADD_LIBRARY(dbiwrapped ${dbiwrapped_SRCS} dbiwrapped.h)

# add_custom_target(dbiwrapped.h 
# make sure the library gets installed
#install_targets(/lib yo)

