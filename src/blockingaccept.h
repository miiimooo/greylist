/**   $Id: blockingaccept.h 13 2009-08-31 22:55:52Z mimo $   **/
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

#ifndef BLOCKINGACCEPT_H
#define BLOCKINGACCEPT_H

#include <iostream>
#include "zthread/Thread.h"
#include "zthread/PoolExecutor.h"

#include "filedescriptor.h"
#include "TQueue.h"
#include "acceptor.h"
/** blocking accept with a queue that launches the threads from a pool**/
class BlockingAccept : public Acceptor
{
public:
  typedef ZThread::PoolExecutor ThreadExecutorType; 
  typedef TQueue<FileDescriptor *> ThreadQueueType;
protected:
  ThreadQueueType _queue;
  ThreadExecutorType _executor;
  unsigned _busyHandlers;
  ZThread::Mutex lock;
  public:
  
  BlockingAccept() : _queue(), _executor(100), _busyHandlers(0) {}
  // Acceptor
  virtual FileDescriptor* accept(const FileDescriptor* );
  virtual void addBusyHandler(const ZThread::Runnable *);
  virtual void removeBusyHandler(const ZThread::Runnable *);
  
  void add(FileDescriptor* fd) { _queue.put(fd); }
  virtual void run();
};

#endif // BLOCKINGACCEPT_H
