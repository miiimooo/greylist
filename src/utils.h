/**   $Id: utils.h 41 2009-09-24 23:23:32Z mimo $   **/
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

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <algorithm>
#include <string.h>

class Utils
{
public:
  template<typename T>
  static void zap(T* ptr) {
    if (ptr) {
      delete ptr;
      ptr = 0;
    }
  }
  // helper templates for converting numerics to string to pass to Query::Arguments
  template<typename T, typename F > 
  static T StrmConvert ( const F from )
  {
    std::stringstream temp;
    temp << from;
    T to = T();
    temp >> to;
    return to;
  }
  template<typename F> 
  static std::string StrmConvert ( const F from ) {
    return StrmConvert<std::string> ( from );
  }
  static int StrmConvert ( const std::string from ) {
    return StrmConvert<int> ( from );
  }
  struct ToLower
  {
    char operator() (char c) const  { return std::tolower(c); }
  };
  static void lowercase(std::string& str) {  
    std::transform(str.begin(),str.end(), str.begin(), ToLower());
  }
  static std::string getErrmsg(int error) {
    std::string errstr;
    errstr.reserve(512);
    ::strerror_r(error, &errstr[0], errstr.size());
    return errstr;
  }
};

#endif // UTILS_H
