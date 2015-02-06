/**   $Id: requesthandler.h 18 2009-09-02 01:51:53Z mimo $   **/
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

#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include <iostream>
#include "singleacceptthreadpool.h"

class TestRequestHandler : public RequestHandler /*: public ZThread::Runnable*/
{
  FileDescriptor* _fd;
//   unsigned _requests;
//   bool _busy;
  RequestCallback *_callback;
public:
//   ~TestRequestHandler() { std::cout << "RequestHandler destroyed" << std::endl; }
  TestRequestHandler() : _fd(0) { }
  TestRequestHandler(FileDescriptor* fd, RequestCallback *cb) : _fd(fd), _callback(cb) { }
  void setFileDescriptor(FileDescriptor *fd) { _fd = fd; } 
  virtual void run ();
  ssize_t readline(int fd, void *vptr, size_t maxlen);
  ssize_t writen(int fd, const void *vptr, size_t n);
  
//   ssize_t Readline(int fd, void *ptr, size_t maxlen)
  
};

#endif // REQUESTHANDLER_H
