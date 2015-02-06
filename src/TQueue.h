/**   $Id: TQueue.h 13 2009-08-31 22:55:52Z mimo $   **/
#ifndef TQUEUE_H
#define TQUEUE_H
#include <deque>
#include "zthread/Thread.h"
#include "zthread/Condition.h"
#include "zthread/Mutex.h"
#include "zthread/Guard.h"

template<class T> class TQueue {
  ZThread::Mutex lock;
  ZThread::Condition cond;
  std::deque<T> data;
  public:
    TQueue() : cond(lock) {}
    void put(T item) {
      ZThread::Guard<ZThread::Mutex> g(lock);
      data.push_back(item);
      cond.signal();
    }
    T get() {
      ZThread::Guard<ZThread::Mutex> g(lock);
      while(data.empty())
	cond.wait();
      T returnVal = data.front();
      data.pop_front();
      return returnVal;
    }
    size_t size() const { return data.size(); }
};

#endif