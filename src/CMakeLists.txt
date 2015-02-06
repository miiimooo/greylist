# $Id: CMakeLists.txt 71 2011-08-10 20:58:23Z mimo $
#
set(Boost_USE_STATIC_LIBS TRUE)

set(dbiwrapped_PATH dbiwrapped-0.0.1)
set(dbiwrapped_INCLUDE_DIRS ${dbiwrapped_PATH})

add_subdirectory(${dbiwrapped_PATH})

set(threadpool_PATH ../lib/threadpool)
set(threadpool_INCLUDE_PATH ${threadpool_PATH})

find_library(PTHREADS_LIB pthread)

# find_package( Boost 1.35 COMPONENTS thread)
find_package( Boost 1.40 COMPONENTS thread)

if (Boost_FOUND)
  message( "found boost version ${Boost_LIB_VERSION} in ${Boost_LIBRARY_DIRS}"  )
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${dbiwrapped_INCLUDE_DIRS} 
 ${threadpool_INCLUDE_PATH} )


# Just process the sub directories
set(greylist_SRCS main.cpp core.cpp greylist.cpp tokens.cpp ipaddress.cpp
tcplistener.cpp system.cpp tokenlist.cpp)

if(GREYLIST_DEBUG)
  set_source_files_properties(${greylist_SRCS} COMPILE_FLAGS -DDEBUG)
endif(GREYLIST_DEBUG)


# set(sockets_PATH Sockets-2.3.5)
# set(sockets_INCLUDE_DIRS ${sockets_PATH})

# add_subdirectory(${sockets_PATH})

#add_subdirectory(greylist)
# The above could also have been done as: subdirs(libyo app)
# include_directories(${CMAKE_CURRENT_BINARY_DIR} ${dbiwrapped_INCLUDE_DIRS} )
# add_executable(test_dbi_mysql tests/test_dbi_mysql.cpp)
# target_link_libraries(test_dbi_mysql dbiwrapped)

# include(CTest)
# enable_testing()
# add_test(test_dbi_mysql test_dbi_mysql)

add_executable(greylist ${greylist_SRCS})
# add_executeable(greylist greylist.cpp main.cpp)
# target_link_libraries(greylist dbiwrapped ${Boost_LIBRARIES})
target_link_libraries(greylist dbiwrapped ${Boost_LIBRARIES} ${PTHREADS_LIB})
install(TARGETS greylist RUNTIME DESTINATION bin)
# package its twin init.d script also:


# set(simple_server_SRCS ServerSocket.cpp Socket.cpp simple_server_main.cpp)
# add_executable(simple_server ${simple_server_SRCS})
