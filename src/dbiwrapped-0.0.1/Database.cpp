/// $Id: Database.cpp 53 2009-10-11 21:34:25Z mimo $
/**
 **	Database.cpp
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
#include <stdio.h>
#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <sstream>
#include <map>
#include <stdexcept>
#include <iostream>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <mysql/mysql.h>
#include <stdarg.h>
#include <dbi/dbi.h>
#endif

#include "IError.h"
#include "../dbierror.h"
#include "Database.h"


#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif

// bool Database::s_b_dbi_initialised = false;

Database::Mutex Database::s_mutex;

// Database::Database(const std::string& d, IError *e)
//     : m_errhandler(e)
//     , m_embedded(true)
//     , m_mutex(m_mutex)
//     , m_log_mutex()
//     , m_b_use_mutex(false) {
// }


// Database::Database(Mutex& m, const std::string& d, IError *e)
//     : m_errhandler(e)
//     , m_embedded(true)
//     , m_mutex(m)
//     , m_log_mutex()
//     , m_b_use_mutex(true) {
// }


Database::Database(const std::string& h, const std::string& u, const std::string& p, const std::string& d, IError *e)
    : m_errhandler(e)
    , m_embedded(false)
    , m_mutex(m_mutex)
    , m_log_mutex()
    , m_b_use_mutex(false) {
  m_dbparams.insert(dbparams_v::value_type("type", "mysql"));
  m_dbparams.insert(dbparams_v::value_type("host", h));
  m_dbparams.insert(dbparams_v::value_type("username", u));
  m_dbparams.insert(dbparams_v::value_type("password", p));
  m_dbparams.insert(dbparams_v::value_type("dbname", d));
}


Database::Database(Mutex& m, const std::string& h, const std::string& u, const std::string& p, const std::string& d, IError *e)
    : m_errhandler(e)
    , m_embedded(false)
    , m_mutex(m)
    , m_log_mutex()
    , m_b_use_mutex(true)
    , m_max_cons(0) {
  m_dbparams.insert(dbparams_v::value_type("type", "mysql"));
  m_dbparams.insert(dbparams_v::value_type("host", h));
  m_dbparams.insert(dbparams_v::value_type("username", u));
  m_dbparams.insert(dbparams_v::value_type("password", p));
  m_dbparams.insert(dbparams_v::value_type("dbname", d));
}
Database::Database(Mutex& m, const Database::dbparams_v v, unsigned spare) 
  : m_errhandler(0), m_embedded(false), m_mutex(m), m_log_mutex(), 
  m_b_use_mutex(true), m_max_cons(0), m_dbparams(v), m_spareconnections(spare) { }

Database::~Database() {
  Lock lck(m_mutex, m_b_use_mutex);
  for(ConnectionContainer::iterator itr = _free.begin(); itr != _free.end(); ++itr) {
    tracer << "closed " << (*itr) << " used: " << _used.size() << " free: " << _free.size() << std::endl;
    dbi_conn_close((*itr));
  }
  for(ConnectionContainer::iterator itr = _used.begin(); itr != _used.end(); ++itr) {
    std::cerr << "destroying Database object before Query object" << std::endl;
    dbi_conn_close((*itr));
  }
}

void Database::RegErrHandler(IError *p) {
  m_errhandler = p;
}


dbi_conn Database::grabdb() {
  Lock lck(m_mutex, m_b_use_mutex);

  if (Database::Initialiser::instance()._result < -1) {
    if (Database::Initialiser::instance()._result == 0) {
      throw std::runtime_error("no DBI drivers found");
    }
    throw std::runtime_error("failed to initialize DBI");
  }
  while (!_free.empty()) {
    ConnectionContainer::iterator itr = _free.begin();
    dbi_conn con = (*itr);
    _free.erase(itr);
    if (!dbi_conn_ping(con)) {
      dbi_conn_close(con);
//       std::cerr << "Error reusing existing connection" << std::endl;
      continue;
    }
    _used.insert(con);
    tracer << "reused " << con <<" used: " << _used.size() << " free: " << _free.size() << std::endl;
    return con;
  }
  const std::string &type = m_dbparams["type"];
  dbi_conn con = dbi_conn_new(type.c_str());
  if (!con) {
    throw Database::Exception("dbi_conn_new() failed, driver " + type);
  }
  
  for (dbparams_v::const_iterator itr = m_dbparams.begin(); itr != m_dbparams.end();++itr) {
    if (itr->first == "type") {
      continue;
    }
//     std::cout << itr->first << "=" << itr->second << " ";
    if (isNumericOption(type, itr->first)) {
//       std::cout << "numeric" << std::endl;
      if (dbi_conn_set_option_numeric(con, itr->first.c_str(), Utils::StrmConvert(itr->second)) != 0) {
	throw DBIError(DBIError::getDBIError(con), 
		      "dbi_conn_set_option failed for key " + itr->first); 
      }
    } else {
//       std::cout << "string" << std::endl;
      if (dbi_conn_set_option(con, itr->first.c_str(), itr->second.c_str()) != 0) {
	throw DBIError(DBIError::getDBIError(con), 
		      "dbi_conn_set_option failed for key " + itr->first); 
      }
    }
  }
  if (dbi_conn_connect(con) != 0) {
    throw DBIError(DBIError::getDBIError(con), 
		    "dbi_conn_connect failed");  
  }
  _used.insert(con);
  tracer << "inserted new " << con << " used: " << _used.size() << " free: " << _free.size() << std::endl;
  return con;
}


void Database::freedb(dbi_conn con) {
  Lock lck(m_mutex, m_b_use_mutex);
  ConnectionContainer::iterator itr = _used.find(con);
  if (itr == _used.end()) {
    throw std::logic_error("Freeing a db connection that isn't in use");
    return;
  }
  if (_free.size() > m_spareconnections) {
    dbi_conn_close((*itr));
    tracer << "closed " << con << " used: " << _used.size() << " free: " << _free.size() << std::endl;
  } else {
    _free.insert((*itr));
    tracer << "reinserted " << con << " used: " << _used.size() << " free: " << _free.size() << std::endl;
  }
  _used.erase(itr);
}


// void Database::error(const char *format, ...) {
//   if (m_errhandler) {
//     Lock lck(m_log_mutex, m_b_use_mutex);
//     int d;
//     char c;
//     char *s;
//     std::stringstream stm;
// 
// //     m_errhandler -> error(*this, format);
// //     return;
//     va_list ap;
// // 		char errstr[5000];
//     va_start(ap, format);
// #ifdef WIN32
// // 		vsprintf(errstr, format, ap);
// #else
// // 		vsnprintf(errstr, 5000, format, ap);
// #endif
//     while (*format) {
//       switch (*format++) {
// 	case 's':              /* string */
// 	  s = va_arg(ap, char *);
// 	  stm << s;
// 	  break;
// 	case 'd':              /* int */
// 	  d = va_arg(ap, int);
// 	  stm << d;
// 	  break;
// 	case 'c':              /* char */
// 	  /* need a cast here since va_arg only
// 	    takes fully promoted types */
// 	  c = (char) va_arg(ap, int);
// 	  stm << c;
// 	  break;
//       }
//     }
//     va_end(ap);
//     m_errhandler -> error(*this, stm.str());
//   }
// }
// 
// 
// void Database::error(Query& q, const char *format, ...) {
//   if (m_errhandler) {
//     Lock lck(m_log_mutex, m_b_use_mutex);
//     va_list ap;
//     char errstr[5000];
//     va_start(ap, format);
// #ifdef WIN32
//     vsprintf(errstr, format, ap);
// #else
//     vsnprintf(errstr, 5000, format, ap);
// #endif
//     va_end(ap);
//     m_errhandler -> error(*this, q, errstr);
//   }
// }


