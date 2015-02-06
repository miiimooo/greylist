/**
 **	IError.h
 **
 **	Published / author: 2004-06-11 / grymse@alhem.net
 **/

/*
Copyright (C) 2004  Anders Hedstrom

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

#ifndef _IERROR_H
#define _IERROR_H

#include <string>

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


class Database;
class Query;
// class Database::Mutex;
/** Log class interface. */
class IError
{
public:
//   IError(Database::Mutex& m)/* : m_mutex(m_mutex), m_b_use_mutex(false) {}*/;
//   IError()/* : m_mutex(m_mutex), m_b_use_mutex(false) */{}
	virtual void error(Database&,const std::string&) = 0;
	virtual void error(Database&,Query&,const std::string&) = 0;
private:
//   Database::Mutex &m_mutex;
//   bool m_b_use_mutex;

};


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // _IERROR_H
/**
 **	SysLog.h
 **
 **	Published / author: 2004-08-18 / grymse@alhem.net
 **/

/*
Copyright (C) 2004  Anders Hedstrom

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
#ifndef _SYSLOG_H
#define _SYSLOG_H
#ifndef WIN32

#include <syslog.h>

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


/** Log class writing to syslog. */
class SysLog : public IError
{
public:
	SysLog(const std::string& = "database", int = LOG_PID, int = LOG_USER);
	virtual ~SysLog();

	void error(Database&,const std::string&);
	void error(Database&,Query&,const std::string&);

};



#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // WIN32
#endif // _SYSLOG_H
/*
enum_t.h

Copyright (C) 2004  Anders Hedstrom

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
#ifndef _ENUM_T_H
#define _ENUM_T_H

#include <string>
#include <map>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
typedef unsigned __int64 uint64_t;
#define strcasecmp stricmp
#else
#include <stdint.h>
#endif

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


/** Implements MySQL ENUM datatype. */
class enum_t
{
public:
	enum_t(std::map<std::string, uint64_t>& );

	const std::string& String();
	unsigned short Value();
	const char *c_str();

	void operator=(const std::string& );
	void operator=(unsigned short);
	bool operator==(const std::string& );
	bool operator==(unsigned short);
	bool operator!=(const std::string& );

private:
	std::map<std::string, uint64_t>& m_mmap;
	std::map<unsigned short, std::string> m_vmap;
	unsigned short m_value;

};


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // _ENUM_T_H
/*
set_t.h

Copyright (C) 2004  Anders Hedstrom

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
#ifndef _SET_T_H
#define _SET_T_H

#include <string>
#include <map>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


/** Implements MySQL SET datatype. */
class set_t
{
public:
	set_t(std::map<std::string, uint64_t>& );

	const std::string& String();
	uint64_t Value();
	const char *c_str();

	bool in_set(const std::string& );

	void operator=(const std::string& );
	void operator=(uint64_t);
	void operator|=(uint64_t);
	void operator&=(uint64_t);
	void operator+=(const std::string& );
	void operator-=(const std::string& );

private:
	std::map<std::string, uint64_t>& m_mmap;
	uint64_t m_value;
	std::string m_strvalue;

};


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // _SET_T_H
/// $Id: Database.h 53 2009-10-11 21:34:25Z mimo $
/**
 **	Database.h
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

#ifndef _DATABASE_H
#define _DATABASE_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <dbi/dbi.h>
#endif
#include <string>
#include <list>
#include <set>
#include <map>
#include "../singleton.h"
#include <stdexcept>
#include "../utils.h"
#ifdef WIN32
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif
#include "../defensive.h"

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif

class IError;
class Query;
class Mutex;


/** Connection information and pooling. */
class Database {
  /** Mutex container class, used by Lock.
      \ingroup threading */
public:
  class Mutex {
  public:
    Mutex();
    ~Mutex();
    void Lock();
    void Unlock();
  private:
#ifdef _WIN32
    HANDLE m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif
  };
  /** Mutex helper class. */
private:
  class Lock {
  public:
    Lock(Mutex& mutex, bool use);
    ~Lock();
  private:
    Mutex& m_mutex;
    bool m_b_use;
  };
  struct Initialiser : public Templates::Singleton<Initialiser> {
    friend class Templates::Singleton<Initialiser>;
    int _result;
    Initialiser() {
      tracer << "dbi initialised" << std::endl;
      _result = dbi_initialize(0);
    }
    ~Initialiser() {
      if (_result != -1 ) {
	tracer << "dbi shutdown" << std::endl;
	dbi_shutdown();
      }
    }
  };
public:
  typedef std::set<dbi_conn> ConnectionContainer;
  ConnectionContainer _used, _free;
  typedef std::map<std::string, std::string> dbparams_v;
public:
  /** Use embedded libmysqld */
  Database(const std::string& database,
	   IError * = NULL);

