/**   $Id: triplet.h 32 2009-09-19 14:30:08Z mimo $   **/
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

#ifndef TRIPLET_H
#define TRIPLET_H

#include <string>
#include <vector>
#include "tokens.h"
#include "utils.h"
#include "ipaddress.h"

  
class Triplet {
//   std::vector<std::string> _members;
  std::string _sender, _recipient, _tripletstring;
  IPAddress _client_address; 
public:
  typedef enum {  // don't forget about tokens.h
    client_address,
    sender,
    recipient,
    reverse_client_name,
    client_name,
    short_reverse_client_name,
    short_client_name,
    triplet_string,
    network 
  }MemberType;
  /// @params client_address, sender, recipient, reverse_client_name
  Triplet(const std::string& c, const std::string& s, const std::string& r,
	   const std::string& n, const std::string& cn) 
    : _sender(s), _recipient(r), _tripletstring(), _client_address(c, n, cn) { 
      Utils::lowercase(_sender);
      Utils::lowercase(_recipient);
    } 

  unsigned getClientAddressNumericIdx(unsigned idx) {
    return _client_address.getNumericIdx(idx);
  }
  ///return resolved host name or empty string if error
  /// set _not_resolved to the error message if it couldn't
  /// be resolved
  const std::string &getReverseClientName() {
    return _client_address.getReverseName();
  }
  const std::string &getClientName() {
    return _client_address.getClientName();
  }
  const std::string &getShortClientName() {
    return _client_address.getShortClientName();
  }
  const std::string &getShortReverseClientName() {
    return _client_address.getShortReverseName();
  }
  const std::string& getSender() const { 
    return _sender;
  }
  const std::string& getRecipient() const { 
    return _recipient; 
  }
  const std::string& getClientAddress() const {
    return _client_address.getAddress();
  }
  const std::string& getNetwork() {
    return _client_address.getNetwork();
  }
  const std::string& getMember(MemberType type) {
    switch(type) {
      case client_address:
	return getClientAddress();
      case sender:
	return getSender();
      case recipient:
	return getRecipient();
      case reverse_client_name:
	return getReverseClientName();
      case client_name:
	return getClientName();
      case short_reverse_client_name:
	return getShortReverseClientName();
      case short_client_name:
	return getShortClientName();
      case triplet_string:
	return getTripletString();
      case network:
	return getNetwork();
    }
    throw std::out_of_range("unknown triplet member index: " + Utils::StrmConvert(type));
  }
  const std::string& getTripletString() {
    if (!_tripletstring.empty()) {
      return _tripletstring;
    }
    _tripletstring = "s=" + getSender() + "\n"
      + "r=" + getRecipient() + "\n"
      + "c=" + getClientAddress() + "\n"
      + "h=" + getClientName() + "\n";
    return _tripletstring;
  }
};

#endif // TRIPLET_H
