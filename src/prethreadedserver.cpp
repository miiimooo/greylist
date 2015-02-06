/**   $Id: prethreadedserver.cpp 13 2009-08-31 22:55:52Z mimo $   **/
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
#include <iostream>
#include <errno.h>
#include "./prethreadedserver.h"

using namespace std;

void PreThreadedServer::run() {
  _fd->open();
//   _runner = new RequestRunner;
//   ZThread::Thread t(_runner);
  ZThread::Thread t(_acceptor);
  while(true) {
    FileDescriptor *client = _acceptor->accept(_fd);
//     cout << "got error no " << errno << endl;
//     cout << "got file descriptor " << client->get() << endl;
//     _runner->add(client);
  }
  _fd->close();
}