// 2008-0ct-01T20:30
// log4cxx example of working with an external process that rotates
// log files and sends SIGHUP to the logging app to tell it to close
// and reopen files.
//
// g++ -o syslogAppender syslogAppender.cpp -llog4cxx
//
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/net/syslogappender.h>

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sched.h>

#include "Thread.h"
#include "Mutex.h"
#include "Lock.h"

#include "../TestSuite/Suite.h"


Mutex countMutex;
int count = 0;
int maxcount = 0;

class LogTest : public TestSuite::Test {
  public:
    LogTest(unsigned t) : m_threads(t) {}
    class SimpleThread : public Thread {
      public:
        SimpleThread(unsigned id, log4cxx::LoggerPtr& l) : Thread(), m_id(id), m_logger(l) {
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
            LOG4CXX_INFO(m_logger, "thread #" << m_id << " beep #" << ++i << " bla %s ");
            sched_yield();
//             std::cout << i << std::endl;
//             sleep(2);
          }
        }
      private:
        unsigned m_id;
        log4cxx::LoggerPtr &m_logger;
    };

    void run() {
//       log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
//       root->removeAllAppenders();
      
      log4cxx::BasicConfigurator::configure();

      log4cxx::LayoutPtr l(new log4cxx::PatternLayout("%-4r %-5p %c %x - %m"));
      log4cxx::net::SyslogAppenderPtr a(new log4cxx::net::SyslogAppender(l
                                        , log4cxx::net::SyslogAppender::getFacility("DAEMON")));
      a->setSyslogHost("127.0.0.1");
//       log4cxx::helpers::Pool p;
//       a->activateOptions(p);

      log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("MyApp"));
      logger->removeAllAppenders();
      logger->addAppender(a);
      for (int i = 0; i < m_threads;i++) {
        SimpleThread *prTh = new SimpleThread(i, logger);
        prTh->Start();
      }
      do {
        sleep(1);

      } while (count != 0);
      LOG4CXX_INFO(logger, "maxthreads " << maxcount);
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
