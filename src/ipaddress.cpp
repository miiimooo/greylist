/// $Id: ipaddress.cpp 27 2009-09-15 19:37:26Z mimo $
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
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <limits.h>
#include <arpa/inet.h>

#include "utils.h"
#include "./ipaddress.h"

void IPAddress::resolve() {
  char buf[256];
  struct sockaddr_in sa;
  bzero(&sa, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  inet_pton(AF_INET, _str.c_str(), &sa.sin_addr);
  
  int n = getnameinfo((const sockaddr*)&sa, sizeof(struct sockaddr_in), 
		       buf, sizeof(buf), 0, 0, NI_NAMEREQD) ;
  if (n != 0) {
    _resolve_error = "resolve " + _str + ": " + gai_strerror(n);
  }
  _reverse = buf;
  Utils::lowercase(_reverse);
}