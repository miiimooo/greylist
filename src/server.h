/**   $Id: server.h 64 2010-08-12 21:58:53Z mimo $   **/
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

#ifndef SERVER_H
#define SERVER_H

#include <stdexcept>
#include <iostream>
#include <map>
#include <algorithm>
#include <errno.h>
#include <syslog.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
// #include <boost/threadpool.hpp>
#include <boost/detail/atomic_count.hpp>
#include "defensive.h"

#include "triplet.h"
#include "filedescriptor.h"
#include "tokens.h"
#include "postfixreader.h"
#include "parser.h"
#include "core.h"
#include "action.h"
#include "simplescheduler.h"
#include "cacherefresh.h"

using namespace boost;
// using namespace boost::threadpool;
using namespace std;

class Server  : public enable_shared_from_this< Server > , private noncopyable

{
  unsigned _monitorRefresh;
  unsigned _cacheRefresh;
  unsigned _maxRequests;
  unsigned _statsevery;
  typedef boost::function0<void>FunctionType;
  typedef SimpleScheduler<FunctionType> SchedulerType;
  SchedulerType _scheduler;
  volatile sig_atomic_t & _signal;
  
  shared_ptr<FileDescriptor> _sockfd;

  shared_ptr<Core> _core;
  Action _action;
  
  typedef map<unsigned, bool>SocketContainer;
  SocketContainer _sockets;
  boost::mutex _socketsLock;
  boost::detail::atomic_count _busyHandlers, _handled;
  boost::mutex _maxLock;
  unsigned _maxBusyHandlers;
//   class ClientSocket;
  class TripletWriter;
  class SocketReader;
  class ScoketWrapper;
public:
  
  Server(volatile sig_atomic_t & sig, shared_ptr<FileDescriptor> sockfd, 
	  shared_ptr<Core> core, unsigned spare, unsigned max,
	  unsigned mr, unsigned cacherefresh, unsigned maxr, bool deferwithstatus, 
	  const std::string &defer, const std::string & dunno, 
	  const std::string & onerror, unsigned loglevel, unsigned stats) 
    : _signal(sig), _sockfd(sockfd), _core(core), 
      _action(deferwithstatus, defer, dunno, onerror, loglevel),
      _monitorRefresh(mr), _cacheRefresh(cacherefresh),
      _maxRequests(maxr), _statsevery(stats),
      _scheduler(spare, max), 
      _busyHandlers(0), _handled(0),
      _maxBusyHandlers(0) { }
  
  ~Server() { tracer << "server destroyed" << std::endl;}
  
