/// $Id: cachedpatternmatch.h 27 2009-09-15 19:37:26Z mimo $
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

#ifndef CACHEDPATTERNMATCH_H
#define CACHEDPATTERNMATCH_H

#include <iostream>
#include <memory>
#include <boost/smart_ptr.hpp>
#include "triplet.h"
#include "core.h"
#include "datasource.h"
#include "regexcpp.h"
#include "cachehandler.h"


class CachedPatternMatch
{
//   class Row : public std::vector<std::string> {  };
  typedef  std::pair<RegEx, std::string >Row;
  class Data : public std::vector<Row> {  };
  struct Cache {
//     Cache() { std::cout << "Cache created" << std::endl; }
//     ~Cache() { std::cout << "Cache destroyed" << std::endl; }
    Data _data;
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
	std::string exp = query->getString(1);
	std::string val = query->getString(2); 
	cache->_data.push_back(Data::value_type(RegEx(exp), val));
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
//   RegEx _regex;//REG_ICASE (REG_EXTENDED|REG_ICASE)
  public:
    CachedPatternMatch(boost::shared_ptr<DataSource> ds, const std::string& column, Triplet::MemberType m,
		const std::string& returnfield) 
		: _ds(ds), _column(column), _member(m), _returnfield(returnfield) { }
    virtual bool match(Triplet& triplet, Core::Extra& extra) {
      boost::shared_ptr<CacheType> cache = HandlerType::instance().get(_ds.get());
      if (!cache) {
	Factory factory(_ds, _column, _returnfield);
	cache = HandlerType::instance().create(_ds.get(), factory);
      }
      unsigned row = 0;
      CacheType::RLock rl(cache.get());
      for(Data::const_iterator itr = rl->_data.begin(); itr != rl->_data.end(); ++itr) {
	if ((*itr).first.match(triplet.getMember(_member))) {
// 	  	  std::cout << "MATCH   " << triplet.getMember(_member) << std::endl;
	  extra.setComment((*itr).second);
	  return true;
	}
	row++;
      }
      return false;	
    }
    virtual ~CachedPatternMatch() { }
		
};

#endif // PATTERNMATCH_H
