/**   $Id: pftests.cpp 28 2009-09-17 01:22:44Z mimo $   **/


//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include "../TestSuite/Suite.h"
#include <string>
#include <cstddef>
#include <memory>

// c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <deque>

// /c
// #include <gc/leak_detector.h>

// #include "zthread/Thread.h"
// #include "zthread/ThreadedExecutor.h"
// #include "zthread/AtomicCount.h"
// #include "zthread/Mutex.h"
// #include "zthread/Guard.h"
// #include "zthread/Condition.h"

#include "defensive.h"
#include "filedescriptor.h"
// #include "blockingaccept.h"
// #include "blockingacceptnoqueue.h"
#include "tcplistener.h"
// #include "prethreadedserver.h"
#include "singleacceptthreadpool.h"
#include "requesthandler.h"
#include "tokenlist.h"
#include "parser.h"
#include "tokens.h"
#include "postfixreader.h"

#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr.hpp>

#include "sharedcache.h"

using namespace std;
using namespace TestSuite;
using namespace boost::threadpool;
// using namespace Greylist;


class PfRequestHandler : public RequestHandler 
{
  boost::shared_ptr<FileDescriptor> _fd;
  //   unsigned _requests;
  //   bool _busy;
  RequestCallback *_callback;
public:
  class TestWriter {
    vector<string> _strList;
    const TokenList* _tokenlist;
    public:
      TestWriter() : _strList(), _tokenlist(0) { }
      TestWriter(const TokenList *t) : _strList(), _tokenlist(t) { }
      void add(const std::string &, const std::string &val) { }
      void add(unsigned i, const std::string &val) {
	_strList.push_back(val);
	if (_tokenlist) {
	  cout << _tokenlist->getToken(i) << " ";
	}
	cout << val << endl;
      }
      size_t size() const { return _strList.size(); }
      void clear() { _strList.clear(); }
  };
  //   ~TestRequestHandler() { std::cout << "RequestHandler destroyed" << std::endl; }
  PfRequestHandler() : _fd() { }
  PfRequestHandler(boost::shared_ptr<FileDescriptor> fd, RequestCallback *cb) : _fd(fd), _callback(cb) { }
//   void setFileDescriptor(FileDescriptor *fd) { _fd = fd; } 
  virtual void run () {
    char line[255];
    _callback->addHandler(this);
    if(_fd->getErrno()) {
      cout << "got errno " << _fd->getErrno() 
      << " error: " << strerror_r(_fd->getErrno(), line, 255) << endl;
    }
//     TestWriter pfwriter(Tokens::instance().postfixTokens());
    TestWriter pfwriter(Tokens::instance().postfixTokens());
    PostfixReader<boost::shared_ptr<FileDescriptor> > pfReader(_fd);
    unsigned handled = 0;
    Parser<PostfixReader<boost::shared_ptr<FileDescriptor> >, TestWriter> pfParser(Tokens::instance().postfixTokens());
    SharedStringCache shared;
    while(!pfReader.isEof()) {
      pfParser.parse(pfReader, pfwriter);
      if (pfReader.isEof()) {
	break;
      }
      if(_fd->getErrno()) {
	cout << "got errno " << _fd->getErrno() 
	<< " error: " << strerror_r(_fd->getErrno(), line, 255) << endl;
      }
      if (handled%50 == 0) {
	SharedCache::WriterLock wtlock(&shared);
	shared._text = "shared memory is " + Utils::StrmConvert(handled);
      }
      
      SharedCache::ReaderLock rdlock(&shared);
      cout << shared._text << endl;
      string action = "action=dunno\n\n";
      ssize_t nwritten;
      if ( (nwritten = write(_fd->get(), action.c_str(), action.length())) <= 0) {
	perror("write failed: ");
      }
      cout << "handled " << ++handled << " requests so far" << endl;
      
    }
    //   ssize_t Readline(int fd, void *ptr, size_t maxlen)

    _fd->close();
    if(_fd->getErrno()) {
      cout << "got errno " << _fd->getErrno() 
      << " error: " << strerror_r(_fd->getErrno(), line, 255) << endl;
    }
//     Utils::zap(_fd);
    _callback->removeHandler(this);
  }
  
};
    

class PfTests : public Test {
  public:
    void run() {
	//       FileDescriptor stdin(0);
      boost::shared_ptr<TCPListener> listener (new TCPListener("", "4096") );
//       test_(listener != 0);
      SingleAcceptThreadPool<PfRequestHandler>* server = 
	new SingleAcceptThreadPool<PfRequestHandler>(listener, 50, 10, 100, 5000);
//       Thread t(server);
//       t.wait();
      server->run();
      cout << "server finished"<<endl;
      delete server;
//       Task t(&server);
    }
};

int main(int argc, char *argv[]) {
  Suite suite("Server Tests");
//   suite.addTest(new PoolTest(atoi(argv[1])));
//   suite.addTest(new ParamListTest);
  suite.addTest(new PfTests);
  //   suite.addTest(new SelectTest);
  //   suite.addTest(new Chatserver);
  suite.run();
  long nFail = suite.report();
  suite.free();
//   CHECK_LEAKS();
  return nFail;
}
