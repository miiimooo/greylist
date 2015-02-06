/*
#include "config.h"
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

#ifndef CONFIGFILEREADER_H
#define CONFIGFILEREADER_H

#include <fstream>
#include <errno.h>
#include <string.h>
#include <stdexcept>
#include "utils.h"

template <class StreamClass>
class ConfigFileReader
{
  StreamClass _strm;
  const std::string _filename;
public:
  static std::string errmsg(int error) {
    std::string errstr;
    errstr.reserve(512);
    ::strerror_r(error, &errstr[0], errstr.size());
    return errstr;
  }
  ConfigFileReader(const std::string &filename ) 
    : _filename(filename), _strm(filename.c_str()) {
    if (!_strm) {
      int error = errno;
      throw std::runtime_error("Error opening '" + filename + "'"
	+ errmsg(error) + " (" + Utils::StrmConvert(error) + ")");
    }
  }
  int getline(std::string &str) {
    if (_strm.eof()) {
      return -1;
    }
    bool bBegin = true;
    bool bComment = false;
    unsigned cnt = 0;
    char c = 0;
    str = "";
    while (!_strm.eof()) {
      _strm.get(c);
      if (bComment) {
	if (c == '\n') {
	  rtrim(str, cnt);
	  if (cnt > 0) {
	    return cnt;
	  }
	  bComment = false;
	  continue;
	}
	continue;
      }
      if (bBegin) {
	if (c == '=') {
	  bBegin = false;
	} else {
	  c = std::tolower(c);
	}
	if (c == ' ') {
	  continue;
	}
      }
      if (c == '#') {
	bComment = true;
	continue;
      }
      if (c == '\n') {
	if (cnt > 0) {
	  break;
	}
	continue;
      }
      str += c;
      cnt++;
    }
    rtrim(str, cnt);
    if (_strm.eof() && (cnt > 0)) {
      _strm.close();
      return cnt;
    }
    if (_strm.eof()) {
      _strm.close();
      return -1;
    }
    return cnt;
  }
  bool isEof() const { return _strm.eof(); }
  void reload() { 
    _strm.close();
    _strm.open(_filename.c_str());
    if (!_strm) {
      int error = errno;
      throw std::runtime_error("Error opening '" + _filename + "'"
      + errmsg(error) + " (" + Utils::StrmConvert(error) + ")");
    }
  }
private:
  void rtrim(std::string &line, unsigned & cnt, std::string tobedel=" ") {
    std::string::iterator itr = line.end();
    while( !line.empty() && (tobedel.find(*(--itr)) != std::string::npos) ) {
      line.erase(itr);
      --cnt;
    }
  }
};

#endif // CONFIGFILEREADER_H
