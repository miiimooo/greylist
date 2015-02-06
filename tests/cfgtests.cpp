/**   $Id: cfgtests.cpp 36 2009-09-22 01:12:30Z mimo $   **/


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
// /*#include <boost/smart_ptr.hpp>*/

#include "core.h"
#include "greylist.h"
#include "dbdatasource.h"
#include "ipaddress.h"
#include "exactmatch.h"
// #include "cache.h"
#include "tmplcore.h"
#include "patternmatch.h"
#include "cachehandler.h"
#include "signals.h"
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <readwriteable.h>
#include "typedefs.h"
#include "cachedexactmatch.h"
#include "system.h"

using namespace std;
using namespace TestSuite;
using namespace boost::threadpool;
using namespace Templates;
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
      void add(const std::string &, const std::string &) { }
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
  ~PfRequestHandler() { std::cout << "RequestHandler destroyed" << std::endl; }
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
    while(!pfReader.isEof()) {
      pfParser.parse(pfReader, pfwriter);
      if (pfReader.isEof()) {
	break;
      }
      if(_fd->getErrno()) {
	cout << "got errno " << _fd->getErrno() 
	<< " error: " << strerror_r(_fd->getErrno(), line, 255) << endl;
      }
      
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
class CfgTests : public Test {
public:
//   struct ReloadableEngine : public Event_Handler {
//     virtual int handle_signal (int signum) {
//       cout << "received signal " << signum << endl;
//       kill(0, SIGHUP);
//     }
  struct ReloadableEngine {
    void run() {
      cout << "run!" << endl;
//       Signal_Handler::instance()->register_handler(SIGHUP, this);
      
      boost::shared_ptr<TCPListener> listener (new TCPListener("", "4096"));
      //       test_(listener != 0);
      boost::shared_ptr<SingleAcceptThreadPool<PfRequestHandler> >
	server ( 
	  new SingleAcceptThreadPool<PfRequestHandler>(listener, 50, 10, 100, 5000)
	);
      //       Thread t(server);
      //       t.wait();
      server->run();
      cout << "server finished"<<endl;
    }
    ReloadableEngine() { }
  };
  struct TestReader {
    vector<string> _lines;
    vector<string>::const_iterator _itr;
    TestReader(const vector<string> & vs) : _lines(vs), _itr(_lines.begin()) { } 
    int getline(std::string &line) {
      if (_itr == _lines.end()) {
	return -1;
      }
      line = (*_itr);
      std::cout << line << std::endl;
      int len = (*_itr).length();
      ++_itr;
      return len;
    }
    bool isEof() const { return (_itr == _lines.end()); }
    void reload() { _itr = _lines.begin(); }
    void push_back(const std::string & l) { _lines.push_back(l); }
  };
  void pr_action(Core::ActionType a) {
    cout << "Action=";
    switch(a) {
      case Core::dunno:
	cout << "dunno";
	break;
      case Core::defer:
	cout << "defer";
	break;
      case Core::cont:
	cout << "cont";
	break;
    }
    cout  << endl;
  }  
  void run() {
    vector<string> testcfg;
    testcfg.push_back("[database]");
    testcfg.push_back("name=greylistdb");
    testcfg.push_back("type=mysql");
    testcfg.push_back("host=localhost");
    testcfg.push_back("dbname=greylist");
    testcfg.push_back("username=root");
    testcfg.push_back("password=");

    testcfg.push_back("[datasource]");
    testcfg.push_back("type=db");
    testcfg.push_back("name=tripletds");
    testcfg.push_back("table=triplet");
    testcfg.push_back("database=greylistdb");

    testcfg.push_back("[datasource]");
    testcfg.push_back("type=db");
    testcfg.push_back("name=recipientds");
    testcfg.push_back("table=recipient");
    testcfg.push_back("database=greylistdb");
    
    testcfg.push_back("[match]");
    testcfg.push_back("name=recipient match");
    testcfg.push_back("type=cached exact match");
    testcfg.push_back("lookup=address");
    testcfg.push_back("match=recipient");
    testcfg.push_back("return=comment"); //?
    testcfg.push_back("datasource=recipientds");
    
    testcfg.push_back("[policy]");
    testcfg.push_back("type=whitelist");
    testcfg.push_back("match=recipient match");
//     testcfg.push_back("log=yes"); //?
//     testcfg.push_back("datasource=greylistdb");
    
    testcfg.push_back("[match]");
    testcfg.push_back("name=allmatch");
    testcfg.push_back("type=all");
    
    testcfg.push_back("[policy]");
    testcfg.push_back("type=greylist");
    testcfg.push_back("mode=reverse");
//     testcfg.push_back("table=triplet");
    testcfg.push_back("match=allmatch");
    testcfg.push_back("datasource=tripletds");
    testcfg.push_back("weakbytes=3");
    testcfg.push_back("timeout=60");

    testcfg.push_back("[server]");
//     testcfg.push_back("port=4096");
//     testcfg.push_back("listen=127.0.0.1");
    testcfg.push_back("threads=50");
    testcfg.push_back("minthreads=10");
    testcfg.push_back("maxthreads=110");
    testcfg.push_back("maxrequests=0");
    testcfg.push_back("cacherefresh=50"); // seconds
    testcfg.push_back("poolrefresh=5"); // seconds
//     testcfg.push_back("defertext=blabla");
   testcfg.push_back("log=full"); //full, none
    testcfg.push_back("deferwithstatus=true"); //full, none
    testcfg.push_back("defer=defer_if_permit Service is unavailable");
    testcfg.push_back("dunno=dunno");
    testcfg.push_back("onerror=dunno");
    
//     testcfg.push_back("[action]");
    
    TestReader reader(testcfg);
    
    System<TestReader> system;
    
    system.run(reader);
    return;
    Triplet triplet("216.145.54.171", "someuser@yahoo.com", "someuser@mydomain.com",
		     "mrout1.yahoo.com", "mrout1.yahoo.com");
    Triplet triplet1("216.145.54.171", "someuser@yahoo.com", "not-whitelisted@mydomain.com",
		      "mrout1.yahoo.com", "mrout1.yahoo.com");
      
//     Core::ActionType act = system.testCore(triplet);
//     pr_action(act);
//     act = system.testCore(triplet1);
//     pr_action(act);
    return;
    // install a signal handler for SIGHUP
//     struct sigaction sa;
//     sa.sa_handler = sighup_handler;
//     sigemptyset (&sa.sa_mask);
//     sa.sa_flags = 0;
//     sigaction (SIGHUP, &sa, 0);
    
    
    try {
      ReloadableEngine engine;
    
//       boost::thread thrd1(boost::bind(&ReloadableEngine::run, &engine));
    
//       thrd1.detach();
    
      cout << "Engine thread launched" << endl;
      engine.run();
//       while(true) {
// 	boost::this_thread::sleep(boost::posix_time::seconds(1)); 
// 	thrd1.interrupt();
// 	raise(SIGHUP);
//       }
    } catch(AcceptException &e) { // this has to be in the main thread
      cout << "caught signal " << e.get() << endl;
      throw;
    }
  }
};
      

int main(int argc, char *argv[]) {
  Suite suite("Greylist Tests");
//   suite.addTest(new PoolTest(atoi(argv[1])));
//   suite.addTest(new ParamListTest);
//   suite.addTest(new GlTests);
//   suite.addTest(new CacheTests);
  suite.addTest(new CfgTests);
//   suite.addTest(new SelectTest);
  //   suite.addTest(new Chatserver);
//   try {
    suite.run();
//   } catch (Signal &e) {
//     cout << "caught signal in main" << endl;
//   }
  long nFail = suite.report();
  suite.free();
//   CHECK_LEAKS();
  return nFail;
}
