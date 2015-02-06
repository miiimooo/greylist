/** \file Thread.cpp
 **	\date  2004-10-30
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004-2008  Anders Hedstrom

This library is made available under the terms of the GNU GPL.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdio.h>
#ifdef _WIN32
#include <process.h>
#include "socket_include.h"
#else
#include <unistd.h>
#endif

#include <cstdlib>
#include <iostream>

#include "Thread.h"
#include "Lock.h"
#include "Mutex.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// Mutex Thread::s_threadListMutex;
// Thread::ThreadMap Thread::s_waiting_threads;
// Mutex Thread::s_mutex;

Thread::Thread(bool release)
:m_thread(0)
,m_running(true)
,m_release(release)
,m_b_delete_on_exit(false)
,m_b_destructor(false)
{
}
void Thread::Start() {
//   Lock runLock(m_runMutex);
#ifdef _WIN32
//	m_thread = ::CreateThread(NULL, 0, StartThread, this, 0, &m_dwThreadId);
	m_thread = (HANDLE)_beginthreadex(NULL, 0, &StartThread, this, 0, &m_dwThreadId);
#else
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
//   if (pthread_create(&m_thread,&attr, StartThread,this) == -1)
	if (pthread_create(&m_thread, &attr, StartThread, this) == -1)
	{
		perror("Thread: create failed");
		SetRunning(false);
	}
//	pthread_attr_destroy(&attr);
#endif
// 	m_release = release;
//   Lock runLock(m_runMutex);
//   Lock listLock(s_threadListMutex);
//   s_waiting_threads.insert(ThreadMap::value_type(this, &this->m_runMutex));
//   std::cout << "constructor waiting: " << s_waiting_threads.size() << std::endl;
}


Thread::~Thread()
{
	m_b_destructor = true;
	if (m_running)
	{
		SetRelease(true);
		SetRunning(false);
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
#ifdef _WIN32
	if (m_thread)
		::CloseHandle(m_thread);
#endif
}

// Mutex Thread::s_mutex;
threadfunc_t STDPREFIX Thread::StartThread(threadparam_t zz)
{
//   std::cout << "worker waiting: " << s_waiting_threads.size() << std::endl;
//   {
//     Lock listLock(s_threadListMutex);
//     std::cout << "got lock waiting: " << s_waiting_threads.size() << std::endl;
//   }
//   Lock lock(s_mutex);
//   ThreadInfo *i = (ThreadInfo*) zz;
//   Lock runLock(*i->m);
	Thread *p = (Thread *)zz;
  while (p -> m_running && !p -> m_release)
  {
#ifdef _WIN32
    Sleep(1000);
#else
    std::cout << "this shouldn't happen" <<std::endl;
    exit(-1);
    sleep(1);
#endif
  }
  if (p -> m_running)
  {
    p -> Run();
  }
  p -> SetRunning(false); // if return
  if (p -> DeleteOnExit() && !p -> IsDestructor())
  {
    delete p;
  }
#ifdef _WIN32
  _endthreadex(0);
#endif
  return (threadfunc_t)NULL;

  
  
  //   delete i;
  //   Lock runLock(p -> m_runMutex);
  p->Run();
  p -> SetRunning(false); // if return
  if (p -> DeleteOnExit() && !p -> IsDestructor())
  {
    delete p;
  }
#ifdef _WIN32
  _endthreadex(0);
#endif
  return (threadfunc_t)NULL;
//   bool bConstructorFinished = false;
//   while(!bConstructorFinished) {
//     long unsigned int size = 0;
//     {
//       Lock listLock(s_threadListMutex);
//       size = s_waiting_threads.size();
//     }
//     if (size == 0) {
// //       sleep(1);
//       continue;
//     }
//     Lock listLock(s_threadListMutex);
//     ThreadMap::iterator itr = s_waiting_threads.begin();
//     {
//       Lock runLock(*(itr->second));
//       p = itr->first;
//     }
//     s_waiting_threads.erase(itr);
//     bConstructorFinished = true;
// /*    bool bInList = false;
//     {
//       Lock listLock(s_threadListMutex);
//       ThreadMap::iterator itr = s_waiting_threads.find(p);
//       bInList = (itr != s_waiting_threads.end()); 
//     }
//     if (!bInList ) {
//       std::cout << "not inlist" << std::endl;
//       sleep(1);
//       continue;
//     }*/
// /*    Lock listLock(s_threadListMutex);
//     ThreadMap::iterator itr = s_waiting_threads.find(p);
//     if (itr == s_waiting_threads.end()) {
//       continue;
//     }*/
// //     std::cout << "got listlock, waiting " << s_waiting_threads.size() << std::endl;
// //     for(ThreadMap::iterator itr=s_waiting_threads.begin(); itr != s_waiting_threads.end(); ++itr) {
// //       std::cout << "p = " << p << " itr=" << itr->first <<std::endl;
// //     }
// //     ThreadMap::iterator itr = s_waiting_threads.find(p);
// //     std::cout << "found p in list " << p << std::endl;
// //     Lock runLock(*(itr->second));
// //     bConstructorFinished = true;
// //     s_waiting_threads.erase(p);
//   }
//   Lock constructorLock(p->getMutex());
//   Mutex threadMutex;
// 	s_waiting_threads.insert(std::pair<p, threadMutex>);
  while (p -> m_running && !p -> m_release)
	{
#ifdef _WIN32
		Sleep(1000);
#else
    std::cout << "this shouldn't happen" <<std::endl;
    exit(-1);
		sleep(1);
#endif
	}
	if (p -> m_running)
	{
		p -> Run();
	}
	p -> SetRunning(false); // if return
	if (p -> DeleteOnExit() && !p -> IsDestructor())
	{
		delete p;
	}
#ifdef _WIN32
	_endthreadex(0);
#endif
	return (threadfunc_t)NULL;
}


bool Thread::IsRunning()
{
 	return m_running;
}


void Thread::SetRunning(bool x)
{
 	m_running = x;
}


bool Thread::IsReleased()
{
 	return m_release;
}


void Thread::SetRelease(bool x)
{
 	m_release = x;
}


bool Thread::DeleteOnExit()
{
	return m_b_delete_on_exit;
}


void Thread::SetDeleteOnExit(bool x)
{
	m_b_delete_on_exit = x;
}


bool Thread::IsDestructor()
{
	return m_b_destructor;
}


#ifdef SOCKETS_NAMESPACE
}
#endif


