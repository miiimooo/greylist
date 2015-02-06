/// $Id: cachehandler.h 62 2009-10-14 00:15:12Z mimo $
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

#ifndef CACHEHANDLER_H
#define CACHEHANDLER_H

#include <map>
#include <boost/bind.hpp>
#include "defensive.h"
#include "utils.h"
#include "datasource.h"
#include "readwriteable.h"
#include "singleton.h"
#include "tokenlist.h"
#include "cacherefresh.h"

template <class CacheType, class FactoryType>
class CacheHandler : public Templates::Singleton<CacheHandler<CacheType, FactoryType> >
{
  friend class Templates::Singleton<CacheHandler>;
  typedef std::map<const DataSource*, boost::shared_ptr<CacheType> > ContainerType;
  // a lockable container
  typedef Templates::RWLockable< ContainerType >CacheStorageType;
  CacheStorageType _caches;
  unsigned _id;
  boost::shared_ptr<CacheRefresh> _backend;
public:
//   typedef typename CacheType::Factory FactoryType;
  typedef CacheStorageType RWType;
  typedef typename CacheStorageType::WLock CacheWriterLock;
  CacheHandler() :  _caches(new ContainerType),
    _backend(CacheRefresh::Proxy::instance().get()) { 
    _id = _backend->registerHandler(boost::bind(&CacheHandler::clear, this));
  }
  virtual ~CacheHandler() { 
    _backend->unregisterHandler(_id);
  }
  
  void clear() {
    tracer << "CacheHandler clear called" << std::endl;
    typename CacheStorageType::WLock wl(&_caches);
    wl->clear();
  }
  // create or find a cache based on the handle DataSource
  boost::shared_ptr<CacheType> create(const DataSource *ds, const FactoryType &factory) {
    typename CacheStorageType::WLock wl(&_caches);
    typename ContainerType::iterator itr = wl->find(ds);
    if (itr != wl->end()) {
//       (*itr).second.swap( c );
      return (*itr).second;
    }
    boost::shared_ptr<CacheType> c(factory.create());
    wl->insert(typename ContainerType::value_type(ds, c));
    return c;
  }
  // returns either a shared ptr to the CacheType or an empty shared_ptr()
  boost::shared_ptr<CacheType> get(const DataSource* ds) {
    typename CacheStorageType::RLock rl(&_caches); // lock the container 
    typename ContainerType::iterator itr = rl->find(ds);
    return (itr != rl->end() ? boost::shared_ptr<CacheType>((*itr).second) 
      : boost::shared_ptr<CacheType>()); // or throw
  }
};

#endif // CACHEHANDLER_H
