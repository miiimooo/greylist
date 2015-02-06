/**   $Id: action.h 64 2010-08-12 21:58:53Z mimo $   **/
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

#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <syslog.h>
#include "triplet.h"
#include "core.h"
#include "utils.h"
#include "tokens.h"

class Action
{
  bool _deferwithstatus;
  Tokens::LogTokensType _logLevel;
//   const std::string _dunno, _defer, _onerror;
  typedef std::map<Core::ActionType, const std::string> ActionMap;
  ActionMap _map;
  typedef std::map<Core::ActionType, bool> DeferMap;
  DeferMap _deferMap;
public:
//   Action(const std::string &defer = "Service is unavailable") 
//     : _defer("action=defer_if_permit " + defer + "\n\n"), 
//     _dunno("action=dunno\n\n") { }
  Action(bool deferwithstatus, 
	  const std::string & defer, const std::string & dunno, 
	  const std::string & onerror, unsigned loglevel)
     : _deferwithstatus(deferwithstatus), /*_dunno(dunno), 
       _defer(defer), _onerror(onerror),*/ 
       _logLevel(Tokens::LogTokensType(loglevel)) {
    _map.insert(ActionMap::value_type(Core::dunno, dunno));
    _map.insert(ActionMap::value_type(Core::defer, defer));
    _map.insert(ActionMap::value_type(Core::error, onerror));
    const std::string & deferprefix = Tokens::instance().postfixResponseTokens()->getToken(Tokens::PFT_defer);
    for(ActionMap::const_iterator itr = _map.begin(); itr != _map.end(); ++itr) {
//       bool isDefer = ((*itr).second.substr(0, deferprefix.length()-1) == deferprefix);
      int cmp = (*itr).second.compare(0, deferprefix.length(), deferprefix);
/*      std::cout << (*itr).second.substr(0, deferprefix.length()) << " = " 
	<< cmp << " == " << deferprefix <<  std::endl;*/
      _deferMap.insert(DeferMap::value_type((*itr).first, cmp == 0));
    }
  }
  std::string process(const Core::ActionType action, Triplet &triplet,
		       const Core::Extra & extra) const {
    ActionMap::const_iterator itr = _map.find(action);
    if (itr == _map.end()) {
      throw std::logic_error("Unexpected Action idx=" + Utils::StrmConvert(action));
    }
    std::string str = "action=";
    str += (*itr).second;
    if (_deferwithstatus && ((*_deferMap.find(action)).second == true)) {
      std::stringstream stm;
      stm << extra.getPrefix() << ": (" << extra.getCount() << ", " 
	<< extra.getDiff() << " secs)";
      str += " " + stm.str(); 
    }
    str += "\n\n";
    if (_logLevel == Tokens::LT_full) {
      const std::string & client = (triplet.getClientName() == "unknown" ? 
	triplet.getClientAddress() : triplet.getClientName());
      std::string comment = (extra.getComment().empty() ? "" : ": " + extra.getComment());
      if (extra.getDiff() == 0) { //either wl or new
	::syslog(LOG_INFO,"%s: %s -> %s, %s (%lu, %lu secs)%s", extra.getPrefix().c_str(),
		  triplet.getSender().c_str(), triplet.getRecipient().c_str(),
		  client.c_str(), extra.getCount(), extra.getDiff(), 
		  comment.c_str());
		  
      } else {
	::syslog(LOG_INFO,"%s: %s -> %s, %s (%lu, %lu secs)%s", extra.getPrefix().c_str(),
		triplet.getSender().c_str(), triplet.getRecipient().c_str(),
		client.c_str(), extra.getCount(), extra.getDiff(), comment.c_str());
      }
    }
    return str;
  }
};

#endif // ACTION_H
/* vim: set expandtab tabstop=4 shiftwidth=2: */
