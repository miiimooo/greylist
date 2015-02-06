/**   $Id: singleacceptthreadpool.h 25 2009-09-09 20:36:04Z mimo $   **/
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
#include <map>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr.hpp>
#include "filedescriptor.h"
#include "clientsocket.h"


// namespace Greylist {

// using namespace std;
// using namespace ZThread;
class AcceptException : public std::exception {
  int _error;
public:
  AcceptException(int e) : _error(e) { }
  int get() const { return _error; }
};
class RequestHandler /*: public Runnable */{
};
class RequestCallback {
public:
  virtual void addHandler(RequestHandler *) = 0;
  virtual void removeHandler(RequestHandler *) = 0;
  virtual ~RequestCallback() { }
};
template <
  class HandlerClass, 
  class ExecutorType=boost::threadpool::pool
>
class SingleAcceptThreadPool : public RequestCallback {
  
  boost::shared_ptr<FileDescriptor> _sockfd;
//   PoolExecutor _executor;
  ExecutorType _executor;
  unsigned _poolSize;
//   unsigned _poolSize, _minPoolSize, _maxPoolSize;
  std::set<RequestHandler*> _handlers;
  boost::mutex _handlersLock;
  class ClientSocket;
  class Monitor;
  class HandlerWrapper;
  Monitor _monitor;
  typedef std::map<unsigned,boost::shared_ptr<ClientSocket> >SocketMapType;
  SocketMapType _sockets;
public:
  SingleAcceptThreadPool(boost::shared_ptr<FileDescriptor> fd, unsigned poolSize,
			  unsigned minPoolSize, unsigned maxPoolSize,
			  unsigned sleep)
      : _sockfd(fd), _executor(poolSize), _poolSize(poolSize),/* _minPoolSize(minPoolSize),
      _maxPoolSize(maxPoolSize), */
      _monitor(fd,_executor, minPoolSize, maxPoolSize, sleep) {
    if (!_sockfd) {
      throw std::runtime_error("FileDescriptor passed to SingleAcceptThreadPool is null");
    }
  }
  virtual ~SingleAcceptThreadPool() {
    std::cout << "destroying SingleAcceptThreadPool" << std::endl;
  }
  void run() {
//     _executor.size(_poolSize);
//     Signal_Handler::instance()->register_handler(SIGHUP, this);

    _sockfd->open();
    if (_sockfd->getErrno()) { //TODO throw in open()
      throw std::runtime_error("error opening filedescriptor");
    }
    boost::thread thrMonitor( boost::bind(&Monitor::run, &_monitor));
    try {
      while(true) {
	int fd = ::accept(_sockfd->get(), 0, 0);
	int error = (fd == -1 ? errno : 0);
	if (error != 0) {
	  throw AcceptException(error);
	}
	boost::shared_ptr<ClientSocket> clientSocket(new ClientSocket(fd, error));
	if (!error) {
	  _sockets.insert(typename SocketMapType::value_type(fd, clientSocket));
	}
	boost::shared_ptr<HandlerClass> handler(new HandlerClass(clientSocket, this));
	schedule(_executor, boost::bind(&HandlerClass::run, handler));
	
      }
    } catch(std::exception& e) { 
      std::cout << "Exiting BlockingAccept" << std::endl;
      thrMonitor.interrupt();
      _sockfd->close();
      for(typename SocketMapType::iterator itr = _sockets.begin(); itr != _sockets.end(); ++itr) {
	if ((*itr).second->isOpen()) {
	  shutdown((*itr).second->get(), SHUT_RDWR);
	}
      }
      _sockets.clear();
      _executor.wait();
      _executor.clear();
      if (thrMonitor.joinable()) {
	thrMonitor.join();
      }
      throw;
    }
    _executor.wait();
    _sockfd->close();
    _executor.clear();
  }
  // Callback
//   virtual void addHandler(RequestHandler *p) { }
//   virtual void removeHandler(RequestHandler *p) { }

  virtual void addHandler(RequestHandler *p) {
    boost::mutex::scoped_lock lock(_handlersLock);
    _handlers.insert(p);
    _monitor.addHandler();
  }
  virtual void removeHandler(RequestHandler *p) {
    boost::mutex::scoped_lock lock(_handlersLock);
    _handlers.erase(p);
    _monitor.removeHandler();
  }
private:
  
