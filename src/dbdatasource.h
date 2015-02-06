/// $Id: dbdatasource.h 72 2011-08-10 21:39:36Z mimo $
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

#ifndef DBDATASOURCE_H
#define DBDATASOURCE_H

#include <iostream>
#include <boost/smart_ptr.hpp>
// #include "dbiwrapped.h"
#include "Query.h"
#include "Database.h"
// replaced dbiwrapped.h
#include "dbierror.h"
#include "datasource.h"
#include "utils.h"

class DBDataSource
{
  boost::shared_ptr<Database> _db;
  const std::string _table;
public:
  DBDataSource(boost::shared_ptr<Database> db, const std::string& table) : _db(db), _table(table) { }
  virtual ~DBDataSource() { /*std::cout << "DBDataSource destroyed" << std::endl;*/}
  virtual DataSource::BaseQuery* createQuery() { return new DBQuery(_db, _table); }
  
  class DBQuery : public DataSource::BaseQuery {
    boost::shared_ptr<Database> _db;
    Query _query;
    std::string _table, _returnFields, _whereClause;
    Query::Arguments _args;
    bool _haveResult;
    std::string _insFields, _values;
    std::string _updFields;
  public:
    DBQuery(boost::shared_ptr<Database> db, const std::string& table) : _db(db), 
      _query(db), _table(table), _returnFields(), _whereClause(), _args(), 
      _haveResult(false), _insFields(), _values() { }
    ~DBQuery() {
      if (_haveResult) {
        _query.free_result();
      }
    }
    virtual void setTable(const std::string& t) { _table = t; }
    virtual BaseQuery* addReturnField(const std::string& field) {
      if (field == "*" ) {
        _returnFields = field;
        return this;
      }
      if (!_returnFields.empty()) {
        _returnFields += ",";
      }
      _returnFields += "`" + field + "`"; // TODO unless *
      return this;
    }
    virtual BaseQuery* addWhere(const std::string &field, const std::string& val,
					 const std::string& op) {
      if (!_whereClause.empty()) {
        _whereClause += " AND ";
      }
      _whereClause += "`" + field + "`" + op + "%s";
      _args.push_back(val);
      return this;
    }
    virtual BaseQuery* addWhere(const std::string &field, unsigned long long val,
				 const std::string& op) {
      if (!_whereClause.empty()) {
        _whereClause += " AND ";
      }
      _whereClause += "`" + field + "`" + op + "%d";
      _args.push_back(Utils::StrmConvert(val));
      return this;
    }
    virtual void lookup() { 
//       std::cout << _returnFields << std::endl;
//       std::cout << _whereClause << std::endl;
      std::string returnFields = (_returnFields.empty() ? "COUNT(*)" : _returnFields);
      std::string whereClause = (_whereClause.empty() ? "" : "` WHERE (" + _whereClause + ")");
      
      if (_query.get_resultf("SELECT " + returnFields + " FROM `" 
        + _table + whereClause, _args)) {
        _haveResult = true;
      }
    }
    virtual unsigned long numRows() { return _query.num_rows(); }
    virtual unsigned long numCols() { return _query.num_cols(); }
    virtual BaseQuery* addString(const std::string &field, const std::string &val) {
      if (!_insFields.empty()) {
        _insFields += ",";
        _values += ",";
      }
      _insFields += "`" + field + "`";
      _values += "%s";
      _args.push_back(val);
      return this;
    }
    virtual BaseQuery* addNumeric(const std::string &field, unsigned long val) {
      if (!_insFields.empty()) {
        _insFields += ",";
        _values += ",";
      }
      _insFields += "`" + field + "`";
      _values += "%d";
      _args.push_back(Utils::StrmConvert(val));
      return this;
    }
    virtual void insert() {
//       std::cout << _insFields << std::endl;
//       std::cout << _values << std::endl;
      ///TODO add try/catch for duplicate inserts (DBIError) 1062
      try {
        if (_query.executef("INSERT INTO " + _table + " (" + _insFields + ") "
          + " VALUES(" + _values + ")", _args)) {
          _haveResult = true;
        }
      } catch(DBIError &e) {
        if (e.getDBIErrorNumber() != 1062) {
          throw e;
        }
        ///TODO could log a warning
      }
    }
    virtual bool fetchRow() { return (_query.fetch_row() > 0); }
    virtual unsigned long getNumeric(unsigned idx) { 
      return _query.getlongval(idx);
    }
    virtual unsigned long long getBigNumeric(unsigned idx) { 
      return _query.getulonglongval(idx);
    }
    virtual const std::string getString(unsigned idx) {
      return _query.getstr(idx);
    }
    virtual void addIncrement(const std::string &field, unsigned inc) {
      if (!_updFields.empty()) {
        _updFields += ",";
      }
      _updFields += "`" + field + "`=`" + field + "`+%d";
      _args.push_back(Utils::StrmConvert(inc));
    }
    virtual void addSet(const std::string &field, unsigned val) {
      if (!_updFields.empty()) {
        _updFields += ",";
      }
      _updFields += "`" + field + "`=%d";
      _args.push_back(Utils::StrmConvert(val));
    }
    virtual void update() {
//       std::cout << _updFields << std::endl;
//       std::cout << _whereClause << std::endl;
      if (_query.executef("UPDATE " + _table + " SET " 
        + _updFields + " WHERE(" + _whereClause + ")", _args)) {
        _haveResult = true;
      }
    }
    
  };
};

#endif // DBDATASOURCE_H
