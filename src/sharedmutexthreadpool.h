/**   $Id: core.h 7 2009-08-23 03:17:04Z mimo $   **/
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

#ifndef SINGLEACCEPTTHREADPOOL_H
#define SINGLEACCEPTTHREADPOOL_H
#include <stdexcept>
#include <set>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include "zthread/Thread.h"
#include "zthread/Mutex.h"
#include "zthread/Guard.h"
#include "zthread/PoolExecutor.h"
#include "zthread/ThreadedExecutor.h"
#include "zthread/AtomicCount.h"
#include "filedescriptor.h"
#include "clientsocket.h"

namespace Greylist {

using namespace std;
using namespace ZThread;
class RequestHandler : public Runnable {
};
class RequestCallback {
public:
  virtual void addHandler(RequestHandler *) = 0;
  virtual void removeHandler(RequestHandler *) = 0;
  virtual ~RequestCallback() { }
};
template <class HandlerClass, class ExecutorType=PoolExecutor>
class SingleAcceptThreadPool : public ZThread::Runnable, public RequestCallback
{
  
  FileDescriptor *_sockfd;
//   PoolExecutor _executor;
  ExecutorType _executor;
  unsigned _poolSize;
//   unsigned _poolSize, _minPoolSize, _maxPoolSize;
  set<RequestHandler*> _handlers;
  Mutex _handlersLock;
  class ClientSocket;
  class DynamicResizer;
  class HandlerWrapper;
  DynamicResizer _resizer;
public:
  SingleAcceptThreadPool(FileDescriptor *fd, unsigned poolSize,
			  unsigned minPoolSize, unsigned maxPoolSize,
			  unsigned sleep)
      : _sockfd(fd), _executor(poolSize), _poolSize(poolSize),/* _minPoolSize(minPoolSize),
      _maxPoolSize(maxPoolSize), */
      _resizer(_executor, minPoolSize, maxPoolSize, sleep) {
    if (!_sockfd) {
      throw runtime_error("FileDescriptor passed to SingleAcceptThreadPool is null");
    }
  }
  virtual ~SingleAcceptThreadPool() {
    cout << "destroying SingleAcceptThreadPool" << endl;
  }
  void run() {
//     _executor.size(_poolSize);
    _sockfd->open();
    if (_sockfd->getErrno()) { //TODO throw in open()
      throw runtime_error("error opening filedescriptor");
    }
//     Thread resizerThread(&_resizer);
    try {
      ThreadedExecutor myexecutor;
      Mutex sharedMutex;
      for(int i=0; i < _poolSize; i++) {
	myexecutor.execute(new HandlerWrapper(_sockfd, sharedMutex, this));
// 	Thread t(new HandlerWrapper(_sockfd, sharedMutex, this));
      }
//       while(!Thread::interrupted()) {
// 	int fd = ::accept(_sockfd->get(), 0, 0);
// 	int error = (fd == -1 ? errno : 0);
// 	ClientSocket* client = new ClientSocket(fd, error);
// 	HandlerClass *handler = new HandlerClass(new ClientSocket(fd, error), this);
// 	assert(handler);
// 	_executor.execute(handler);
	myexecutor.wait();
//       }
    } catch(ZThread::Interrupted_Exception&) { 
      std::cout << "Exiting BlockingAccept" << std::endl;
    }
    _sockfd->close();
    _executor.interrupt();
  }
  // Callback
  virtual void addHandler(RequestHandler *p) { }
  virtual void removeHandler(RequestHandler *p) { }
/*  virtual void addHandler(RequestHandler *p) {
    Guard<Mutex> g(_handlersLock);
    _handlers.insert(p);
    _resizer.addHandler();
  }
  virtual void removeHandler(RequestHandler *p) {
    Guard<Mutex> g(_handlersLock);
    _handlers.erase(p);
    _resizer.removeHandler();
  }*/
private:
  
