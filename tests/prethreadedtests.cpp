/**   $Id: prethreadedtests.cpp 12 2009-08-31 22:49:57Z mimo $   **/

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

#include "zthread/Thread.h"
#include "zthread/ThreadedExecutor.h"
#include "zthread/AtomicCount.h"
#include "zthread/Mutex.h"
#include "zthread/Guard.h"
#include "zthread/Condition.h"

#include "defensive.h"
#include "filedescriptor.h"
#include "blockingaccept.h"
#include "blockingacceptnoqueue.h"
#include "tcplistener.h"
#include "prethreadedserver.h"

using namespace std;
using namespace TestSuite;
using namespace ZThread;

#define MAXDATASIZE 100 // max number of bytes we can get at once 
class Runner {
};
class PreThreaded : public Test {
  public:
    void run() {
//       FileDescriptor stdin(0);
      TCPListener *listener = new TCPListener("", "4096"); 
      BlockingAcceptNoQueue *acceptor = new BlockingAcceptNoQueue;
      PreThreadedServer server(listener, acceptor);
      server.run();
    }
};

int main() {
  Suite suite("PreThreaded Tests");
  suite.addTest(new PreThreaded);
  //   suite.addTest(new SelectTest);
  //   suite.addTest(new Chatserver);
  suite.run();
  long nFail = suite.report();
  suite.free();
//   CHECK_LEAKS();
  return nFail;
}
