/// $Id: regexcpp.h 62 2009-10-14 00:15:12Z mimo $
/***************************************************************************
 *   Copyright (C) 2004 by Michael Moritz                                  *
 *   mimo@restoel.net                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef __REGEX_H
#define __REGEX_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <exception>
#include <string>
#include <regex.h> /* Provides regular expression matching */
#include <string.h>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include "utils.h"

class RegEx {
  struct RegexDeleter {
    void operator()(regex_t * regexdata) {
      if (regexdata) {
	::regfree(regexdata);
	delete regexdata;
      }
    }
  };
  struct Storage : public boost::noncopyable {
    int _errno;
    regex_t _regex;
    Storage() : _errno(0) {
      memset(&_regex, 0, sizeof(regex_t));
    }
    ~Storage() {
      if (_errno == 0) {
	::regfree(&_regex);
      }
    }
  };
public:
  RegEx(const std::string & expr, int cflags = REG_ICASE | REG_NOSUB | REG_EXTENDED) 
  : /*m_regex(new regex_t, RegexDeleter())*/m_regex(new Storage), m_compflags(cflags){
//     ::memset(m_regex.get(), 0, sizeof(regex_t));
//     int err_no;
    if ((m_regex->_errno = regcomp(&m_regex->_regex, expr.c_str(), m_compflags)) != 0) { /* Compile the regex */
      boost::mutex::scoped_lock lock(*getMutex().get());
      size_t length;
      char *buffer;
      std::string strErr;
      length = regerror(m_regex->_errno, &m_regex->_regex, NULL, 0);
      buffer = new char[length];
      regerror(m_regex->_errno, &m_regex->_regex, buffer, length);
      strErr = buffer;
      delete [] buffer;
      throw std::runtime_error("Regex: " + strErr + "(" + Utils::StrmConvert(m_regex->_errno) 
	+ "), "	+ expr);
    }
  }
  virtual ~RegEx() { }
  bool match(const std::string &s) const {
    return (regexec(&m_regex->_regex, s.c_str(), 0, 0, 0) == 0); /* Found a match */
  }
private:
//   boost::shared_ptr<regex_t> m_regex;
  boost::shared_ptr<Storage> m_regex;
  int m_compflags;
  boost::shared_ptr<boost::mutex> getMutex() {
    static boost::shared_ptr<boost::mutex> s_pm;
    if (!s_pm) {
      s_pm.reset(new boost::mutex);
    }
    return s_pm;
  }
};

#endif
