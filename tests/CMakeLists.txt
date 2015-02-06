# $Id: CMakeLists.txt 64 2010-08-12 21:58:53Z mimo $
#

# Just process the sub directories
# set(greylist_SRCS main.cpp greylist.cpp)
set(dbiwrapped_PATH ../src/dbiwrapped-0.0.1)
set(dbiwrapped_INCLUDE_DIRS ${dbiwrapped_PATH})

# set(sockets_PATH ../src/Sockets-2.3.5)
# set(sockets_INCLUDE_DIRS ${sockets_PATH})

set(testsuite_PATH ../TestSuite)
set(testsuite_INCLUDE_DIRS ${testsuite_PATH})

set(greylist_PATH ../src)
set(greylist_INCLUDE_DIRS ${greylist_PATH})

set(zthread_PATH ../lib/zthread)
set(zthread_INCLUDE_DIRS ${zthread_PATH}/include)

set(threadpool_PATH ../lib/threadpool)
set(threadpool_INCLUDE_PATH ${threadpool_PATH})
# add_subdirectory(${sockets_PATH})

# add_subdirectory(${dbiwrapped_PATH})
#add_subdirectory(greylist)
# The above could also have been done as: subdirs(libyo app)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${dbiwrapped_INCLUDE_DIRS} 
  ${testsuite_INCLUDE_DIRS} ${greylist_INCLUDE_DIRS} 
  ${zthread_INCLUDE_DIRS} ${threadpool_INCLUDE_PATH} ../src)
  
add_executable(dbi_mysql dbi_mysql.cpp)
target_link_libraries(dbi_mysql dbiwrapped)

##add_executable(query_quote_escape query_quote_escape.cpp)
##target_link_libraries(query_quote_escape dbiwrapped)

# add_executable(sockets_server sockets_server.cpp)
# target_link_libraries(sockets_server sockets)

# add_executable(simplethreads simplethreads.cpp)
# target_link_libraries(simplethreads sockets dbiwrapped)

##add_executable(simpletestsuite simpletestsuite.cpp)
##target_link_libraries(simpletestsuite testsuite)

# add_executable(stlthreads stlthreads.cpp)
# target_link_libraries(stlthreads sockets testsuite)

# add_executable(thread_safe_logger thread_safe_logger.cpp ${dbiwrapped_PATH}/thread_safe_log.cpp)

# add_executable(log4cxx log4cxx.cpp)
# find_library(LOG_LIB log4cxx)
# target_link_libraries(log4cxx dbiwrapped sockets testsuite ${LOG_LIB})
# 
# add_executable(log4cpp log4cpp.cpp)
# find_library(LOG4CPP_LIB log4cpp)
# target_link_libraries(log4cpp dbiwrapped sockets testsuite ${LOG4CPP_LIB})

# add_executable(syslog syslog.cpp)
# target_link_libraries(syslog dbiwrapped testsuite)

# add_executable(coretests coretests.cpp ${greylist_PATH}/core.cpp)
# target_link_libraries(coretests testsuite)

# add_executable(rhserver server.c)
find_library(PTHREADS_LIB pthread)
# target_link_libraries(rhserver ${PTHREADS_LIB})

# set(zthread_PATH ../src/ZThread-2.3.2)

# add_custom_command(OUTPUT Makefile COMMAND ./configure)
# add_custom_command(OUTPUT lib.a COMMAND make DEPENDS Makefile)
# add_custom_command(OUTPUT ${prefix}/lib.a COMMAND cp lib.a ${prefix}/lib.a 
# DEPENDS lib.a)



# add_custom_command(
#   OUTPUT ${zthread_PATH}/Makefile 
#   COMMAND ${zthread_PATH}/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/zthread-build
# #   COMMAND CXXFLAGS=-fpermissive ${zthread_PATH}/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/zthread-build
# )
# add_custom_command(
#   OUTPUT ${zthread_PATH}/src/.libs/libZThread.a
#   COMMAND make && make install
#   DEPENDS ${zthread_PATH}/Makefile
# )
# add_custom_target(zthread_build DEPENDS ${zthread_PATH}/src/.libs/libZThread.a)
# 
# 
# 
# execute_process(
#   WORKING_DIRECTORY ${zthread_PATH}
#   COMMAND CXXFLAGS=-fpermissive ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/zthread && make && make install
#   RESULT_VARIABLE ret_var
# )
# IF(NOT "${ret_var}" STREQUAL 0)
#   MESSAGE(FATAL_ERROR "Unable to build zthreads. Error -> ${ret_var}")
# ENDIF(NOT "${ret_var}" STREQUAL 0)

# find_library(ZTHREAD_LIB zthread ${CMAKE_CURRENT_BINARY_DIR}/zthread)
# add_custom_target(
# find_library(GCCPP_LIB gccpp)
# add_executable(pooltest pooltest.cpp)
# target_link_libraries(pooltest testsuite ZThread ${PTHREADS_LIB} ${GCCPP_LIB})

# add_executable(prethreadedtests prethreadedtests.cpp ../src/tcplistener.cpp ../src/acceptor.cpp
# ../src/prethreadedserver.cpp ../src/blockingaccept.cpp ../src/requesthandler.cpp
# ../src/statistics.cpp ../src/blockingacceptnoqueue.cpp ../src/signals.cpp)
# target_link_libraries(prethreadedtests testsuite ZThread ${PTHREADS_LIB})
find_package( Boost 1.35 COMPONENTS thread)

# add_executable(servertests servertests.cpp ../src/tcplistener.cpp 
# ../src/requesthandler.cpp ../src/signals.cpp ../src/tokens.cpp)
# # target_link_libraries(servertests testsuite ZThread ${PTHREADS_LIB})
# target_link_libraries(servertests testsuite ${Boost_LIBRARIES})

##add_executable(pftests pftests.cpp ../src/tcplistener.cpp 
##../src/requesthandler.cpp ../src/signals.cpp ../src/tokens.cpp ../src/tokenlist.cpp)
# target_link_libraries(servertests testsuite ZThread ${PTHREADS_LIB})
##target_link_libraries(pftests testsuite ${Boost_LIBRARIES})

# add_executable(gltests gltests.cpp ../src/core.cpp
# ../src/greylist.cpp ../src/tokens.cpp ../src/ipaddress.cpp ../src/tokenlist.cpp)
# target_link_libraries(gltests testsuite dbiwrapped ${Boost_LIBRARIES})

##add_executable(cfgtests cfgtests.cpp ../src/core.cpp 
##../src/greylist.cpp ../src/tokens.cpp ../src/ipaddress.cpp
##../src/tcplistener.cpp ../src/signals.cpp ../src/system.cpp ../src/tokenlist.cpp)
##target_link_libraries(cfgtests testsuite dbiwrapped ${Boost_LIBRARIES})


##include_directories()
##add_executable(mergesort mergesort.cpp)
##target_link_libraries (mergesort ${Boost_LIBRARIES})

# include(CTest)
enable_testing()
##add_test(query_quote_escape query_quote_escape 10)
add_test(dbi_mysql dbi_mysql 20)
# add_test(sockets_server sockets_server 30)
# add_test(simplethreads simplethreads 40)
##add_test(simpletestsuite simpletestsuite)
##add_test(gltests gltests)
# add_test(stlthreads stlthreads)
# add_test(coretests coretests)
# add_test(log4cxx log4cxx)
# add_test(log4cpp log4cpp)
# add_test(thread_safe_logger thread_safe_logger)
# add_executable(greylist ${greylist_SRCS})
# add_executeable(greylist greylist.cpp main.cpp)
# target_link_libraries(greylist dbiwrapped)