  /** Use embedded libmysqld + thread safe */
  Database(Mutex& , const std::string& database,
	   IError * = NULL);

  /** Connect to a MySQL server */
  Database(const std::string& host,
	   const std::string& user,
	   const std::string& password = "",
	   const std::string& database = "",
	   IError * = NULL);

  /** Connect to a MySQL server + thread safe */
  Database(Mutex& , const std::string& host,
	   const std::string& user,
	   const std::string& password = "",
	   const std::string& database = "",
	   IError * = NULL);
	   
  /** the constructor we use **/	   
  Database(Mutex& , dbparams_v, unsigned spare=10);

  virtual ~Database();

  /** Callback after mysql_init */
//   virtual void OnMyInit(OPENDB *);

  /** try to establish connection with given host */
  bool Connected();

  void RegErrHandler(IError *);
  void error(Query&, const char *format, ...);

  /** Request a database connection.
  The "grabdb" method is used by the Query class, so that each object instance of Query gets a unique
  database connection. I will reimplement your connection check logic in the Query class, as that's where
  the database connection is really used.
  It should be used something like this.
  {
    Query q(db);
    if (!q.Connected())
	     return false;
    q.execute("delete * from user"); // well maybe not
  }

  When the Query object is deleted, then "freedb" is called - the database connection stays open in the
  m_opendbs vector. New Query objects can then reuse old connections.
  */
  dbi_conn grabdb();
  void freedb(dbi_conn );

  // use dbi quoting and escaping
//   std::string quotestr(const std::string& );


  // utility
  std::string safestr(const std::string&);
  std::string unsafestr(const std::string&);
  std::string xmlsafestr(const std::string&);

  int64_t a2bigint(const std::string&);
  uint64_t a2ubigint(const std::string&);

  unsigned long int getMaxCons() const {
    return m_max_cons;
  }
  
  struct Exception : public std::runtime_error {
    Exception(const std::string& str) : std::runtime_error(str) { }
  };
  static bool isNumericOption(const std::string &type, const std::string &key) {
    using namespace std;
    typedef map<string, set<string> > MapType;
    static MapType _map;
    
    if (_map.empty()) {
      set<string> mysql;
      mysql.insert("port");
      _map.insert(MapType::value_type("mysql", mysql));
      
      set<string> sqlite;
      mysql.insert("sqlite_timeout");
      _map.insert(MapType::value_type("sqlite", sqlite));
    
      set<string> sqlite3;
      mysql.insert("sqlite3_timeout");
      _map.insert(MapType::value_type("sqlite3", sqlite3));
    }
    if (_map.find(type) != _map.end()) {
      return (_map[type].find(key) != _map[type].end());
    }
    return false;
  }
private:
//   static bool s_b_dbi_initialised;
  Database(const Database&) : m_mutex(m_mutex) {}
  Database& operator=(const Database&) {
    return *this;
  }
  void error(const char *format, ...);
  //
// 	std::string dbtype;
// 	std::string host;
// 	std::string user;
// 	std::string password;
// 	std::string database;
  dbparams_v m_dbparams;
//   opendb_v m_opendbs;
  IError *m_errhandler;
  bool m_embedded;
  Mutex& m_mutex;
  Mutex m_log_mutex;
  bool m_b_use_mutex;
  unsigned long int m_max_cons;
  static Mutex s_mutex;
  unsigned m_spareconnections;
};

#ifdef MYSQLW_NAMESPACE
} //namespace MYSQLW_NAMESPACE {
#endif


#endif // _DATABASE_H
/*
 **	StderrLog.h
 **
 **	Published / author: 2004-08-18 / grymse@alhem.net
 **/

/*
Copyright (C) 2004  Anders Hedstrom

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
#ifndef _STDERRLOG_H
#define _STDERRLOG_H

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif

class Database;
/** Log class writing to stderr. */
class StderrLog : public IError
{
public:
//   StderrLog(Database::Mutex& m) : m_mutex(m_mutex), m_b_use_mutex(false) {};
//   StderrLog() : m_mutex(m_mutex), m_b_use_mutex(false) {}
	void error(Database&,const std::string&);
	void error(Database&,Query&,const std::string&);
private:
//   Database::Mutex &m_mutex;
//   bool m_b_use_mutex;
};



#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif


