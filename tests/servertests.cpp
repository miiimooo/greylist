/**   $Id: servertests.cpp 20 2009-09-05 20:03:18Z mimo $   **/

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

#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr.hpp>

using namespace std;
using namespace TestSuite;
using namespace boost::threadpool;
// using namespace Greylist;

#define MAXDATASIZE 100 // max number of bytes we can get at once 
// class Runner : public Runnable {
//   unsigned _id;
// public:
//   Runner(int id) : _id(id) { }
//   void run() {
//     unsigned i = 0;
//     while(!Thread::interrupted()) {
//       i++;
//       Thread::sleep(1);
//       if (i%1000 == 1) {
// 	cout << "#" << _id << " i " << i << endl;
//       }
//     }
//   }
// };
class ParamListTest : public Test {
  public:
    class TestReader {
      unsigned _idx;
      vector<string> _strList;
    public:
	TestReader() : _idx(0), _strList() {
	_strList.push_back("bla");
	_strList.push_back("sometoken=first value");
	_strList.push_back("some_other_token=second value");
	_strList.push_back("somerequiredtoken=third value");
	}
      string::size_type getline(string &str) {
	str.clear();
	if (_idx < _strList.size()) {
	  str = _strList[_idx++];
	  return str.length();
	}
	return string::npos;
      }
      bool isEof() const {
	return (_idx == _strList.size());
      }
    };
    class TestWriter {
      vector<string> _strList;
      const TokenList* _tokenlist;
    public:
      TestWriter() : _strList(), _tokenlist(0) { }
      TestWriter(const TokenList *t) : _strList(), _tokenlist(t) { }
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
    class PostfixTestReader {
      unsigned _idx;
      vector<string> _strList;
      public:
	PostfixTestReader() : _idx(0), _strList() {
	  _strList.push_back("reverse_client_name=localhost");
	  _strList.push_back("sender=me@thisplace.com");
	  _strList.push_back("recipient=null@t61");
	  _strList.push_back("client_address=127.0.0.1");
	}
	string::size_type getline(string &str) {
	  str.clear();
	  if (_idx < _strList.size()) {
	    str = _strList[_idx++];
	    return str.length();
	  }
	  return string::npos;
	}
	bool isEof() const {
	  return (_idx == _strList.size());
	}
    };
    void run() {
      TokenList testList;
      testList.addToken("sometoken", false);
      testList.addToken("somerequiredtoken", true);
      
      TestReader reader;
      TestWriter writer;
      Parser<TestReader, TestWriter> myParser(&testList);
      myParser.parse(reader, writer);
      test_(writer.size() == 2);
      
      TestWriter pfwriter(Tokens::instance().postfixTokens());
      PostfixTestReader pfTestReader;
      Parser<PostfixTestReader, TestWriter> pfParser(Tokens::instance().postfixTokens());
      pfParser.parse(pfTestReader, pfwriter);
      test_(pfwriter.size() == 4);
    }
};


class ServerTests : public Test {
  public:
    void run() {
	//       FileDescriptor stdin(0);
      TCPListener *listener = new TCPListener("", "4096");
//       test_(listener != 0);
      SingleAcceptThreadPool<TestRequestHandler>* server = 
	new SingleAcceptThreadPool<TestRequestHandler>(listener, 50, 10, 100, 5000);
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
  suite.addTest(new ParamListTest);
//   suite.addTest(new ServerTests);
  //   suite.addTest(new SelectTest);
  //   suite.addTest(new Chatserver);
  suite.run();
  long nFail = suite.report();
  suite.free();
//   CHECK_LEAKS();
  return nFail;
}