  class HandlerWrapper : public Runnable {
    FileDescriptor *_sockfd;
    Mutex &_mutex;
    RequestCallback *_parent;
  public:
    HandlerWrapper(FileDescriptor *fd, Mutex &m, RequestCallback *p) 
      : _sockfd(fd), _mutex(m), _parent(p) { }
    void run() {
      try {
	while(!Thread::interrupted()) {
	  _mutex.acquire();
	  int fd = ::accept(_sockfd->get(), 0, 0);
	  int error = (fd == -1 ? errno : 0);
	  _mutex.release();
	  HandlerClass handler(new ClientSocket(fd, error), _parent);
// 	  cout << "serving request" <<endl;
	  handler.run();
// 	  delete nh;
// 	  cout << "done " << endl;
	}
	
      }catch(exception &e) {
	cerr << "exception in HandlerWrapper: " << e.what() << endl;
      }
    }
    
  };
  class ClientSocket : public FileDescriptor {
    int _error;
    public:
      ClientSocket(int fd, int e) : FileDescriptor(fd), _error(e) { }
      virtual ~ClientSocket() {
	if (FileDescriptor::get() != -1) {
	  throw logic_error("forgot to close client socket");
	}
      }
      void close() {
	int res = ::close(FileDescriptor::get());
	_error = (res == 0 ? 0 : errno);
	FileDescriptor::set(-1); 
      }
      virtual int getErrno() const { return _error; }
      
  };
  class DynamicResizer : public Runnable {
    ExecutorType &_executor;
    unsigned _defMinHandlers, _defMaxHandlers;
    unsigned _sleep;
    unsigned _busyHandlers, _maxBusyHandlers;    
    unsigned long _countHandlers;
    unsigned _allTimeMaxBusyHandlers;
    Mutex _counterLock;
  public:
    DynamicResizer(ExecutorType &ex, unsigned min, unsigned max, unsigned sleep) 
      : _executor(ex), _defMinHandlers(min), 
      _defMaxHandlers(max), _sleep(sleep), _busyHandlers(0), _maxBusyHandlers(0), 
      _countHandlers(0), _allTimeMaxBusyHandlers(0) {
    }
    void addHandler() {
      Guard<Mutex> g(_counterLock);
      _busyHandlers++;
      _maxBusyHandlers = max(_busyHandlers, _maxBusyHandlers);
      _countHandlers++;
    }
    void removeHandler() {
      Guard<Mutex> g(_counterLock);
      _busyHandlers--;
    }
    void run() {
      unsigned lastMaxBusyHandlers = _maxBusyHandlers;
      unsigned currenMaxBusyHandlers = _maxBusyHandlers;
      unsigned lastsize = _executor.size();
      while(!Thread::interrupted()) {
	Thread::sleep(_sleep);
	_counterLock.acquire();
	currenMaxBusyHandlers = _maxBusyHandlers;
	_maxBusyHandlers = 0;
	_counterLock.release();
	cout << "sizes: last " << lastMaxBusyHandlers << " current " << currenMaxBusyHandlers << " last " << lastsize<< endl;
	if (lastMaxBusyHandlers == currenMaxBusyHandlers) {
	  continue;
	}
	unsigned newsize = 0;
	if (currenMaxBusyHandlers > ((lastsize * 8) / 10)) {
	  newsize = min(_defMaxHandlers, lastsize + (lastsize*8)/10);
	} else if (currenMaxBusyHandlers < ((lastsize * 5)/10)) {
	  newsize = max(_defMinHandlers, lastsize - (lastsize*5)/10);
	}
	if (newsize == 0) {
	  continue;
	}
// 	unsigned newsize = (currenMaxBusyHandlers > 0) ? 
// 	  currenMaxBusyHandlers + ( (currenMaxBusyHandlers * 2) /10)
// 	  : _defMinHandlers;

// 	newsize = (currenMaxBusyHandlers > ((lastsize * 8) / 10)) ? 
// 	  min(_defMaxHandlers, newsize) : max(_defMinHandlers, newsize);
	lastMaxBusyHandlers = currenMaxBusyHandlers;
	if (newsize == lastsize) {
	  continue;
	}
	_allTimeMaxBusyHandlers = max(_maxBusyHandlers, _allTimeMaxBusyHandlers);
	cout << "resize now to " << newsize << endl;
	_executor.size(newsize);
	lastsize = newsize;
      }
    }
  };
};

}; // namespace Greylist
#endif // SINGLEACCEPTTHREADPOOL_H
