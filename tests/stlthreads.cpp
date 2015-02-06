 
//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include <sstream>
#include <string>
#include <cstddef>
#include "Thread.h"
#include "dbiwrapped.h"
#include <boost/detail/atomic_count_gcc_x86.hpp>

#include "../TestSuite/Suite.h"

// #include "TrimTest.h"
using namespace std;
using namespace TestSuite;
using namespace::boost::detail;
template <class T>
class AtomicCounter : public atomic_count {
  public:
    AtomicCounter(long v) : atomic_count(v) {}
};

template <class T>
class LockedCounter {
  mutable Mutex m_mutex;
  T m_count;
  public:
    // never pass this to a thread before constructor is finished
    LockedCounter(T initial=0) : m_mutex(), m_count(initial) { }
//     T get() const {
    operator T() const {
      Lock lock(m_mutex);
      return m_count;
    }
    bool operator ==(const T &v) const {
      Lock lock(m_mutex);
      return (v==m_count);
    }
    bool operator !=(const T &v) const {
      Lock lock(m_mutex);
      return (v!=m_count);
    }
    const T &operator ++() {
      Lock lock(m_mutex);
      return ++m_count;
    }
    const T &operator --() {
      Lock lock(m_mutex);
      return --m_count;
    }
  protected:
    LockedCounter(const LockedCounter &) {}
};

class STLThreadsTest : public TestSuite::Test {
//   enum {NTESTS = 11};
//   static std::string s[NTESTS];
public:
  template<class T>
  class Reader : public Thread {
    public:
      const T& m_container;
      LockedCounter<int> &m_counter;
      int m_id;
      Reader(const T &container, LockedCounter<int> &counter, unsigned id) 
      : Thread(), m_counter(counter), m_container(container), m_id(id){
        SetDeleteOnExit();
        ++m_counter;
      }
      void Run() {
//         cout << "created #"<<m_id<<endl;
        int count = 0;
        for(typename T::const_iterator i = m_container.begin();i != m_container.end(); ++i) {
//           sleep(1);
//           if (*i != count) {
//              cout << "error in " << m_id << " count=" << count << endl;
//           }
          cout << *i << endl;
          count++;

        }
      }
      ~Reader() {
        --m_counter;
      }
  };
  template<class T>
  class Writer : public Thread {
    public:
      const T& m_container;
      LockedCounter<int> &m_counter;
      int m_id;
      Writer(const T &container, LockedCounter<int> &counter, unsigned id) 
      : Thread(), m_counter(counter), m_container(container), m_id(id){
        SetDeleteOnExit();
        ++m_counter;
      }
      void Run() {
//         cout << "created #"<<m_id<<endl;
        sleep(1);
        int count = 0;
        for(typename T::const_iterator i = m_container.begin();i != m_container.end(); ++i) {
//           sleep(1);
          if (*i != count) {
             cout << "error in " << m_id << " count=" << count << endl;
          }
          count++;

        }
      }
      ~Writer() {
        --m_counter;
      }
  };
  void testLockedCounter() {
//     LockedCounter<int> counter;
    AtomicCounter<int> counter(0);
//     atomic_count counter;
    int initial = counter;
//     int result = ++counter;
    ++counter;
    test_(counter == 1);
//     test_(result == 1);
    cout << "counter is now 1 " << counter << endl; 
    --counter;
    test_(counter == 0);
    test_(counter == initial);
  }
  void testSTL(unsigned threads) {
    LockedCounter<int> counter;
    vector<int> iVec;
    for (int i=0; i<10;i++) {
      iVec.push_back(i);
    }
    for (int i=0; i<threads;i++) {
      Reader<vector<int> > *prTh = new Reader<vector<int> >(iVec, counter, i);
      prTh->Start();
    }
    do {
//       iVec[5] = 4;
      cout << "upset has happened" << endl;
//       sleep(1);
      iVec.push_back(threads+1);
      cout << "counter is now " << counter << " ";
      sleep(1);
      
    } while(counter != 0);
  }
  void testSTL_strings(unsigned threads) {
    LockedCounter<int> counter;
    vector<string> vStrings;
    for (int i=0; i<threads; i++) {
/*      stringstream stm;
      stm << "element #" << i;*/
      vStrings.push_back(StrmConvert(i));
    }
    for (int i=0; i<threads;i++) {
      Reader<vector<string> > *prTh = new Reader<vector<string> >(vStrings, counter, i);
      prTh->Start();
    }
    do {
//       iVec[5] = 4;
      cout << "upset has happened" << endl;
//       sleep(1);
//       iVec.push_back(threads+1);
      cout << "counter is now " << counter << " ";
      sleep(1);
      
    } while(counter != 0);
  }
  void run() {
    testSTL_strings(10);
    testLockedCounter();
    testSTL(10);
  }
};
int main() {
    Suite suite("Threaded STL Tests");
//     suite.addTest(new StringStorageTest);
//     suite.addTest(new SieveTest);
//     suite.addTest(new FindTest);
//     suite.addTest(new RparseTest);
    suite.addTest(new STLThreadsTest);
//     suite.addTest(new CompStrTest);
    suite.run();
    long nFail = suite.report();
    suite.free();
    return nFail;
}
