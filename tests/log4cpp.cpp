#include  <log4cpp/Category.hh>
#include  <log4cpp/SyslogAppender.hh>
#include  <log4cpp/BasicLayout.hh>

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
        SimpleThread(unsigned id, log4cpp::Category& l) : Thread(), m_id(id), m_logger(l) {
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
            m_logger.warnStream() << "thread #" << m_id << " beep #" << ++i << " bla %s ";

            sched_yield();
//             std::cout << i << std::endl;
//             sleep(2);
          }
        }
      private:
        unsigned m_id;
        log4cpp::Category &m_logger;
    };

    void run() {


      // 1 instantiate an appender object that
      // will append to a log file
      log4cpp::Appender* app = new
      log4cpp::SyslogAppender("SyslogAppender", "log4cpp", LOG_DAEMON);

      // 2. Instantiate a layout object
      // Two layouts come already available in log4cpp
      // unless you create your own.
      // BasicLayout includes a time stamp
//       log4cpp::Layout* layout =
//         new log4cpp::BasicLayout();

      // 3. attach the layout object to the
      // appender object
//       app->setLayout(layout);

      // 4. Instantiate the category object
      // you may extract the root category, but it is
      // usually more practical to directly instance
      // a child category
//       log4cpp::Category main_cat = log4cpp::Category::getRoot();
      log4cpp::Category&  root   = log4cpp::Category::getRoot();
      root.setPriority(log4cpp::Priority::INFO);


      // 5. Step 1
      // an Appender when added to a category becomes
      // an additional output destination unless
      // Additivity is set to false when it is false,
      // the appender added to the category replaces
      // all previously existing appenders
      root.setAdditivity(false);

      // 5. Step 2
      // this appender becomes the only one
      root.setAppender(app);

      // 6. Set up the priority for the category
      // and is given INFO priority
      // attempts to log DEBUG messages will fail
//       main_cat.setPriority(log4cpp::Priority::INFO);

      // so we log some examples
//       main_cat.info("This is some info");
//       main_cat.debug("This debug message will fail to write");
//       main_cat.alert("All hands abandon ship");

      // you can log by using a log() method with
      // a priority
//       main_cat.log(log4cpp::Priority::WARN, "This will be a logged warning");

      for (int i = 0; i < m_threads;i++) {
        SimpleThread *prTh = new SimpleThread(i, root);
        prTh->Start();
      }
      do {
        sleep(1);

      } while (count != 0);
      root.infoStream() << "maxthreads " << maxcount;
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