  int run() {
    boost::thread thrMonitor( boost::bind(&Server::monitor, this));
//     thrMonitor.detach();
    using namespace boost::posix_time;
    boost::posix_time::ptime lastWarning(boost::posix_time::min_date_time); 
    int socketerror = 0;
    while(true) {
      int fd = ::accept(_sockfd->get(), 0, 0);
      int error = (fd == -1 ? (errno != 0 ? errno : -1) : 0);
      if (error != 0) {
	socketerror = error;
	break;
      } else {
	if(!_scheduler.schedule(boost::bind(&Server::handle, this->shared_from_this(), fd))) {
	  ::close(fd);
	  if ( (second_clock::local_time() - lastWarning) > minutes(1) ) { // stop the logging from going mad
	    syslog(LOG_WARNING, "Warning: scheduler busy, try increasing 'maxthreads'.");
	    lastWarning = second_clock::local_time();
	  }
	}
      }
    }
//     std::cout << "Got socketerror " << socketerror << std::endl;  // EINVAL
    thrMonitor.interrupt();
    _sockfd->close();
    _scheduler.shutdown();
    {
      boost::mutex::scoped_lock lock(_socketsLock);
      std::for_each(_sockets.begin(), _sockets.end(), &Server::shutdownSocket);
    }
    while(_busyHandlers > 0) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
    _scheduler.wait();
    if (thrMonitor.joinable()) {
      thrMonitor.join();
    }
    return socketerror;
  }
  void handle(int clientfd) {
    SocketWrapper socketWrapper(this->shared_from_this(), clientfd);
    ++_busyHandlers;
    SocketReader pfReader(clientfd);
    unsigned handled = 0;
    Parser<SocketReader, TripletWriter> pfParser(Tokens::instance().postfixTokens());
    while(!pfReader.isEof()) {
      if (boost::this_thread::interruption_requested()) {
	break;
      }
      try {
	TripletWriter tripletWriter;
	try {
	  pfParser.parse(pfReader, tripletWriter);
	  if (pfReader.isEof()) {
	    break;
	  }
	} catch(std::exception &e) { // basically we didn't get a triplet
	  ::syslog(LOG_ERR, "Error: getting triplet input: %s", e.what());
	  ::shutdown(clientfd, SHUT_RDWR);
	  break;
	}
	Triplet triplet(tripletWriter.member(Tokens::client_address), 
	    tripletWriter.member(Tokens::sender), tripletWriter.member(Tokens::recipient), 
	    tripletWriter.member(Tokens::reverse_client_name), tripletWriter.member(Tokens::client_name));
	Core::Extra extra;
	std::string strAction;
	try {
	  Core::ActionType result = _core->process(triplet, extra);
	  strAction = _action.process(result, triplet, extra); 
	} catch(std::exception &e) { // problem in core
	  extra.setComment("Error: caught exception in core: " + std::string(e.what()));
	  strAction = _action.process(Core::error, triplet, extra);
	}
	ssize_t nwritten;
	if ( (nwritten = ::write(clientfd, strAction.c_str(), strAction.length())) <= 0) {
	  int error = errno;
	  if (!_signal) {
	    std::string errstr = Utils::getErrmsg(error);
	    ::syslog(LOG_ERR, "Error: in write: %s (%d)", errstr.c_str(), error);
	  }
	  ::shutdown(clientfd, SHUT_RDWR);
	  break;
	}
	// TODO compare nwritten with strAction.length()
	++_handled;
	handled++;
	if ((_maxRequests > 0) && (handled == _maxRequests)) {
	  break;
	}
      } catch(std::exception &e) { // basically we didn't get a triplet
	::syslog(LOG_ERR, "Error: in handler: %s", e.what());
	::shutdown(clientfd, SHUT_RDWR);
	break;
      }
    }
    --_busyHandlers;
  }
//   friend class Handler;
  void monitor() {
    using namespace boost::posix_time;
    ptime lastCacheRefresh = second_clock::local_time();
    unsigned statscounter = 0;
    while ((_signal != SIGQUIT) && (_signal != SIGHUP) 
	    && (!boost::this_thread::interruption_requested())) {
      if ((_signal == SIGUSR2 ) 
	|| ((_cacheRefresh > 0) 
	&& (second_clock::local_time() > (lastCacheRefresh + seconds(_cacheRefresh))))) {
	::syslog(LOG_INFO, "reloading caches");
	CacheRefresh::Proxy::instance().get()->run();
	if (_signal == SIGUSR2) {
	  _signal = 0;
	}
	lastCacheRefresh = second_clock::local_time();
      }
      boost::this_thread::sleep(boost::posix_time::seconds(_monitorRefresh));
      if ((_statsevery > 0 ) && (++statscounter == _statsevery)) {
	unsigned busyHandlers = _busyHandlers;
	stats();
	statscounter = 0;
      }
    }
    if (_signal) {
//       std::cout << "signal " << _signal << std::endl;
      ::shutdown(_sockfd->get(), SHUT_RDWR);
    }
  }
  void stats() {
    unsigned busyHandlers = _busyHandlers;
    static long long lasthandled = 0;
    unsigned numhandled = _handled-lasthandled;
    float reqs = float(numhandled)/float(_monitorRefresh * _statsevery);
    ::syslog(LOG_INFO, "stats: busy=%d active=%d waiting=%d, handled=%d r/s=%f",
	      busyHandlers, _scheduler.active(), _scheduler.waiting(), numhandled, reqs);
    lasthandled = _handled;	      
//     std::cout << "busyHandlers=" << _busyHandlers 
//       << " pool size=" << _threadpool.size()  << std::endl;
  }

private:
  class SocketWrapper {
    shared_ptr<Server> _server;
    unsigned _clientfd;
    bool _inserted;
  public:
    SocketWrapper(shared_ptr<Server> server, unsigned clientfd) : _server(server), _clientfd(clientfd) {
      boost::mutex::scoped_lock lock(_server->_socketsLock);
      _inserted = _server->_sockets.insert(SocketContainer::value_type(clientfd, true)).second;
      if (!_inserted) {
	syslog(LOG_ERR, "Error: socket still in use %d", clientfd);
      }
      tracer << "opened socket handler " << _clientfd << std::endl;
    }
    ~SocketWrapper() {
      tracer << "closing socket handler " << _clientfd << std::endl;
      if (_inserted) {
	boost::mutex::scoped_lock lock(_server->_socketsLock);
	_server->_sockets.erase(_clientfd);
      }
      tracer << "closed socket handler " << _clientfd << std::endl;
    }
  };
  class TripletWriter {
    std::map<unsigned, std::string> _map;
    const std::string _strNull;
  public:
    TripletWriter() : _strNull() { }
    void add(unsigned idx, const std::string & str) {
      _map.insert(std::map<unsigned, std::string>::value_type(idx, str));
    }
    void add(const std::string & key, const std::string & str) { /*unknown token*/ }
    const std::string & member(Tokens::PostfixTokensType t) const {
      std::map<unsigned, std::string>::const_iterator itr = _map.find(t);
      return (itr == _map.end() ? _strNull : (*itr).second);
    }
  };
  
  class SocketReader
  {
    int _fd;
    bool _eof;
    int _errno;
  public:
    SocketReader(int  fd) : _fd(fd), _eof(false) { }
    ~SocketReader() {
      if (_fd >= 0) {
	int res = ::close(_fd);
	_errno = (res == 0 ? 0 : errno);
	_fd = -1; 
      }
    }
    bool isEof() const { return _eof; }
    std::string::size_type getline(std::string &str) {
      if (!str.empty()) {
	str.clear();
      }
      ssize_t n = 0, rc;
      char c;
      while( (rc = read(_fd, &c, 1)) == 1) {
	if (c == '\n') {
	  if (n == 0) {
	    return std::string::npos;
	  } else {
	    return n;
	  }
	}
	str += c;
	n += rc;
      }
      _eof = true;
      if (rc < 0) {
	_errno = errno;
// 	_fd->setErrno(errno);
      }
      if (n == 0) {
	return std::string::npos;
      }
      return n;
    }
  };
  static void shutdownSocket(SocketContainer::value_type &s) {
//     if (s.second) {
      ::shutdown(s.first, SHUT_RDWR);
//     }
  }
};

#endif // SERVER_H
