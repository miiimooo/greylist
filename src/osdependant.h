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

#ifndef OSDEPENDANT_H
#define OSDEPENDANT_H

#include <iostream>
#include <pthread.h>
#include <boost/thread.hpp>

class OSDependant
{
public:
  /* by default the stack size of a thread is multiple MBs */
  static void tuneThisThread() {
    size_t newsize = PTHREAD_STACK_MIN, size; 
    pthread_attr_t tattr;
    if (pthread_attr_init(&tattr) != 0) {
      perror("pthread_attr_init: ");
    }
    if (pthread_attr_setstacksize(&tattr, newsize) != 0) {
      perror("pthread_attr_setstacksize: ");
    }
    if (pthread_attr_getstacksize (&tattr, &size) != 0) {
      perror("pthread_attr_getstacksize: ");
    }
    if (pthread_attr_destroy(&tattr) != 0) {
      perror("pthread_attr_init: ");
    }
    
    std::cout << "stacksize is now " << size << std::endl;
  }
};

#endif // OSDEPENDANT_H
