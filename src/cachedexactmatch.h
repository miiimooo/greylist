/// $Id: cachedexactmatch.h 48 2009-10-10 00:51:03Z mimo $
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

#ifndef CACHEDEXACTMATCH_H
#define CACHEDEXACTMATCH_H

#include <iostream>
#include <memory>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "triplet.h"
#include "core.h"
#include "datasource.h"
#include "cachehandler.h"
#include "readwriteable.h"

class CachedExactMatch
{
  class Index : public std::map<std::string, unsigned> {  };
//   class Row : public std::vector<std::string> {  };
  typedef  std::string Row;
  class Data : public std::vector<Row> {  };
  struct Cache {
    Cache() { tracer << "Cache created" << std::endl; }
    ~Cache() { tracer << "Cache destroyed" << std::endl; }
    Data _data;
    Index _index;
  };
  typedef Templates::RWLockable<Cache> CacheType;
  
  struct Factory {
    boost::shared_ptr<DataSource> _ds;
    const std::string _column, _returnfield;
    Factory(boost::shared_ptr<DataSource> ds, const std::string &column, 
	     const std::string& returnfield) 
	     : _ds(ds), _column(column), _returnfield(returnfield) { }
    CacheType *create() const {
      std::auto_ptr<DataSource::BaseQuery> query( _ds->createQuery());
      query->addReturnField(_column); // add the index column
      query->addReturnField(_returnfield);
      query->lookup();
      Cache * cache = new Cache;
      cache->_data.reserve(query->numRows());
      unsigned count = 0;
      unsigned cols = query->numCols();
      while(query->fetchRow()) {
	assert(count < query->numRows());
	std::string key = query->getString(1);
	std::string val = query->getString(2); 
	Utils::lowercase(key);
	Utils::lowercase(val);
	cache->_index.insert(Index::value_type(key, count));
	cache->_data.push_back(val);
	count++;
      }
      return new CacheType(cache);
    }
  };
  typedef CacheHandler<CacheType, Factory> HandlerType;
  
  boost::shared_ptr<DataSource> _ds;
  const std::string _column;
  Triplet::MemberType _member;
  const std::string _returnfield;
public:
  CachedExactMatch(boost::shared_ptr<DataSource> ds, const std::string& column, 
		    Triplet::MemberType m, const std::string& returnfield) 
    : _ds(ds), _column(column), _member(m), _returnfield(returnfield) { }
  virtual bool match(Triplet& triplet, Core::Extra& extra) {
    boost::shared_ptr<CacheType> cache = HandlerType::instance().get(_ds.get());
    if (!cache) {
      Factory factory(_ds, _column, _returnfield);
      cache = HandlerType::instance().create(_ds.get(), factory);
    }
    CacheType::RLock rl(cache.get());
    Index::const_iterator itr = rl->_index.find(triplet.getMember(_member));
    if (itr == rl->_index.end()) {
      return false;
    }
    extra.setComment(rl->_data[(*itr).second]); //todo could return the id too..
//     std::cout << "Match " << rl->_data[(*itr).second] << " " << (*itr).second << std::endl;
    return true;
  }
  virtual ~CachedExactMatch() { }
};

#endif // EXACTMATCH_H
