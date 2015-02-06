/**   $Id: main.cpp 55 2009-10-11 22:51:45Z mimo $   **/
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

//#include "./main.h"
#include <pthread.h>
#include <errno.h>
#include "../config.h"
#include "system.h"
#include "configfilereader.h"
// #include "osdependant.h"

int main(int argc, char *argv[]) {
  int res = 0;
  //TODO this doesn't work
//   OSDependant::tuneThisThread();
  size_t reasonable_size = 65536;
  size_t size = reasonable_size;
#ifdef HAVE_PTHREAD_H
  pthread_attr_t tattr;
  if (pthread_attr_init(&tattr) != 0) {
    perror("pthread_attr_init: ");
  }
  if (pthread_attr_getstacksize (&tattr, &size) != 0) {
    perror("pthread_attr_getstacksize: ");
  }
/*  size_t newsize = size;
  while (newsize > (10 * reasonable_size)) {
    newsize >>= 2;
//     std::cout << newsize << std::endl;
  }
  if (newsize != size) {
    if (pthread_attr_setstacksize(&tattr, newsize) != 0) {
      perror("pthread_attr_setstacksize: ");
    }
    if (pthread_attr_getstacksize (&tattr, &size) != 0) {
      perror("pthread_attr_getstacksize: ");
    }
    if (size != newsize) {*/
    if (size > (10 * reasonable_size)) {
      std::cout << "WARNING: the system stack size is set to " << size/1024
	<< "KB.\nRunning with a stack size this big will use very much memory\n"
	<< "and is unnecessary. E.g. with 50 concurrent connections greylist daemon\n"
	<< "will use " << (size*50)/1024/1024 << "MB.\n\nTry changing the size to "
	<< "something more reasonable (" << reasonable_size << "KB).\n"
	<< "You should change the stack limit in your startup script or the shell\n"
	<< "using something like 'ulimit -s 64'.\n\n" << std::endl;
    }
#endif    
//   }
  try {
    ConfigFileReader<std::ifstream> cfgReader("/etc/greylist.cfg");
    System<ConfigFileReader<std::ifstream> > system;
    res = system.run(cfgReader);
  } catch(std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    res = -1;
  }
  return res;
}
