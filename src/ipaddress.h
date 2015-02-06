/// $Id: ipaddress.h 32 2009-09-19 14:30:08Z mimo $
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

#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <arpa/inet.h>
#include "utils.h"

class IPAddress
{
  typedef std::vector<unsigned> Numeric;
  std::string _str;
  Numeric _numeric;
  std::string _reverse, _clientname, _resolve_error, _short_reverse, _network,
    _short_clientname;
public:
  /*typedef */enum { default_length = 4 };
  /// @params client_address, reverse_client_name
  IPAddress(const std::string& c, const std::string& n, const std::string& cn) : 
    _str(c), _numeric(), _reverse(n), _clientname(cn), _resolve_error(), 
    _short_reverse(), _network() { 
      Utils::lowercase(_reverse);
      Utils::lowercase(_clientname);
    }
//   IPAddress(const std::string& s) : _str(s), _numeric(), _reverse(), _resolve_error() { }
  unsigned getNumericIdx(unsigned idx) {
    if (_numeric.empty()) {
      initNumeric();
    }
    return _numeric[idx];
  }
  const std::string& getAddress() const { 
    return _str; 
  }
  const std::string& getClientName() const { 
    return _clientname; 
  }
  const std::string& getShortClientName() {
    if (!_short_clientname.empty()) {
      return _short_clientname; 
    }
    if (_clientname.empty()) {
      return _clientname;
    }
    _short_clientname = getShortenedName(_clientname);
    return _short_clientname;
  }
  const std::string &getReverseName() {
    if (!_resolve_error.empty()) {
      return _reverse;
    }
    if (!_reverse.empty()) {
      return _reverse;
    }
    resolve();
    return _reverse;
  }
  const std::string &getResolveError() const {
    return _resolve_error;
  }
  const std::string &getShortReverseName() {
    if (!_short_reverse.empty()) {
      return _short_reverse;
    }
    if (!_resolve_error.empty()) {
      return _short_reverse;
    }
    if (_reverse.empty()) {
      resolve();
    }
    if (_reverse.empty()) {
      return _short_reverse;
    }
    _short_reverse = getShortenedName(_reverse);
    return _short_reverse;
  }
  const std::string &getNetwork() {
    if (!_network.empty()) {
      return _network;
    }
    std::string::size_type dotpos = _str.rfind('.');
    _network = _str.substr(0, (dotpos == std::string::npos ? dotpos : dotpos+1));
    return _network;
  }
protected:
  void resolve();
  void initNumeric() {
    std::string::size_type startpos = 0, dotpos;
    bool endFlag = false;
    while(true) {
      dotpos = _str.find('.', startpos);
      const std::string strVal = _str.substr(startpos, dotpos);
      std::stringstream stm;
      unsigned val;
      stm << strVal;
      stm >> val;
      _numeric.push_back(val);
      if (dotpos == std::string::npos) {
	break;
      }
      startpos = dotpos+1;
    }
    if (_numeric.size() != default_length) {
      throw std::runtime_error("Length of parsed IP address: " + _str);
    }
  }
  std::string getShortenedName(const std::string &name) {
    int parts = 0;
    std::string::size_type len = name.length();
    std::string::size_type pos = len; // don't count any dot at end
    while (pos>0 && parts<3) { 
      // right-to-left until we have at least 2 domain parts
      pos--;
      if (pos>0 && name[pos-1] == '.') {
	parts++;
	if (len-pos >= 7) {
	  parts++; // accept com.com
	}
      }
    }
    if(parts == 0) { // no dot in rDNS, ignore it and fall back (to weak?)
      return "";
    }
    std::string shortened = name.substr(pos);
    return shortened;
  }
private:
};

#endif // IPADDRESS_H
