/**   $Id: prethreadedserver.h 7 2009-08-23 03:17:04Z mimo $   **/
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

#ifndef PRETHREADEDSERVER_H
#define PRETHREADEDSERVER_H

#include "zthread/Thread.h"
#include "zthread/ThreadedExecutor.h"

#include "filedescriptor.h"
#include "acceptor.h"
#include "requestrunner.h"

// template <class T>
class PreThreadedServer
{
  FileDescriptor* _fd;
  Acceptor* _acceptor;
  TQueue<FileDescriptor *> _queue;
  RequestRunner *_runner;
public:
  PreThreadedServer(FileDescriptor* fd, Acceptor* ac) : _fd(fd), _acceptor(ac)
    , _queue(), _runner(0) {}
  void run();
};


#endif // PRETHREADEDSERVER_H
