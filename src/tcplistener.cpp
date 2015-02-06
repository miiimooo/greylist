/**   $Id: tcplistener.cpp 39 2009-09-22 23:25:30Z mimo $   **/
/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <errno.h>
#include <string.h>
// #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <assert.h>

#include "utils.h"
#include "./tcplistener.h"

using namespace std;

void TCPListener::open() {
  int listenfd, n;
  const int on = 1;
  struct addrinfo hints, *res, *ressave;
  const char *host = (_host != "" ? _host.c_str() : NULL);
  const char *serv = _port.c_str();
  
  assert(FileDescriptor::get()==-1);
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    stringstream estm;
    estm << "tcp_listen getaddrinfo error for " << _host << ":" 
      << _port << ": " << gai_strerror(n) << "(" << n << ")";
    throw runtime_error(estm.str()); 
  }
  int error = errno;
  ressave = res;
  std::string op = "getaddrinfo";
  do {
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd < 0) {
      error = errno;
      op = "listen";
      continue;		/* error, try next one */
    }
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
      op = "bind";
      error = errno;
      break;			/* success */
    }
    ::close(listenfd);	/* bind error, close and try next one */
  } while ( (res = res->ai_next) != NULL);
  
  if (res == NULL) {	/* errno from final socket() or bind() */
//     err_sys("tcp_listen error for %s, %s", host, serv);
    stringstream estm;
    estm << op << " error for " << _host << ":" 
    << _port << ": " << strerror(error) << "(" << error << ")";
    throw runtime_error(estm.str()); 
  }
  
  listen(listenfd, _backlog);
  
  _addrlen = res->ai_addrlen;	/* return size of protocol address */
    
  freeaddrinfo(ressave);
//   stringstream estm;
//   estm << "tcp_listen got socket " << listenfd << " for " << _host << ", " 
//   << _port << ": " << gai_strerror(errno);
//   throw runtime_error(estm.str());
  FileDescriptor::set(listenfd);
//   std::cout << "tcp_listen got socket " << listenfd << " for " << _host << ":" << _port  << std::endl;
}
void TCPListener::close() {
//   assert(FileDescriptor::get()!=-1);
  if(::close(FileDescriptor::get()) != 0) {
    cerr << "error closing TCP socket descriptor" << endl;
  }
}

