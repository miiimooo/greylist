/**   $Id: filedescriptor.h 13 2009-08-31 22:55:52Z mimo $   **/
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

#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <unistd.h>

class FileDescriptor
{
  int _fd;
public:
  FileDescriptor() : _fd(-1) { }
  FileDescriptor(int fd) : _fd(fd) { }
  int get() const { return _fd; }
  void set(int fd) { _fd = fd; }
  virtual void close() { }
  virtual void open() { }
  virtual socklen_t getClientLength() const { return 0; }
  virtual int getErrno() const { return 0; }
  virtual void setErrno(int) { }
  virtual ~FileDescriptor() { }
};

#endif // FILEDESCRIPTOR_H
