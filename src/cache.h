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

#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <memory>
#include "triplet.h"
#include "core.h"
#include "datasource.h"

class Cache {
  DataSource* _ds;
  const std::string _tablename;
  typedef std::vector<std::string> ReturnFieldsType;
  typedef std::pair<std::string,std::string> WhereType;
  typedef std::vector<WhereType> WhereClausesType;
public:
  typedef enum { 
    init,
    first,
    iterating,
    end
  } StateType;
  Cache(DataSource* ds, const std::string& table) : _ds(ds), _tablename(table), 
    _index(0), _data(0) { }
  
  virtual DataSource::BaseQuery* createQuery() { return new CacheQuery(this); }
  
  void _addReturnField(const std::string& f) { _returnfields.push_back(f); }
  void _addWhere(const std::string &field, const std::string &val, const std::string &op ) {
    _whereclauses.push_back(WhereClausesType::value_type(field,val));
  }
  class CacheQuery : public DataSource::BaseQuery {
    Cache *_cache;
    unsigned _row, _num;
    Cache::StateType _state;
//     bool _end; 
  public:
    CacheQuery(Cache *cache) : _cache(cache), _row(0), _num(0), _state(Cache::init) { }
    virtual BaseQuery* addReturnField(const std::string &f) {
      _cache->_addReturnField(f);
      return this;
    }
    virtual BaseQuery* addWhere(const std::string &field, const std::string &val,
					const std::string &op ) {
      _cache->_addWhere(field, val, op);
      return this;
    }
    virtual BaseQuery* addWhere(const std::string &field, 
					unsigned long long val,
					const std::string &op ) {
      return this;
    }
    /* query with result */ 
    virtual void lookup() {  
      _cache->initCache();  
      _cache->_lookup(_row, _num, _state);
    }
    virtual unsigned long numRows() { return  _num; }
    virtual unsigned long numCols() { return 0; }    
    // returns false if there is no row
    virtual bool fetchRow() { return _cache->_fetchRow(_row, _num, _state); }
    virtual unsigned long getNumeric(unsigned) { return  0; }
    virtual unsigned long long getBigNumeric(unsigned)  { return  0; }
    virtual const std::string getString(unsigned idx)  { 
      return  _cache->_getString(_row, idx);
    }

    /** inserts **/					
    virtual BaseQuery* addString(const std::string &field, const std::string &val) {      
      return this;
    }
    
    virtual BaseQuery* addNumeric(const std::string &field, unsigned long val) {      
      return this;
    }
    virtual void insert() { }

    /** update **/
    virtual void addIncrement(const std::string &field, unsigned) { }
    virtual void addSet(const std::string &field, unsigned) { }
    virtual void update() { }

  };  
  virtual ~Cache() { 
    /// TODO mutex
    Utils::zap(_data);
    Utils::zap(_index);
  };
  
  class Index : public std::map<std::string, unsigned> {
  };
  class Row : public std::vector<std::string> {
  };
  class Data : public std::vector<Row> {
  };
  void initCache() {
    if (_data) {
      return;
    }
    // Mutex
    // ?_data
    // return 
    std::auto_ptr<DataSource::BaseQuery> query( _ds->createQuery());
    query->addReturnField("*");
    query->lookup();
    _data = new Data;
    _data->reserve(query->numRows());
    unsigned count = 0;
    unsigned cols = query->numCols();
    Utils::zap(_index);
    _index = (_whereclauses.empty() ? 0 : new Index);
    unsigned indexIdx = (_whereclauses.empty() ? cols : 0);
    while(query->fetchRow()) {
      assert(count < query->numRows());
      Row newRow;
      newRow.reserve(cols);
      for(unsigned i=0; i < cols; i++) {
	std::string val = query->getString(i+1); 
	if (i == indexIdx) {
	  _index->insert(Index::value_type(val, count));
	}
	newRow.push_back(val);
      }
      _data->push_back(newRow);
      count++;
    }
    dumpData();
    dumpIndex();
  }
  void _lookup(unsigned &row, unsigned &num, StateType &state) {
    row = num = 0;
    if ((!_index) || _whereclauses.empty() ) { // full select
      row = 0;
      state = first;
      num = _data->size();
      return;
    }
    WhereClausesType::const_iterator wtr = _whereclauses.begin();
    Index::const_iterator itr = _index->find((*wtr).second);
    if (itr == _index->end()) {
      state = end;
    }
    row = (*itr).second;
    state = first;
    num = 1;
  }
  bool _fetchRow(unsigned &row, unsigned num, StateType &state) {
    if (state == end) {
      return false;
    }
    if (state == first) {
      state = iterating;
      return true;
    }
    if (state == iterating) {
      if (num == 1) {
	state = end;
	return true;
      }
      if (row < num-1) {
	row++;
      } else {
	state = end;
      }
    }
  }
  virtual const std::string& _getString(unsigned row, unsigned idx)  { 
    return _data->at(row)[idx-1];
  }  
  void dumpData() const {
    if(!_data) {
      return;
    }
    unsigned row = 0;
    for(Data::const_iterator itr = _data->begin(); itr != _data->end(); ++itr) {
      std::cout << "#" << row << "\t";
      for(Row::const_iterator j = (*itr).begin(); j != (*itr).end(); ++j) {
	std::cout << (*j) << "\t";
      }
      std::cout << std::endl;
      row++;
    }
  }
  void dumpIndex() const {
    if(!_index) {
      return;
    }
    unsigned row = 0;
    for(Index::const_iterator itr = _index->begin(); itr != _index->end(); ++itr) {
      std::cout << (*itr).first <<  "\t" << (*itr).second << std::endl;
    }
  }
  private:
  Index* _index;
  Data* _data;
  ReturnFieldsType _returnfields;
  WhereClausesType _whereclauses;
};

#endif // CACHE_H
