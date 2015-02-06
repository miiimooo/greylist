/**   $Id: blockingaccept.cpp 7 2009-08-23 03:17:04Z mimo $   **/
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
#include "signals.h"
#include "./blockingacceptnoqueue.h"

using namespace std;
using namespace ZThread;
/*
FileDescriptor *BlockingAcceptNoQueue::accept(const FileDescriptor* sockfd)
{
  assert(sockfd);
  /// TODO check this code!!
  socklen_t peer_addr_size = sockfd->getClientLength();
  struct sockaddr *peer_addr = reinterpret_cast<struct sockaddr *>(malloc(peer_addr_size));
  ZThread::Mutex acceptLock;
  
  assert(peer_addr);
  
  acceptLock.acquire();
  int fd = ::accept(sockfd->get(), peer_addr, &peer_addr_size);
  acceptLock.release();
  int error = (fd == -1 ? errno : 0);
  FileDescriptor *pfd = new ClientSocket(fd, peer_addr_size, peer_addr, error);
  RequestHandler *p = new RequestHandler(pfd, this);
  assert(p);
//   _executorLock.acquire();
  _executor.execute(p);
//   _executorLock.release();
  return pfd;
}
*/
void BlockingAcceptNoQueue::addBusyHandler(const ZThread::Runnable *) {
  ZThread::Guard<ZThread::Mutex> g(_counterLock);
  _busyHandlers++;
  _maxBusyHandlers = max(_busyHandlers, _maxBusyHandlers);
  _countHandlers++;
    
}
void BlockingAcceptNoQueue::removeBusyHandler(const ZThread::Runnable *) {
  ZThread::Guard<ZThread::Mutex> g(_counterLock);
  _busyHandlers--;
//   _minBusyHandlers = min(_busyHandlers, _minBusyHandlers);
}
int BlockingAcceptNoQueue::handle_signal (int signum) {
  cout << "caught signal " << signum << endl;
  cout << "count " << _countHandlers << " current max " << _maxBusyHandlers 
    << " all time max " << _allTimeMaxBusyHandlers << endl;
  exit(0); //not pretty;
}
void BlockingAcceptNoQueue::run() {
//   ZThread::Thread t(new Statistics(&_queue, &_busyHandlers));
  Signal_Handler::instance()->register_handler(SIGINT, this);
  Signal_Handler::instance()->register_handler(SIGTERM, this);
  Signal_Handler::instance()->register_handler(SIGQUIT, this);
  unsigned lastMaxBusyHandlers = _maxBusyHandlers;
  unsigned currenMaxBusyHandlers = _maxBusyHandlers;
//   unsigned lastMinBusyHandlers = _maxBusyHandlers;
  try {
    while(!ZThread::Thread::interrupted()) {
      // resize executor here
      Thread::sleep(500);
      continue;
      _counterLock.acquire();
      currenMaxBusyHandlers = _maxBusyHandlers;
      _counterLock.release();
      
      if ((lastMaxBusyHandlers != currenMaxBusyHandlers)
	/*|| (lastMinBusyHandlers != _minBusyHandlers)*/ ) {
	Guard<Mutex> g(_counterLock);
	cout << "last " << lastMaxBusyHandlers << " current " << currenMaxBusyHandlers;
      // 	lastMaxBusyHandlers = _maxBusyHandlers;
	unsigned oldsize = _executor.size();
	unsigned newsize = oldsize;
	newsize = (currenMaxBusyHandlers > 0) ? 
	  currenMaxBusyHandlers + ( (currenMaxBusyHandlers * 2) /10)
	  : _defMinHandlers;
	cout << " oldsize " << oldsize << " newsize " << newsize << endl;
	if (currenMaxBusyHandlers > ((oldsize * 8) / 10)) {
	  newsize = min(_defMaxHandlers, newsize);
	} else {
	  newsize = max(_defMinHandlers, newsize);
	}
	
	if (newsize != oldsize) {
// 	  Guard<Mutex> g(_executorLock);
	  _allTimeMaxBusyHandlers = max(_maxBusyHandlers, _allTimeMaxBusyHandlers);
	  _maxBusyHandlers = 0;
// 	  unsigned needed = _executor.size(0);
	  cout << "resize now to " << newsize << endl;
// 	  cout << "needed " << needed << endl;
	  _executor.size(newsize);
	}
      }
      lastMaxBusyHandlers = currenMaxBusyHandlers;
//       _counterLock.release();
    }
  } catch(ZThread::Interrupted_Exception&) { /* Exit */ }
  std::cout << "Exiting BlockingAcceptNoQueue" << std::endl;
}
  
