/**   $Id: datasource.h 21 2009-09-05 23:07:59Z mimo $   **/
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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>

class DataSource {
public:
  DataSource() { };
  virtual ~DataSource() { };
  class BaseQuery {
  public:
    virtual ~BaseQuery() { }
//     virtual void setTable(const std::string&) = 0;
    // used for lookups
    /** select (update) **/
    virtual BaseQuery* addReturnField(const std::string &) = 0;
    virtual BaseQuery* addWhere(const std::string &field, 
					const std::string &val,
					const std::string &op ) = 0;
    virtual BaseQuery* addWhere(const std::string &field, 
					unsigned long long val,
					const std::string &op ) = 0;
    /* query with result */ 
    virtual void lookup() = 0;
    virtual unsigned long numRows() = 0;
    virtual unsigned long numCols() = 0;
    // returns false if there is no row
    virtual bool fetchRow() = 0;
    virtual unsigned long getNumeric(unsigned) = 0;
    virtual unsigned long long getBigNumeric(unsigned) = 0;
    virtual const std::string getString(unsigned) = 0;
    
    /** inserts **/					
    virtual BaseQuery* addString(const std::string &field, const std::string &val) = 0;
    virtual BaseQuery* addNumeric(const std::string &field, unsigned long val) = 0;
    virtual void insert() = 0;
    
    /** update **/
    virtual void addIncrement(const std::string &field, unsigned) = 0;
    virtual void addSet(const std::string &field, unsigned) = 0;
    virtual void update() = 0;
  };
  
  virtual BaseQuery* createQuery() = 0;
  
//   Query *
//   virtual void lookup(const std::string& table, const FieldList& resultfields, 
// 		       const FieldList& fields, const ValueList& value) = 0;
};
template <class BackendType>
class TmplDataSource : public DataSource {
  BackendType _backend;
public:
  TmplDataSource(boost::shared_ptr<class Database> db, const std::string& p1) : _backend(db, p1) { }
  TmplDataSource(boost::shared_ptr<DataSource> ds, const std::string& p1) : _backend(ds, p1) { }
  virtual ~TmplDataSource() { }
  virtual BaseQuery* createQuery() { return _backend.createQuery(); }
};

#endif // DATASOURCE_H
