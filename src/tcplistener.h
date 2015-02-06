/**   $Id: tcplistener.h 13 2009-08-31 22:55:52Z mimo $   **/
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

#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include <string>
#include "filedescriptor.h"

class TCPListener : public FileDescriptor
{
  std::string _host, _port; 
  unsigned _backlog;
  socklen_t _addrlen;
public:
  /* basic constructor */
  TCPListener(const std::string &host, const std::string port)
    : FileDescriptor(), _host(host), _port(port), _backlog(1024) { }
  virtual void open();
  virtual socklen_t getClientLength() const { return _addrlen; }
  virtual void close();
};

#endif // TCPLISTENER_H