#endif // _STDERRLOG_H
/// $Id: Query.h 39 2009-09-22 23:25:30Z mimo $
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
/*
 **	Query.h
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

#ifndef _QUERY_H
#define _QUERY_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <boost/smart_ptr.hpp>
#include <dbi/dbi.h>

#ifdef WIN32
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif


#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


/** SQL Statement execute / result set helper class. */
class Query 
{
public:	
  /// TODO rewrite this to get error strings from dbi
  class Exception : public std::runtime_error {
  public:
    Exception(const std::string& e) : std::runtime_error(e) {}
  };
//   class DBIException : public std::runtime_error {
//     dbi_result _dbiResult;
//     public:
//       DBIException(dbi_result dbires, const std::string& e) 
// 	: _dbiResult(dbires), std::runtime_error(e) {}
//   };
  // 	class Row {
// 	public:
// 		std::string &operator [] (int i)  {
// 			return m_map[i];
// 		}
// 	private:
// 		std::map<unsigned,const std::string> m_map;
// 	};
// 	typedef std::map<unsigned,const std::string> Row;
	typedef unsigned long long Row;
  typedef std::vector<std::string>Arguments;
public:
	/** Constructor accepting reference to database object. */
	Query(boost::shared_ptr<class Database> dbin);
	/** Constructor accepting reference to database object
		and query to execute. */
	Query(boost::shared_ptr<class Database> dbin,const std::string& sql);
	~Query();

	/** Check to see if database object is connectable. */
	bool Connected();
	/** Return reference to database object. */
	boost::shared_ptr<class Database> GetDatabase() const;
	/** Return string of last query executed. */
	const std::string& GetLastQuery();

	/** execute() returns true if query is successful,
		does not store result */
	bool execute(const std::string& sql);
  /** execute query after replacing any string spaceholders
    with what is in the map **/
  bool executef(const std::string& sql, const Arguments &);
	/** execute query and store result. */
// 	MYSQL_RES *get_result(const std::string& sql);
	dbi_result get_result(const std::string& sql);

  /** execute query after replacing any string spaceholders
    with what is in the map **/
  dbi_result get_resultf(const std::string& sql, const Arguments &);

	/** free stored result, must be called after get_result() */
	void free_result();
	/** Fetch next result row.
		\return false if there was no row to fetch (end of rows) */
	bool fetch_row();
	/** Get id of last insert. */
	unsigned long long insert_id();
	/** Returns number of rows returned by last select call. */
	long num_rows();
	/** Number of columns in current result. */
	int num_cols();
	/** Last error string. */
	std::string GetError();
	/** Last error code. */
	int GetErrno();

	/** Check if column x in current row is null. */
/*	bool is_null(const std::string& x);
	bool is_null(int x);
	bool is_null();
*/
	/** Execute query and return first result as a string. */
	const char *get_string(const std::string& sql);
	/** Execute query and return first result as a long integer. */
	long get_count(const std::string& sql);
	/** Execute query and return first result as a double. */
// 	double get_num(const std::string& sql);

// 	const char *getstr(const std::string& x);
// 	const std::string &getstr(const std::string& x);
	const char *getstr(int x);
// 	const std::string &getstr(int x);
	const char *getstr();
// 	long getval(const std::string& x);
	int getval(int x);
	int getval();
  long getlongval(int x);
  long getlongval();
  unsigned long long getulonglongval(int x);
  unsigned long long getulonglongval();
// 	unsigned long getuval(const std::string& x);
// 	unsigned long getuval(int x);
// 	unsigned long getuval();
// 	int64_t getbigint(const std::string& x);
//  	int64_t getbigint(int x);
// 	int64_t getbigint();
// 	uint64_t getubigint(const std::string& x);
// 	uint64_t getubigint(int x);
// 	uint64_t getubigint();
// 	double getnum(const std::string& x);
// 	double getnum(int x);
// 	double getnum();

	std::string safestr(const std::string& x);

protected:
	Query(Database *dbin);
	Query(Database *dbin,const std::string& sql);
private:
	Query(const Query& q) : m_db(q.GetDatabase()) {}
	Query& operator=(const Query& ) { return *this; }
	void error(const std::string& );
	boost::shared_ptr<Database> m_db;
	dbi_conn con;
// 	Database::OPENDB *odb;
// 	MYSQL_RES *res;
	dbi_result res;
// 	MYSQL_ROW row;
	Row row;
	short rowcount;
	std::string m_tmpstr;
	std::string m_last_query;
	std::map<std::string,int> m_nmap;
	std::map<int,unsigned short> m_tmap;
	int m_num_cols;
};
// helper templates for converting numerics to string to pass to Query::Arguments
template<typename T, typename F > T StrmConvert ( const F from )
{
    std::stringstream temp;
    temp << from;
    T to = T();
    temp >> to;
    return to;
}

template<typename F> std::string StrmConvert ( const F from ) {
    return StrmConvert<std::string> ( from );
}


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // _QUERY_H
