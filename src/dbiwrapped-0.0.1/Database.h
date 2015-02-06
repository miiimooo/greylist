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
