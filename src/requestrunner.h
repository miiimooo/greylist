/**   $Id: requestrunner.h 13 2009-08-31 22:55:52Z mimo $   **/
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

#ifndef REQUESTRUNNER_H
#define REQUESTRUNNER_H

#include <iostream>
#include "zthread/Thread.h"
#include "zthread/PoolExecutor.h"

#include "filedescriptor.h"
#include "acceptor.h"
#include "TQueue.h"
#include "requesthandler.h"

class RequestRunner : public ZThread::Runnable
{
  TQueue<FileDescriptor *> _queue;
  ZThread::PoolExecutor _executor;
public:
  RequestRunner() : _queue(), _executor(100) {}
  void add(FileDescriptor* fd) { _queue.put(fd); }
  void run() {
    try {
      while(!ZThread::Thread::interrupted()) {
	FileDescriptor* fd = _queue.get();
// 	_executor.execute(new RequestHandler(fd));
// 	std::cout << "pool size " << _executor.size() << std::endl;
      }
    } catch(ZThread::Interrupted_Exception&) { /* Exit */ }
    std::cout << "Exiting RequestRunner" << std::endl;
  }
  
};

#endif // REQUESTRUNNER_H
