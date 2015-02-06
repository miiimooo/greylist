/**   $Id: triplet.cpp 7 2009-08-23 03:17:04Z mimo $   **/
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
#include <netdb.h>
#include "./triplet.h"


/// shared function for initialisation of a triplet
// - set up _resolved (if reverse)
// - set up _client_address_numeric (always?)
// - set up _weakclient (if in weak mode)
void Triplet::_init() {
  bool weak = g_getCfg().isModeWeak();
  bool reverse = g_getCfg().isModeReverse();
  bool verbose = g_getCfg().isVerbose();

  /// first set up the numeric representation of the client address

  const std::string& tStr = _client_address;
  //	std::cout << "tStr" << tStr << std::endl;
  std::string::size_type i = tStr.find('.');
  std::string::size_type j = 1; //skip '
  if (tStr[0] != '\'') //unquoted
    j = 0;
  if (i != std::string::npos) {
    do {
      //std::cout << "substr " << tStr.substr(j,i-j) << std::endl;
      _client_address_numeric += atoi((tStr.substr(j, i - j)).c_str());
      j = i + 1;
      i = tStr.find('.', j);
      if (i == std::string::npos) {
        i = tStr.find('\'', j);
        if ((i == std::string::npos) && (tStr[0] != '\'')) { //unquoted
          //std::cout << "substr " << tStr.substr(j,i) << std::endl;
          _client_address_numeric += atoi((tStr.substr(j, i)).c_str());
          break;
        }
      }
    } while (i != std::string::npos);
  }

  /// next resolve the IP address

  if (reverse && _resolved == "") {
    const std::string& clientAddressNumeric = _client_address_numeric;
    //would like to use getnameinfo but documentation sucks big time
    hostent* hp = ::gethostbyaddr(clientAddressNumeric.c_str(),
                                  clientAddressNumeric.size(),
                                  AF_INET);
    if (hp) {
      _resolved = hp->h_name;
    } else {
      std::string err;
      switch (h_errno) {
        case HOST_NOT_FOUND:
          err = "HOST_NOT_FOUND";
          break;
        case NO_ADDRESS:
          err = "NO_ADDRESS|NO_DATA";
          break;
        case NO_RECOVERY:
          err = "NO_RECOVERY";
          break;
        case TRY_AGAIN:
          err = "TRY_AGAIN";
          break;
        default:
          err = "Unexpected";
          break;
      }
      _not_resolved = err;
    }
  }

  /// finally set up _weakclient which isn't always used

  _weakclient = "'" + getClientAddressNumericStr('.', 3) + "." + "'";

}
