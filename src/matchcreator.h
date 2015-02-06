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

#ifndef MATCHCREATOR_H
#define MATCHCREATOR_H
#include <iostream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "tokens.h"
#include "core.h"

template <
class CallbackType,
class ArgumentProviderType,
class DataSourceContainerType,
class MatchContainerType,
class MatcherType
>
class MatchCreator
{
public:

  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg, 
			     boost::shared_ptr<Core>, int sectionId, int typeId) {
    const std::string &name = cfg.getVal(Tokens::MT_name);
    const std::string &lookup = cfg.getVal(Tokens::MT_lookup);
    const std::string &match = cfg.getVal(Tokens::MT_match);
    const std::string &ret = cfg.getVal(Tokens::MT_return);
    Tokens::SectionsTokensType stk = Tokens::SectionsTokensType (sectionId);
    DataSourceContainerType & dss = _callback.getDataSourceContainer();
    typename DataSourceContainerType::const_iterator itr = _callback.checkForRef(cfg, 
      dss, Tokens::datasource, stk, Tokens::MT_datasource, name);
    _callback.checkForToken(cfg, stk, Tokens::MT_lookup, name);
    _callback.checkForToken(cfg, stk, Tokens::MT_match, name);
    // hardcoded map tokens to triplet members
    unsigned m = Tokens::instance().configColumnTokens()->findToken(match);
    boost::shared_ptr<Core::Matcher> spmatcher(
      new MatcherType((*itr).second, lookup, Triplet::MemberType(m), ret));
    MatchContainerType & mts = _callback.getMatchContainer();
    if (mts.find(name) != mts.end()) {
      throw std::runtime_error("match named '" + name + "' already defined");
    }
    mts.insert(typename MatchContainerType::value_type(name, spmatcher));
    tracer << "created "
      << Tokens::instance().configTypeTokens(stk)->getToken(typeId) << " " 
      <<  name << " column " << lookup
      << " triplet member " << match 
      << " return " << ret << std::endl;
    return name;
  }
//   static std::string createAllMatcher(CallbackType & _callback, ArgumentProviderType & cfg, 
// 			     int sectionId, int typeId) {
    
};
template <
  class CallbackType,
  class ArgumentProviderType,
  class DataSourceContainerType,
  class MatchContainerType
>
class MatchCreator<CallbackType, ArgumentProviderType, DataSourceContainerType, 
  MatchContainerType, AllMatchMatcher>
{
public:

  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg, 
			     boost::shared_ptr<Core>, int sectionId, int typeId) {
    const std::string &name = cfg.getVal(Tokens::MT_name);
    Tokens::SectionsTokensType stk = Tokens::SectionsTokensType (sectionId);
    const std::string &type = Tokens::instance().configTypeTokens(stk)->getToken(typeId);
    boost::shared_ptr<Core::Matcher> spmatcher( new AllMatchMatcher );
    MatchContainerType & mts = _callback.getMatchContainer();
    if (mts.find(name) != mts.end()) {
      throw std::runtime_error("match named '" + name + "' already defined");
    }
    mts.insert(typename MatchContainerType::value_type(name, spmatcher));
    tracer << "created match" << type << " " <<  name  << std::endl;
    return name;
  }
//   static std::string createAllMatcher(CallbackType & _callback, ArgumentProviderType & cfg, 
// 			     int sectionId, int typeId) {
    
};

#endif // MATCHCREATOR_H