bool Database::Connected() {
  dbi_conn con = grabdb();
  if (!con) {
    return false;
  }
  int ping_result = dbi_conn_ping(con);
  freedb(con);
  return (ping_result == 1) ? true : false;
}


Database::Lock::Lock(Mutex& mutex, bool use) : m_mutex(mutex), m_b_use(use) {
//   if (m_b_use) {
    m_mutex.Lock();
//   }
}


Database::Lock::~Lock() {
//   if (m_b_use) {
    m_mutex.Unlock();
//   }
}


Database::Mutex::Mutex() {
#ifdef _WIN32
  m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
  pthread_mutex_init(&m_mutex, NULL);
#endif
}


Database::Mutex::~Mutex() {
#ifdef _WIN32
  ::CloseHandle(m_mutex);
#else
  pthread_mutex_destroy(&m_mutex);
#endif
}


void Database::Mutex::Lock() {
#ifdef _WIN32
  DWORD d = WaitForSingleObject(m_mutex, INFINITE);
  // %! check 'd' for result
#else
  pthread_mutex_lock(&m_mutex);
#endif
}


void Database::Mutex::Unlock() {
#ifdef _WIN32
  ::ReleaseMutex(m_mutex);
#else
  pthread_mutex_unlock(&m_mutex);
#endif
}

// std::string Database::quotestr(const std::string& str) {
//   OPENDB *odb = grabdb();
//   if (!odb)
//   {
//     return false;
//   }
//
//
// }
std::string Database::safestr(const std::string& str) {
  std::string str2;
  for (size_t i = 0; i < str.size(); i++) {
    switch (str[i]) {
      case '\'':
      case '\\':
      case 34:
	str2 += '\\';
      default:
	str2 += str[i];
    }
  }
  return str2;
}


std::string Database::unsafestr(const std::string& str) {
  std::string str2;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '\\') {
      i++;
    }
    if (i < str.size()) {
      str2 += str[i];
    }
  }
  return str2;
}


std::string Database::xmlsafestr(const std::string& str) {
  std::string str2;
  for (size_t i = 0; i < str.size(); i++) {
    switch (str[i]) {
      case '&':
	str2 += "&amp;";
	break;
      case '<':
	str2 += "&lt;";
	break;
      case '>':
	str2 += "&gt;";
	break;
      case '"':
	str2 += "&quot;";
	break;
      case '\'':
	str2 += "&apos;";
	break;
      default:
	str2 += str[i];
    }
  }
  return str2;
}


int64_t Database::a2bigint(const std::string& str) {
  int64_t val = 0;
  bool sign = false;
  size_t i = 0;
  if (str[i] == '-') {
    sign = true;
    i++;
  }
  for (; i < str.size(); i++) {
    val = val * 10 + (str[i] - 48);
  }
  return sign ? -val : val;
}


uint64_t Database::a2ubigint(const std::string& str) {
  uint64_t val = 0;
  for (size_t i = 0; i < str.size(); i++) {
    val = val * 10 + (str[i] - 48);
  }
  return val;
}


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

