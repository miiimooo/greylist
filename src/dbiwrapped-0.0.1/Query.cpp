/// $Id: Query.cpp 39 2009-09-22 23:25:30Z mimo $
/**
 **	Query.cpp
 **
 **	Published / author: 2001-02-15 / grymse@alhem.net
 **/

/*
Copyright (C) 2001  Anders Hedstrom

This program is made available under the terms of the GNU GPL.

If you would like to use this program in a closed-source application,
a separate license agreement is available. For information about
the closed-source license agreement for this program, please
visit http://www.alhem.net/sqlwrapped/license.html and/or
email license@alhem.net.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <mysql/mysql.h>
#include <dbi/dbi.h>
#endif

#include "Database.h"
#include "Query.h"
#include "../dbierror.h"


#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


// Query::Query(boost::shared_ptr<Database> dbin)
//     : m_db(dbin)
//     , odb(dbin ? dbin -> grabdb() : NULL)
//     , res(NULL)
//     , row()
//     , m_num_cols(0) {
// }


Query::Query(boost::shared_ptr<Database> dbin) : m_db(dbin), con(dbin->grabdb()), res(NULL), row()
    , m_num_cols(0) {
}


// Query::Query(boost::shared_ptr<Database> dbin, const std::string& sql) : m_db(*dbin)
//     , odb(dbin ? dbin -> grabdb() : NULL), res(NULL), row()
//     , m_num_cols(0) {
//   execute(sql);
// }


Query::Query(boost::shared_ptr<Database> dbin, const std::string& sql) : m_db(dbin), 
  con(dbin->grabdb()), res(NULL), row() , m_num_cols(0) {
  execute(sql); // returns 0 if fail
}


Query::~Query() {
  if (res) {
    std::cerr << "mysql_free_result in destructor" << std::endl;
//     throw std::logic_error("mysql_free_result in destructor");
    dbi_result_free(res);
  }
  if (con) {
    m_db->freedb(con);
  }
}


boost::shared_ptr<Database> Query::GetDatabase() const {
  return m_db;
}


bool Query::execute(const std::string& sql) {		// query, no result
  m_last_query = sql;
  if (con && res) {
    throw std::logic_error("execute: query busy");
  }
  if (con && !res) {
    if ((res = dbi_conn_query(con, sql.c_str())) == NULL) {
      throw DBIError(DBIError::getDBIError(con), "query failed");
    } else {
      return true;
    }
  }
  return false;
}
bool Query::executef(const std::string& sql, const Query::Arguments &args) {  // query, no result
  m_last_query = sql;
  if (con && res) {
    throw std::logic_error("executef: query busy");
  }
  if (con && !res) {
    std::string quoted_sql;
    Arguments::const_iterator arg = args.begin();
    bool bInside = false;
    for (std::string::const_iterator itr = sql.begin();itr != sql.end(); itr++) {
      if (!bInside) {
	if (*itr != '%') {
	  quoted_sql += *itr;
	  continue;
	}
	bInside = true;
	continue;
      }
      if (*itr == '%') {
	quoted_sql += *itr;
	bInside = false;
	continue;
      }
      /// for the moment we ignore whether it's %s, %d, %i
      if (arg == args.end()) {
	throw std::invalid_argument("executef: not enough arguments");
      }
      switch (*itr) {
	case 's': {
	    char *psz = NULL;
	    dbi_conn_quote_string_copy(con, (*arg).c_str(), &psz);
	    if (!psz) {
	      //         GetDatabase().error(*this, "dbi_conn_quote_string_copy ");
	      throw std::runtime_error("dbi_driver_quote_string_copy " + (*arg));
	    }
	    quoted_sql.append(psz);
	    arg++;
	    if (psz) {
	      free(psz);
	      psz = NULL;
	    }
	  }
	  bInside = false;
	  break;
	default:
	case 'd':
	  quoted_sql.append(*arg);
	  arg++;
	  bInside = false;
	  break;
      }
    }
    m_last_query = quoted_sql;
    if ((res = dbi_conn_query(con, quoted_sql.c_str())) == NULL) {
      throw DBIError(DBIError::getDBIError(con), "query failed");
    } else {
      return true;
    }
  }
  return false;
}



// methods using db specific api calls

dbi_result Query::get_result(const std::string& sql) {	// query, result
  if (con && res) {
    throw std::logic_error("get_result: query busy");
  }
  if (con && !res) {
    if (execute(sql)) {
// 			res = mysql_store_result(&odb -> mysql); /// res gets stored by execute
      if (res) {
	m_num_cols = dbi_result_get_numfields(res);
	for (int i = 1; i <= m_num_cols; i++) {
	  const char *name = dbi_result_get_field_name(res, i);
	  if (name != NULL) {
	    m_nmap[name] = i;
	  }
	  unsigned short type = dbi_result_get_field_type_idx(res, i);
	  m_tmap[i] = type;
	}
      }
    }
  }
  return res;
}
dbi_result Query::get_resultf(const std::string& sql, const Query::Arguments &args) {
  if (con && res) {
    throw std::logic_error("get_result: query busy");
  }
  if (con && !res) {
    executef(sql, args);
    if (res) {
      m_num_cols = dbi_result_get_numfields(res);
      for (int i = 1; i <= m_num_cols; i++) {
	const char *name = dbi_result_get_field_name(res, i);
	if (name != NULL) {
	  m_nmap[name] = i;
	}
	unsigned short type = dbi_result_get_field_type_idx(res, i);
	m_tmap[i] = type;
      }
    }
  }
  return res;

}


void Query::free_result() {
  if (con && res) {
    dbi_result_free(res);
    res = NULL;
    row = 0;
  }
//   while (!m_nmap.empty()) { /// shouldn't his be m_nmap.clear()?
//     std::map<std::string, int>::iterator it = m_nmap.begin();
//     m_nmap.erase(it);
//   }
  m_nmap.clear();
  m_tmap.clear();
  m_num_cols = 0;
}


bool Query::fetch_row() {
  rowcount = 1; /// libdbi starts at 1
  return con && res ? ((row = dbi_result_next_row(res)) == 1) : false;
// 	return row;
// 	return odb && res ? row = mysql_fetch_row(res) : NULL;
}


unsigned long long  Query::insert_id() {
  return con && res ? dbi_conn_sequence_last(con, NULL) : 0; //hmmmm sequence name??
}


long Query::num_rows() {
  return con && res ? dbi_result_get_numrows(res) : 0;
}


int Query::num_cols() {
  return m_num_cols;
}


// bool Query::is_null(int x)
// {
// 	if (odb && res && (row.size() > x))
// 	{
// // 		return row[x] ? false : true;
// 	}
// 	return false; // ...
// }


// bool Query::is_null(const std::string& x)
// {
// 	int index = m_nmap[x] - 1;
// 	if (index >= 0)
// 		return is_null(index);
// 	error("Column name lookup failure: " + x);
// 	return false;
// }
//
//
// bool Query::is_null()
// {
// 	return is_null(rowcount++);
// }


// const char *Query::getstr(const std::string& x)
// const std::string &Query::getstr(const std::string& x)
// {
// 	int index = m_nmap[x] - 1;
// 	if (index >= 0)
// 		return getstr(index);
// 	error("Column name lookup failure: " + x);
// 	return NULL;
// }


// const std::string &Query::getstr(int x)
const char *Query::getstr(int x) {
  if (con && res && row) {
    const char *str = NULL;
    switch (m_tmap[x]) {
      case DBI_TYPE_STRING:
	str = dbi_result_get_string_idx(res, x);
	return str ? str : "";
      default:
// 			GetDatabase().error(*this, "unknown type idx=%d type=%d", x, m_tmap[x]);
	return "";
    }
// 		return row[x] ? row[x] : "";
// 		return row[x];
  }
  return NULL;
}


const char *Query::getstr() {
  return getstr(rowcount++);
}


// double Query::getnum(const std::string& x)
// {
// 	int index = m_nmap[x] - 1;
// 	if (index >= 0)
// 		return getnum(index);
// 	error("Column name lookup failure: " + x);
// 	return 0;
// }


// double Query::getnum(int x)
// {
// 	return odb && res && (row.size() > x) && row[x] ? atof(row[x]) : 0;
// }


/*long Query::getval(const std::string& x)
{
      int index = m_nmap[x] - 1;
      if (index >= 0)
	      return getval(index);
      error("Column name lookup failure: " + x);
      return 0;
}

*/
int Query::getval(int x) {
  return con && res && row && (m_tmap[x] == DBI_TYPE_INTEGER) ?
	 dbi_result_get_int_idx(res, x) : 0;
}

