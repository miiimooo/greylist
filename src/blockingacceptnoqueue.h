/**   $Id: blockingaccept.h 7 2009-08-23 03:17:04Z mimo $   **/
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

#ifndef BLOCKINGACCEPTNOQUEUE_H
#define BLOCKINGACCEPTNOQUEUE_H

#include <iostream>
#include "zthread/Thread.h"
#include "zthread/PoolExecutor.h"

#include <errno.h>
#include "filedescriptor.h"
#include "TQueue.h"
#include "acceptor.h"
#include "requesthandler.h"
#include "clientsocket.h"
#include "signals.h"

/** blocking accept with a queue that launches the threads from a pool**/
class BlockingAcceptNoQueue : public Acceptor, public Event_Handler
{
public:
  typedef ZThread::PoolExecutor ThreadExecutorType; 
  typedef TQueue<FileDescriptor *> ThreadQueueType;
protected:
  ThreadQueueType _queue;
  ThreadExecutorType _executor;
  unsigned _busyHandlers;
  unsigned _maxBusyHandlers;
//   unsigned _minBusyHandlers;
  unsigned _countHandlers;
  ZThread::Mutex _counterLock;
  unsigned _defMaxHandlers;
  unsigned _defMinHandlers;
  ZThread::Mutex _executorLock;
  unsigned _allTimeMaxBusyHandlers;
  class Worker : public ZThread::Runnable {
    const FileDescriptor *_fd;
    ZThread::Mutex &_acceptLock;
    Acceptor *_acceptor;
//     RequestHandler *_handler;
  public:
    Worker(const FileDescriptor *fd, ZThread::Mutex &al, Acceptor *ac/*RequestHandler ,
	    *r*/) 
      : _fd(fd), _acceptLock(al), _acceptor(ac)/*, _handler(r)*/ { }
    void run() {
      while(!ZThread::Thread::interrupted()) {
	_acceptLock.acquire();
	int fd = ::accept(_fd->get(), 0, 0);
	int error = (fd == -1 ? errno : 0);
	ClientSocket pfd(fd, 0, 0, error);
	/*FileDescriptor *pfd = new*/ 
// 	assert(pfd);
	_acceptLock.release();
	RequestHandler *p = new RequestHandler(&pfd, _acceptor);
	assert(p);
	p->run();
	delete p;
// 	delete pfd;
      }
    }
  };
public:
  
  BlockingAcceptNoQueue() : _queue(), _executor(50), _busyHandlers(0), 
    _maxBusyHandlers(0), /*_minBusyHandlers(100),*/ _countHandlers(0), _defMaxHandlers(100), 
    _defMinHandlers(10), _allTimeMaxBusyHandlers(0) { }
  // Acceptor
  virtual FileDescriptor* accept(const FileDescriptor* sockfd) {
    unsigned long size = _executor.size();
    for(int i=0; i<size;i++) {
      _executor.execute(new Worker(sockfd, _executorLock, this));
    }
    while(!ZThread::Thread::interrupted()) {
      // resize executor here
      ZThread::Thread::sleep(1000);
    }
    _executor.interrupt();
  }
  virtual void addBusyHandler(const ZThread::Runnable *);
  virtual void removeBusyHandler(const ZThread::Runnable *);
  // Event_Handler
  virtual int handle_signal (int signum);
  
  void add(FileDescriptor* fd) { _queue.put(fd); }
  virtual void run();
};

#endif // BLOCKINGACCEPT_H
