/**   $Id: blockingaccept.cpp 13 2009-08-31 22:55:52Z mimo $   **/
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
#include <deque>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
// #include <string.h>
#include <assert.h>
#include "filedescriptor.h"
#include "clientsocket.h"
#include "requesthandler.h"
#include "statistics.h"
#include "./blockingaccept.h"

using namespace std;

FileDescriptor *BlockingAccept::accept(const FileDescriptor* sockfd)
{
  /// TODO check this code!!
  socklen_t peer_addr_size = sockfd->getClientLength();
  struct sockaddr *peer_addr = reinterpret_cast<struct sockaddr *>(malloc(peer_addr_size));

  int fd = ::accept(sockfd->get(), peer_addr, &peer_addr_size);
  int error = (fd == -1 ? errno : 0);
  FileDescriptor *pfd = new ClientSocket(fd, peer_addr_size, peer_addr, error);
  _queue.put(pfd);
  return pfd;
}
void BlockingAccept::addBusyHandler(const ZThread::Runnable *) {
  ZThread::Guard<ZThread::Mutex> g(lock);
  _busyHandlers++;
}
void BlockingAccept::removeBusyHandler(const ZThread::Runnable *) {
  ZThread::Guard<ZThread::Mutex> g(lock);
  _busyHandlers--;
}

void BlockingAccept::run() {
  ZThread::Thread t(new Statistics(&_queue, &_busyHandlers));
  try {
    while(!ZThread::Thread::interrupted()) {
      FileDescriptor* fd = _queue.get();
      _executor.execute(new RequestHandler(fd, this));
    }
  } catch(ZThread::Interrupted_Exception&) { /* Exit */ }
  std::cout << "Exiting BlockingAccept" << std::endl;
}
  