/*
double Query::getnum()
{
      return getnum(rowcount++);
}
*/

int Query::getval() {
  return getval(rowcount++);
}

long Query::getlongval(int x) {
  return con && res && row && (m_tmap[x] == DBI_TYPE_INTEGER) ?
	 dbi_result_get_long_idx(res, x) : 0;
}
long Query::getlongval() {
  return getlongval(rowcount++);
}
unsigned long long Query::getulonglongval(int x) {
  return con && res && row && (m_tmap[x] == DBI_TYPE_INTEGER) ?
	 dbi_result_get_ulonglong_idx(res, x) : 0;
}
unsigned long long Query::getulonglongval() {
  return getulonglongval(rowcount++);
}


/*
unsigned long Query::getuval(const std::string& x)
{
      int index = m_nmap[x] - 1;
      if (index >= 0)
	      return getuval(index);
      error("Column name lookup failure: " + x);
      return 0;
}


unsigned long Query::getuval(int x)
{
      unsigned long l = 0;
      if (odb && res && (row.size() > x) && row[x])
      {
	      l = m_db.a2ubigint(row[x]);
      }
      return l;
}


unsigned long Query::getuval()
{
      return getuval(rowcount++);
}


int64_t Query::getbigint(const std::string& x)
{
      int index = m_nmap[x] - 1;
      if (index >= 0)
	      return getbigint(index);
      error("Column name lookup failure: " + x);
      return 0;
}


int64_t Query::getbigint(int x)
{
      return odb && res && (row.size() > x) && row[x] ? m_db.a2bigint(row[x]) : 0;
}


int64_t Query::getbigint()
{
      return getbigint(rowcount++);
}


uint64_t Query::getubigint(const std::string& x)
{
      int index = m_nmap[x] - 1;
      if (index >= 0)
	      return getubigint(index);
      error("Column name lookup failure: " + x);
      return 0;
}


uint64_t Query::getubigint(int x)
{
      return odb && res && (row.size() > x) && row[x] ? m_db.a2ubigint(row[x]) : 0;
}


uint64_t Query::getubigint()
{
      return getubigint(rowcount++);
}


double Query::get_num(const std::string& sql)
{
      double l = 0;
      if (get_result(sql))
      {
	      if (fetch_row())
	      {
		      l = getnum();
	      }
	      free_result();
      }
      return l;
}
*/

