/// $Id: dbierror.h 64 2010-08-12 21:58:53Z mimo $
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

#ifndef DBIERROR_H
#define DBIERROR_H

#include <stdexcept>
#include <dbi/dbi.h>

class DBIError : public std::runtime_error {
public:
  struct Data {
    int _dbierrno;
    std::string _dbierror;
    Data(int e, const std::string & errstr) : _dbierrno(e), _dbierror(errstr) { }
  };
  DBIError(DBIError::Data data, const std::string& str) 
    : std::runtime_error(str + ": " + data._dbierror), 
    _data(data) { }
  virtual ~DBIError() throw() { }
  static DBIError::Data getDBIError(dbi_conn con) {
    if(dbi_conn_error_flag(con) == 0) {
      return DBIError::Data(0, "");
    }
    // obtain a global lock
    //   Lock lck(s_mutex, m_b_use_mutex);
    const char *errmsg_dest = 0;
    int rc = dbi_conn_error(con, &errmsg_dest);
    if (rc == -1) {
      return DBIError::Data(0, "");
    }
    std::string str = errmsg_dest;
    return DBIError::Data(rc, str);
  }
  int getDBIErrorNumber() const {
    return _data._dbierrno;
  }
private:
  DBIError::Data _data;
};


#endif // DBIERROR_H