  class ClientSocket : public FileDescriptor {
    int _error;
    public:
      ClientSocket(int fd, int e) : FileDescriptor(fd), _error(e) { }
      virtual ~ClientSocket() {
	if (FileDescriptor::get() != -1) {
	  throw std::logic_error("forgot to close client socket");
	}
      }
      void close() {
	if (FileDescriptor::get() >= 0) {
	  int res = ::close(FileDescriptor::get());
	  _error = (res == 0 ? 0 : errno);
	  FileDescriptor::set(-1); 
	}
      }
      bool isOpen() const { return (FileDescriptor::get() >= 0); }
      virtual int getErrno() const { return _error; }
      
  };
  class Monitor {
    boost::shared_ptr<FileDescriptor> _sockfd;
    ExecutorType &_executor;
    unsigned _defMinHandlers, _defMaxHandlers;
    unsigned _sleep;
    unsigned _busyHandlers, _maxBusyHandlers;    
    unsigned long long _countHandlers;
    unsigned _allTimeMaxBusyHandlers;
    boost::mutex _counterLock;
  public:
    Monitor(  boost::shared_ptr<FileDescriptor> fd,
      ExecutorType &ex, unsigned min, unsigned max, unsigned sleep) 
      : _sockfd(fd), _executor(ex), _defMinHandlers(min), 
      _defMaxHandlers(max), _sleep(sleep), _busyHandlers(0), _maxBusyHandlers(0), 
      _countHandlers(0), _allTimeMaxBusyHandlers(0) {
    }
    void addHandler() {
      boost::mutex::scoped_lock lock(_counterLock);
      _busyHandlers++;
      _maxBusyHandlers = std::max(_busyHandlers, _maxBusyHandlers);
      _countHandlers++;
    }
    void removeHandler() {
      boost::mutex::scoped_lock lock(_counterLock);
      _busyHandlers--;
    }
    void output(unsigned lastMaxBusyHandlers, unsigned currenMaxBusyHandlers,
		unsigned lastsize, unsigned long long lastcountHandlers) {
      float reqs = (_countHandlers-lastcountHandlers)/(_sleep/1000.0);
      std::cout << "sizes: last " << lastMaxBusyHandlers << " current " 
	<< currenMaxBusyHandlers << " lastsize " << lastsize
	<< " count " << _countHandlers << " req/s " << reqs << std::endl;
    }
    void run() {
      unsigned long long lastcountHandlers;
      unsigned lastMaxBusyHandlers = _maxBusyHandlers;
      unsigned currenMaxBusyHandlers = _maxBusyHandlers;
      unsigned lastsize = _executor.size();
//       while(!boost::thread::interruption_requested()) {
      while(!boost::this_thread::interruption_requested()) {
// 	boost::xtime xt;
// 	boost::xtime_get(&xt, boost::TIME_UTC);
// 	xt.sec += (_sleep/1000);
	boost::this_thread::sleep(boost::posix_time::milliseconds(_sleep));
// 	_sockfd->close();
	shutdown(_sockfd->get(), SHUT_RDWR);
	_counterLock.lock();
	currenMaxBusyHandlers = _maxBusyHandlers;
	this->output(lastMaxBusyHandlers, currenMaxBusyHandlers, lastsize, lastcountHandlers);
	lastcountHandlers = _countHandlers;
	_maxBusyHandlers = 0;
	_counterLock.unlock();
	if (lastMaxBusyHandlers == currenMaxBusyHandlers) {
	  continue;
	}
	unsigned newsize = 0;
	if (currenMaxBusyHandlers > ((lastsize * 8) / 10)) {
	  newsize = std::min(_defMaxHandlers, lastsize + (lastsize*8)/10);
	} else if (currenMaxBusyHandlers < ((lastsize * 5)/10)) {
	  newsize = std::max(_defMinHandlers, lastsize - (lastsize*5)/10);
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
	_allTimeMaxBusyHandlers = std::max(_maxBusyHandlers, _allTimeMaxBusyHandlers);
	std::cout << "resize now to " << newsize << std::endl;
	_executor.size_controller().resize(newsize);
	lastsize = newsize;
      }
    }
  };
};

// }; // namespace Greylist
#endif // SINGLEACCEPTTHREADPOOL_H