long Query::get_count(const std::string& sql) {
  long l = 0;
  if (get_result(sql)) {
    if (fetch_row())
      l = getval();
    free_result();
  }
  return l;
}


const char *Query::get_string(const std::string& sql) {
  bool found = false;
  m_tmpstr = "";
  if (get_result(sql)) {
    if (fetch_row()) {
      m_tmpstr = getstr();
      found = true;
    }
    free_result();
  }
  return m_tmpstr.c_str(); // %! changed from 1.0 which didn't return NULL on failed query
}


const std::string& Query::GetLastQuery() {
  return m_last_query;
}


std::string Query::GetError() {
  if (!con) {
    return "";
  }
  if (dbi_conn_error_flag(con) == 0) {
    return "";
  }
//   return "DB Error";
  const char *strErr;
  if (dbi_conn_error(con, &strErr) == -1) {
    return "";
  }
  return std::string(strErr);
}


int Query::GetErrno() {
  return con ? dbi_conn_error_flag(con) : 0;
}


bool Query::Connected() {
  if (con) {
    if (!dbi_conn_ping(con)) {
//       GetDatabase().error(*this, "dbi_conn_ping() failed");
      return false;
    }
  }
  return con ? true : false;
}


// void Query::error(const std::string& x) {
//   m_db.error(*this, x.c_str());
// }


std::string Query::safestr(const std::string& x) {
  return m_db->safestr(x);
}


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

