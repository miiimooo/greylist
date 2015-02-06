/// $Id: simplescheduler.h 68 2011-08-09 23:45:45Z mimo $
/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef SIMPLESCHEDULER_H
#define SIMPLESCHEDULER_H

#include <map>
#include <deque>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/detail/atomic_count.hpp>
// #include "osdependant.h"

template <class TaskType>
class SimpleScheduler {
  struct Worker /* : public boost::enable_shared_from_this<SimpleScheduler<TaskType> >*/ {
    SimpleScheduler<TaskType> & _scheduler;
    Worker(SimpleScheduler<TaskType> & s) : _scheduler(s) { }
    ~Worker() { /*std::cout << "Worker destroyed" << std::endl;*/ }
//     void run(SimpleScheduler<TaskType> &scheduler) {
    void run() {
//       OSDependant::tuneThisThread();
//       std::cout << "thread running" << std::endl;
      while (!_scheduler.exec()) { }
    }
  };
  unsigned _maxSpare, _maxWorkers;
  typedef std::map<boost::thread::id, boost::thread *> WorkerContainer;
  typedef std::deque<TaskType> TaskQueue;
  TaskQueue _queue;
  boost::detail::atomic_count _active;
  boost::detail::atomic_count _waiting;
  boost::condition_variable _signal;
  boost::mutex _monitor, _mutex;
  volatile bool _shutdown;
  
public:
  typedef SimpleScheduler<TaskType> SchedulerType;
//   typedef boost::thread * TaskHandle;
  SimpleScheduler(unsigned maxSpare, unsigned maxWorkers) : _maxSpare(maxSpare), 
    _maxWorkers(maxWorkers), _active(0), _waiting(0), _shutdown(false) { }
  ~SimpleScheduler() {
  }
  void shutdown() { 
    _shutdown = true; 
  }
  void wait() { 
    while((_active > 0) || (_waiting > 0)) {
//       std::cout << "entering shutdown active=" << unsigned(_active) << " waiting=" << unsigned(_waiting) << std::endl;
      {
	boost::mutex::scoped_lock lock(_monitor);
// 	_monitor.unlock();
	_signal.notify_all();
      }
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
  }
  bool isShutdown() const { return _shutdown; }
  
  bool schedule(TaskType const & task) {
    if (_shutdown) {
      return false;
    }
    if ( (_waiting + _active) >= _maxWorkers) {
      return false;
    }
    if ((_waiting == 0) || (!_queue.empty())) {
      boost::shared_ptr<Worker> worker(new Worker(*this));
      boost::thread workerThread(boost::bind(&Worker::run, worker));
//       boost::thread * workerThread = new boost::thread(boost::bind(&Worker::run, worker));
//       workerThread->detach();
    }
    {
      boost::mutex::scoped_lock lock(_monitor);
      _queue.push_back(task);
      _signal.notify_one();
    }
//     _signal.notify_one();
    return true;
  }

  bool exec() {
    ++_waiting;
//     std::cout << "entering exec active=" << unsigned(_active) << " waiting=" << unsigned(_waiting) << std::endl;
    
    boost::mutex::scoped_lock lock(_monitor);
//     std::cout << "entering wait" << std::endl;
    _signal.wait(lock, boost::bind(std::logical_or<bool>(),
		boost::bind(&SimpleScheduler<TaskType>::isShutdown, this),
		boost::bind(std::logical_not<bool>(), boost::bind( &TaskQueue::empty, &_queue))));
    if (isShutdown()) {
//       std::cout << "waiting thread got shutdown" << std::endl;
      --_waiting;
      return true;
    }
    TaskType task = _queue.front();
    _queue.pop_front();
    lock.unlock();
    
    --_waiting;
    ++_active;
    task();
//     std::cout << "task completed active=" << unsigned(_active) << " waiting=" << unsigned(_waiting) << std::endl;
    --_active;
    if ((_waiting >= _maxSpare) || _shutdown ) {
      return true; // end this thread
    }
    return false; // poll for more tasks
  }
  size_t size() const { return _active + _waiting; }
  unsigned active() const { return _active; }
  unsigned waiting() const { return _waiting; }
};

#endif // SIMPLESCHEDULER_H
