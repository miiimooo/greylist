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

#ifndef SHAREDCACHE_H
#define SHAREDCACHE_H

#include <boost/thread/shared_mutex.hpp>

class SharedCache
{
  boost::shared_mutex _access;
public:
  class ReaderLock {
    boost::shared_lock<boost::shared_mutex> _lock;
  public:
    ReaderLock(SharedCache* cache) : _lock(cache->_access) { }  
  };
  class WriterLock {
    boost::upgrade_lock<boost::shared_mutex> _uglock;
    boost::upgrade_to_unique_lock<boost::shared_mutex> _unLock;
  public:
    WriterLock(SharedCache* cache) : _uglock(cache->_access), _unLock(_uglock) { }  
  };
  virtual ~SharedCache() { }
private:  
  friend class SharedCache::ReaderLock;
  friend class SharedCache::WriterLock;
  
};
class SharedStringCache : public SharedCache {
public:
  std::string _text;
};

#endif // SHAREDCACHE_H
