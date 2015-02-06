/**   $Id: clientsocket.h 13 2009-08-31 22:55:52Z mimo $   **/
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

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "utils.h"
#include "filedescriptor.h"

class ClientSocket : public FileDescriptor
{
  socklen_t _peerAddressSize;
  struct sockaddr *_pPeerAddress;
  int _error;
public:
  ClientSocket(int fd, socklen_t clilen, struct sockaddr *cliaddr, int e) 
    : FileDescriptor(fd), _peerAddressSize(clilen), _pPeerAddress(cliaddr)
      , _error(e) { }
      void close() {
	int res = ::close(FileDescriptor::get());
	_error = (res == 0 ? 0 : errno);
	FileDescriptor::set(-1); 
      }
  virtual int getErrno() const { return _error; }
      
  virtual ~ClientSocket() {
    Utils::zap(_pPeerAddress);
    if (FileDescriptor::get() != -1) {
      std::cout << "forgot to close client socket" << std::endl;
    }
  }
};

#endif // CLIENTSOCKET_H
