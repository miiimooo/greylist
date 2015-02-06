/// $Id: cacherefresh.h 63 2009-10-18 18:52:36Z mimo $
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

#ifndef CACHEREFRESH_H
#define CACHEREFRESH_H

#include <map>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include "defensive.h"
#include "singleton.h"

class CacheRefresh /*: public Templates::Singleton<CacheRefresh>*/
{
//   friend class Templates::Singleton<CacheRefresh>;
  typedef boost::function0<void> FunctionType;
  typedef std::map<unsigned,FunctionType> ContainerType;
  ContainerType _functions;
  boost::mutex _mutex;
  unsigned _countid;
public:
  struct Proxy : public Templates::Singleton<Proxy> {
    friend class Templates::Singleton<Proxy>;
    boost::mutex _mutex;
    boost::shared_ptr<CacheRefresh> _cacheRefresh;
    boost::shared_ptr<CacheRefresh> get() {
      if (_cacheRefresh) {
	return _cacheRefresh;
      }
      boost::mutex::scoped_lock lock(_mutex);
      if (_cacheRefresh) {
	return _cacheRefresh;
      }
      _cacheRefresh.reset(new CacheRefresh);
      return _cacheRefresh;
    }
  };
  CacheRefresh() : _countid(0) { }
  unsigned registerHandler(FunctionType f) {
    boost::mutex::scoped_lock lock(_mutex);
    unsigned pos = ++_countid;
    bool result = _functions.insert(ContainerType::value_type(pos, f)).second;
    tracer << "new cache id=" << pos << std::endl;
    return (result ? pos : 0);
  }
  void run() {
    tracer << "CacheRefresh running, size=" << _functions.size()  <<  std::endl;
    boost::mutex::scoped_lock lock(_mutex);
    for(ContainerType::iterator itr = _functions.begin(); itr != _functions.end(); ++itr) {
      ((*itr).second)();
    }
  }
  size_t size() {
    boost::mutex::scoped_lock lock(_mutex);
    return _functions.size();
  }
  void unregisterHandler(unsigned id) {
    boost::mutex::scoped_lock lock(_mutex);
    _functions.erase(id);
    tracer << "rm cache id=" << id << std::endl;
  }
};

#endif // CACHEREFRESH_H
