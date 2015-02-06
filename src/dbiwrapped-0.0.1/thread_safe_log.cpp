// Solution 4 (final solution)
// note: compiles with gcc 3.2, VC6 and
// Comeau++ (http://www.comeaucomputing.com/tryitout/)
#include "thread_safe_log.h"
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm>
//////////////////////////////////////////////////////////
// Test
#include <iostream>
#include <fstream>
#include <iomanip>
const int THREADS_COUNT = 200;
const int WRITES_PER_THREAD = 500;
// the writer
thread_safe_log_writer_sharethread & get_ts_writer()
{
  static thread_safe_log_writer_sharethread writer;
  return writer;
}
template< int i> struct int_to_type {
  int_to_type() {} };
// return out<idx>.txt
// (example: for 3, return 'out3.txt')
template< int idxLog> 
std::string get_out_name(
  bool bIsSharedLog,
  int_to_type< idxLog> * = NULL /* workaround for VC6 bug */)
{
  std::ostringstream out;
  out << "out" << (bIsSharedLog ? "sharedthread" : "ownthread")
  << idxLog << ".txt";
  return out.str();
}
// (Shared Thread) we have 10 logs
// log <idx> has priority (<idx>+1)^2 * 10
// (example: log 6 has priority 490)
template< int idxLog>
thread_safe_log templ_get_log_sharedthread(int_to_type< idxLog> * = NULL /* workaround for VC6 bug */) {
  static std::ofstream out(get_out_name< idxLog>(true).c_str());
  static internal_thread_safe_log_sharethread log(out, get_ts_writer(), 10 * (idxLog + 1) * (idxLog + 1));
  return thread_safe_log(log);
}
// (Own Thread) we have 10 logs
// each log has its own thread for writing to it
template< int idxLog>
thread_safe_log templ_get_log_ownthread(int_to_type< idxLog> * = NULL /* workaround for VC6 bug */) {
  static std::ofstream out(get_out_name< idxLog>(false).c_str());
  static internal_thread_safe_log_ownthread log(out);
  return thread_safe_log(log);
}
// based on the index, return a different log
//// first 10 logs - logs sharing the same thread
// last 10 logs  - each log with its own thread
thread_safe_log get_log(int idxLog) {
  switch (idxLog)    {
    case 0: return templ_get_log_sharedthread< 0>();
    case 1: return templ_get_log_sharedthread< 1>();
    case 2: return templ_get_log_sharedthread< 2>();
    case 3: return templ_get_log_sharedthread< 3>();
    case 4: return templ_get_log_sharedthread< 4>();
    case 5: return templ_get_log_sharedthread< 5>();
    case 6: return templ_get_log_sharedthread< 6>();
    case 7: return templ_get_log_sharedthread< 7>();
    case 8: return templ_get_log_sharedthread< 8>();
    case 9: return templ_get_log_sharedthread< 9>();
    case 10: return templ_get_log_ownthread< 0>();
    case 11: return templ_get_log_ownthread< 1>();
    case 12: return templ_get_log_ownthread< 2>();
    case 13: return templ_get_log_ownthread< 3>();
    case 14: return templ_get_log_ownthread< 4>();
    case 15: return templ_get_log_ownthread< 5>();
    case 16: return templ_get_log_ownthread< 6>();
    case 17: return templ_get_log_ownthread< 7>();
    case 18: return templ_get_log_ownthread< 8>();
    case 19: return templ_get_log_ownthread< 9>();
    default:
      assert(false);
      return templ_get_log_sharedthread< 0>();
  }
}
// #include <windows.h>
// LONG nRemainingThreads = THREADS_COUNT;
// DWORD WINAPI WriteToLog(LPVOID lpData) {
//   int *pnThreadID = (int *)lpData;
//   int idxLog = *pnThreadID % 20;
//   // wait for all threads to be created, so that
//   // we write at about the same time (stress it ;-))
//   Sleep(500);
//   for (int idx = 0; idx < WRITES_PER_THREAD; idx++)    {
//     get_log(idxLog).ts() << "writing double: " << 5.23 << std::endl;
//     get_log(idxLog).ts() << "message " << idx << " from thread " << *pnThreadID << std::endl;
//     // ... get other threads a chance to write
//     Sleep(1);
//     if ((idx == 10) && (*pnThreadID == 10))        {
//       // from now on, '5.23' will be written as '5,23'
//       // (german locale)
//       std::locale loc = std::locale("german");
//       get_log(idxLog).ts().imbue(loc);
//     }
//   }
//   InterlockedDecrement(&nRemainingThreads);
//   delete pnThreadID;
//   return 0;
// }
// int main(int argc, char* argv[]) {
//   // make sure the statics are initialized
//   get_ts_writer();
//   get_log(0);
//   get_log(1);
//   get_log(2);
//   get_log(3);
//   get_log(4);
//   get_log(5);
//   get_log(6);
//   get_log(7);
//   get_log(8);
//   get_log(9);
//   get_log(10);
//   get_log(11);
//   get_log(12);
//   get_log(13);
//   get_log(14);
//   get_log(15);
//   get_log(16);
//   get_log(17);
//   get_log(18);
//   get_log(19);
//   // would cause an assertion - forgot to write std::endl to the stream!
//   // get_log( 0).ts() << "test";
//   // the following should generate an assertion:
//   // writing to temporary after it's been destructed
//   //
//   // std::ostream & out1 = get_log( 0).ts();
//   // out1 << "blabla" << std::endl;
//   // std::ostream & out2 = get_log( 0).ts();
//   // out2 << std::endl;
//   for (int idx = 0; idx < THREADS_COUNT; ++idx)    {
//     DWORD dwThreadID;
//     CreateThread(0, 0, WriteToLog, new int(idx), 0, &dwThreadID);
//   }
//   // wait for all threads to end
//   while (true)    {
//     InterlockedIncrement(&nRemainingThreads);
//     if (InterlockedDecrement(&nRemainingThreads) == 0)
//       break;
//     Sleep(100);
//   }
//   return 0;
// }
// kate: indent-mode cstyle; space-indent on; indent-width 2;
