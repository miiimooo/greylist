#include "TcpSocket.h"
#include "Exception.h"
#include "Thread.h"
#include "Lock.h"
#include "SocketHandler.h"
#include "ListenSocket.h"
#include "StdoutLog.h"
#include <signal.h>
#include <iostream>
#include <ctime>
#include "dbiwrapped.h"

using namespace std;

Mutex countMutex;
int count = 0;
int maxcount = 0;

class SimpleThread : public Thread
{
public:
  SimpleThread(boost::shared_ptr<Database> db, port_t port) : Thread(), m_db(db), m_port(port) {
//   SimpleThread(Database &db, port_t port) : m_db(db), m_port(port), Thread() {
    SetDeleteOnExit();
    Lock lock(countMutex);
    ++::count;
    if (::count > ::maxcount) {
       ::maxcount = ::count;
    }
  }
  ~SimpleThread() {
    Lock lock(countMutex);
    --::count;
  }

  void Run() {
    try
    {
//       StdoutLog log;
      string strSender, strRecipient, strClientAddress, strReverseClientName;
      strSender = "me@thisplace.com";
      strRecipient = "null@t61";
      strClientAddress = "127.0.0.1";
      strReverseClientName = "localhost";
      time_t ltime;
      time(&ltime); //get ultime (unix timestamp) in ltime
      unsigned long lNow = ltime;
      Query q(m_db);
      Query::Arguments sqlArgs;
      sqlArgs.push_back(strSender);
      sqlArgs.push_back(strRecipient);
      sqlArgs.push_back(StrmConvert(static_cast<int>(m_port)));
      sqlArgs.push_back(StrmConvert(lNow));
//       std::stringstream stm;
//       stm << "INSERT INTO triplet (sender,recipient,count,uts) " 
//         << " VALUES('" << strSender << "', '" << strRecipient << "'," << static_cast<int>(m_port) 
//         << "," << lNow << ")";
//       q.execute(stm.str());
//       q.free_result();
//       return;
      q.executef("INSERT INTO triplet (sender,recipient,count,uts) " 
        " VALUES(%s,%s,%d,%d)", sqlArgs);
//       cout << "Query: " << q.GetLastQuery() << endl;
//       cout << "Inserted ID: " << q.insert_id() << endl;
      unsigned long long last_id = q.insert_id();
      string sqlError = q.GetError();
      if (sqlError != "") {
        cout << sqlError << endl;
      }
      q.free_result();

//       SocketHandler h(&log);
//       ListenSocket<EchoSocket> l(h);
//       l.Bind(m_port);
//       h.Add(&l);
//       EchoSocket client(h);
//       client.EnableSSL();
//       client.Open("localhost", m_port);
//       h.Add(&client);
//       while (h.GetCount() > 1)
//       {
//         h.Select(1, 0);
//       }
//       std::cout << "Port#" << m_port << ": Leaving thread" << std::endl;
    }
    catch (const Exception& e)
    {
      std::cerr << "Port#" << m_port << ": " << e.ToString() << std::endl;
    }
  }

private:
  port_t m_port;
  boost::shared_ptr<Database> m_db;
  
};
class AnotherSimpleThread : public Thread
{
public:
  AnotherSimpleThread() : Thread() {
//   SimpleThread(Database &db, port_t port) : m_db(db), m_port(port), Thread() {
    SetDeleteOnExit();
    Lock lock(countMutex);
    ++::count;
  }
  ~AnotherSimpleThread() {
    Lock lock(countMutex);
    --::count;
  }

  void Run() {
    sleep(1);
  }
};
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <number of threads>\n", *argv);
    return -1;
  }
  StderrLog logger;
//   SysLog logger;
  Database::Mutex dbMutex;
  boost::shared_ptr<Database> db(new Database(dbMutex, "localhost","root","","greylist", &logger));
//   Database db(dbMutex, "localhost","root","","greylist");
  if (!db->Connected()) {
    printf("Database not connected - exiting\n");
    exit(-1);
  }
  Query q(db);
  q.execute("TRUNCATE triplet");
  cout << "Query: " << q.GetLastQuery() << endl;
  signal(SIGPIPE, SIG_IGN);
  try {
    for (int i = 0; i < atoi(argv[1]); i++) {
      SimpleThread *psth = new SimpleThread(db, 4000 + i);
      psth->Start();
      AnotherSimpleThread *pasth = new AnotherSimpleThread();
      pasth->Start();
    }
  } catch (const std::exception& e) {
      std::cerr << "Exception" << e.what() << std::endl;
  }
  
  bool bIsFinished = false;
  while (!bIsFinished)
  {
    sleep(1);
    Lock lock(countMutex);
    if (::count == 0) {
      bIsFinished = true;
    }
  }
  cout << "maxcount " << maxcount << endl;
  cout << "maxcons " << db->getMaxCons() << endl;
}

