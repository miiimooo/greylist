/**   $Id: readwriteable.h 23 2009-09-06 20:05:06Z mimo $   **/

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

#ifndef READWRITEABLE_H
#define READWRITEABLE_H

#include <boost/smart_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace Templates {

template <class T>
class RWLockable
{
public:
  typedef boost::shared_ptr<T> PtrType; 
private:
  struct Proxy : PtrType {
    boost::shared_mutex _access;
    friend class RWLockable::RLock;
    friend class RWLockable::WLock;
  public:
    Proxy(T * m) : PtrType(m) { }
  };
  typedef boost::shared_ptr<Proxy> ProxyPtr;   
  ProxyPtr _proxy;
public:
  RWLockable(T *m) : _proxy(new Proxy(m)) { }
  RWLockable(const RWLockable& c) : _proxy(c._proxy) { }
  RWLockable& operator=(const RWLockable& c) { 
    _proxy = c._proxy;
    return *this;
  }
  
  class RLock {
    boost::shared_lock<boost::shared_mutex> _lock;
    ProxyPtr _proxy;
  public:
    RLock(RWLockable<T> *m) : _lock(m->_proxy->_access), _proxy(m->_proxy) { }
    T* operator->() const {
      return _proxy->get();
    }
  };
  
  class WLock {
    boost::upgrade_lock<boost::shared_mutex> _uglock;
    boost::upgrade_to_unique_lock<boost::shared_mutex> _unLock;
    ProxyPtr _proxy;
  public:
    WLock(RWLockable<T> *m) : _uglock(m->_proxy->_access), _unLock(_uglock), _proxy(m->_proxy) { }  
    T* operator->() const {
      return _proxy->get();
    }
  };
  
  virtual ~RWLockable() { } //not sure what happens here
private:  

    
};
};
#endif // READWRITEABLE_H
