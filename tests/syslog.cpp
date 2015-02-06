// #include  <log4cpp/Category.hh>
// #include  <log4cpp/SyslogAppender.hh>
// #include  <log4cpp/BasicLayout.hh>

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sched.h>
#include <sstream>
#include "Thread.h"
#include "Mutex.h"
#include "Lock.h"

#include <syslog.h>

#include "../TestSuite/Suite.h"


Mutex countMutex;
int count = 0;
int maxcount = 0;

class LogTest : public TestSuite::Test {
  public:
    LogTest(unsigned t) : m_threads(t) {}
    class SimpleThread : public Thread {
      public:
        SimpleThread(unsigned id) : Thread(), m_id(id) {
          SetDeleteOnExit();
          Lock lock(countMutex);
          ++count;
          if (count > maxcount) {
            maxcount = count;
          }
        }
        ~SimpleThread() {
          Lock lock(countMutex);
          --count;
        }

        void Run() {
          for (int i = 0; i < 100; i++) {
            syslog(LOG_INFO, "thread #%d   beep #%d bla %%s ", m_id, ++i);

            sched_yield();
//             std::cout << i << std::endl;
//             sleep(2);
          }
        }
      private:
        unsigned m_id;
    };

    void run() {
      openlog("syslog", 0, LOG_DAEMON);

      for (int i = 0; i < m_threads;i++) {
        SimpleThread *prTh = new SimpleThread(i);
        prTh->Start();
      }
      do {
        sleep(1);

      } while (count != 0);
      syslog(LOG_INFO, "maxthreads %d", maxcount);
    }
  private:
    unsigned m_threads;
};

int main(int argc, char *argv[]) {
  int threads = 10;
  if (argc == 2) {
    threads = atoi(argv[1]);
  }
  TestSuite::Suite suite("LogTest");
  suite.addTest(new LogTest(threads));
  suite.run();
  long nFail = suite.report();
  suite.free();
  return nFail;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; 
