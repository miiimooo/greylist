/// $Id: tokenlist.h 33 2009-09-22 00:40:50Z mimo $
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

#ifndef TOKENLIST_H
#define TOKENLIST_H

#include <string>
#include <deque>
#include <map>
#include <stdexcept>

class TokenList
{
  typedef std::map<const std::string,unsigned > TokenMap;
  typedef std::deque<bool> RequiredVector;
  typedef std::map<unsigned,const std::string> DefaultMap;
  TokenMap _map;
  RequiredVector _required;
  DefaultMap _default;
public:
  typedef std::map<unsigned,bool> RequiredArray;
  
  TokenList() : _map(), _required(), _reqarr() { } 
  const std::string& getToken(unsigned i) const {
    for(TokenMap::const_iterator itr = _map.begin(); itr != _map.end(); ++itr) {
      if ((*itr).second == i) {
	return (*itr).first;
      }
    }
    throw std::out_of_range("TokenList getToken");
  }
  void addToken(const std::string& t, bool b) {
    unsigned ipos = _map.size();
    _map.insert(TokenMap::value_type(t, ipos));
    _required.push_back(b);
    if (b) {
      _reqarr.insert(RequiredArray::value_type(ipos, true));
    }
  }
  void addToken(const std::string& t, bool b, const std::string & def) {
    unsigned ipos = _map.size();
    _map.insert(TokenMap::value_type(t, ipos));
    _required.push_back(b);
    if (b) {
      _reqarr.insert(RequiredArray::value_type(ipos, true));
      _default.insert(DefaultMap::value_type(ipos, def));
    }
  }
  int findToken(const std::string& t) const {
    TokenMap::const_iterator itr = _map.find(t);
    if (itr == _map.end()) {
      return -1;
    }
    return (*itr).second;
  }
  void copyRequired(RequiredArray &target) const {
    target = _reqarr;
  }
  unsigned size() const {
    return _map.size();
  }
  
  int parse(const std::string& t, RequiredArray& container) const {
    int ipos = findToken(t);
    if (ipos == -1) {
      return -1;
    }
    RequiredArray::iterator itr = container.find(ipos);
    if (itr != container.end()) {
      container.erase(itr);
    }
    return ipos;
  }
  bool haveDefaults() const { return !_default.empty(); }
  const std::string & getDefault(unsigned i) const {
    DefaultMap::const_iterator itr = _default.find(i);
    return (itr == _default.end() ? _strNull : (*itr).second);
  }
private:
  RequiredArray _reqarr;
  const static std::string _strNull;
  // Singleton pointer.
//   static TokenList *instance_;
  
};

#endif // TOKENLIST_H
