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

#ifndef GREYLISTCREATOR_H
#define GREYLISTCREATOR_H

#include <iostream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "tokens.h"
#include "core.h"
#include "ipaddress.h"
#include "greylist.h"
#include "systemerror.h"

template <
  class CallbackType,
  class ArgumentProviderType,
  class DataSourceContainerType,
  class MatchContainerType,
  class PolicyType
>

class GreylistCreator
{
public:
  
  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg, 
			     boost::shared_ptr<Core> core, int sectionId, int typeId) {
    const std::string &name = cfg.getVal(Tokens::PT_name);
    Tokens::SectionsTokensType stk = Tokens::SectionsTokensType (sectionId);
    const std::string &type = Tokens::instance().configTypeTokens(stk)->getToken(typeId);
    DataSourceContainerType & dss = _callback.getDataSourceContainer();
    typename DataSourceContainerType::const_iterator dsitr = _callback.checkForRef(cfg, 
      dss, Tokens::policy, stk, Tokens::PT_datasource, type);
    const std::string &strTimeout = cfg.getVal(Tokens::PT_timeout);
    int timeout = Utils::StrmConvert(strTimeout);
    if (timeout < 0) { // by allowing 0 we can handle mode=init or maybe not
      throw BadValue("bad value for 'timeout' " 
	+ ( strTimeout.empty() ? "" : "'" + strTimeout + "'"));
    }
    const std::string &strWeakbytes = cfg.getVal(Tokens::PT_weakbytes);
    int weakbytes = (strWeakbytes.empty() ? 3 : Utils::StrmConvert(strWeakbytes));
    if ((weakbytes < 0) || (weakbytes > IPAddress::default_length)) {
      throw BadValue("bad value for 'weakbytes' " 
      + ( strTimeout.empty() ? "" : "'" + strWeakbytes + "'"));
    }
    const std::string &strMode = cfg.getVal(Tokens::PT_mode);
    int iMode = (strMode.empty() ? Tokens::GT_reverse
      : Tokens::instance().configGreylistTokens()->findToken(strMode) );
    if (iMode == -1) {
      throw BadValue("unknown greylist mode '" + strMode +"'");
    }
    Tokens::GreylistTokensType mode = Tokens::GreylistTokensType(iMode);
    Greylist::ModeType glmode = Greylist::ModeType(iMode);
    bool verbose = true;
    MatchContainerType & mts = _callback.getMatchContainer();
    typename MatchContainerType::const_iterator mitr = _callback.checkForRef(cfg, 
      mts, Tokens::match, stk, Tokens::PT_match, type);
    boost::shared_ptr<Core::Matcher> matcher((*mitr).second);
    boost::shared_ptr<Core::Policy> policy(
      new PolicyType((*dsitr).second, glmode, verbose, timeout, weakbytes));
    core->push_back(matcher, policy); 
    tracer << "created policy " << type << " " <<  name  << std::endl;
    return (name.empty() ? "(greylist)" : name);
  }
};

#endif // GREYLISTCREATOR_H
