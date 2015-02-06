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

#ifndef WHITELISTCREATOR_H
#define WHITELISTCREATOR_H

#include <iostream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "tokens.h"
#include "core.h"

template <
  class CallbackType,
  class ArgumentProviderType,
  class MatchContainerType,
  class PolicyType
>
class WhitelistCreator
{
public:
  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg, 
			     boost::shared_ptr<Core> core, int sectionId, int typeId) {
    const std::string &name = cfg.getVal(Tokens::PT_name);
    Tokens::SectionsTokensType stk = Tokens::SectionsTokensType (sectionId);
    const std::string &type = Tokens::instance().configTypeTokens(stk)->getToken(typeId);
    MatchContainerType & mts = _callback.getMatchContainer();
    typename MatchContainerType::const_iterator mitr = _callback.checkForRef(cfg, 
      mts, Tokens::match, stk, Tokens::PT_match, type);
    boost::shared_ptr<Core::Matcher> matcher((*mitr).second);
    boost::shared_ptr<Core::Policy> policy(new PolicyType((*mitr) . first));
    core->push_back(matcher, policy); 
    tracer << "created policy " << type << " " <<  name  << std::endl;
    return (name.empty() ? "(whitelist)" : name);
  }
};

#endif // WHITELISTCREATOR_H
