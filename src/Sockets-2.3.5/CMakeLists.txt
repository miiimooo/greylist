# $Id: CMakeLists.txt 4 2009-06-28 15:52:22Z mimo $

# Tell CMake to build a shared library from 
# yo.c
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_libraries(ssl crypto pthread)
set(sockets_SRCS Ajp13Socket.cpp AjpBaseSocket.cpp 
    Base64.cpp     Debug.cpp     Event.cpp     EventHandler.cpp     EventTime.cpp     Exception.cpp     File.cpp     FileStream.cpp     HTTPSocket.cpp     HttpBaseSocket.cpp     HttpClientSocket.cpp     HttpDebugSocket.cpp     HttpGetSocket.cpp     HttpPostSocket.cpp     HttpPutSocket.cpp     HttpRequest.cpp     HttpResponse.cpp     HttpTransaction.cpp     HttpdCookies.cpp     HttpdForm.cpp     HttpdSocket.cpp     IEventOwner.cpp     Ipv4Address.cpp     Ipv6Address.cpp     Lock.cpp     MemFile.cpp     Mutex.cpp     Parse.cpp     ResolvServer.cpp     ResolvSocket.cpp     SSLInitializer.cpp     SctpSocket.cpp     SmtpdSocket.cpp     Socket.cpp     SocketHandler.cpp     SocketStream.cpp     StdoutLog.cpp     StreamSocket.cpp     StreamWriter.cpp     TcpSocket.cpp     Thread.cpp     UdpSocket.cpp     Utility.cpp     socket_include.cpp     XmlDocument.cpp     XmlNode.cpp     XmlException.cpp
)
ADD_LIBRARY(sockets ${sockets_SRCS})

# add_custom_target(dbiwrapped.h 
# make sure the library gets installed
#install_targets(/lib yo)

