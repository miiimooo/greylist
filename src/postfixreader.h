/**   $Id: postfixreader.h 28 2009-09-17 01:22:44Z mimo $   **/
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

#ifndef POSTFIXREADER_H
#define POSTFIXREADER_H

#include <iostream>
#include <errno.h>
#include <string>
#include <boost/smart_ptr.hpp>
#include "filedescriptor.h"


template <class FileDescriptorType=boost::shared_ptr< FileDescriptor> >
class PostfixReader
{
  FileDescriptorType _fd;
  bool _eof;
public:
  PostfixReader(FileDescriptorType  fd) : _fd(fd), _eof(false) { }
  bool isEof() const { return _eof; }
  std::string::size_type getline(std::string &str) {
    if (!str.empty()) {
      str.clear();
    }
    ssize_t n = 0, rc;
    char c;
    while( (rc = read(_fd->get(), &c, 1)) == 1) {
      if (c == '\n') {
	if (n == 0) {
	  return std::string::npos;
	} else {
	  return n;
	}
      }
      str += c;
      n++;
    }
//     std::cout << "end of file " << _fd->get() << std::endl;
    _eof = true;
    if (rc < 0) {
      _fd->setErrno(errno);
    }
    if (n == 0) {
//       std::cout << "end of file " << _fd->get() << " closed" << std::endl;
      return std::string::npos;
    }
//     std::cout << "socket " << _fd->get() << " closed" << std::endl;
    return n;
  }
};

#endif // POSTFIXREADER_H
