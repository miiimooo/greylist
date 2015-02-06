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

#ifndef DATABASECREATOR_H
#define DATABASECREATOR_H

#include <iostream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "tokens.h"
#include "Database.h"
#include "systemerror.h"


template <
  class CallbackType,
  class ArgumentProviderType,
  class DatabaseContainerType,
  class DatabaseMutexType
>
class DatabaseCreator
{
public:
  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg,
			     class ServerSettings&, int) {
    DatabaseContainerType & dbs = _callback.getDatabaseContainer(); 
    Database::dbparams_v dbparams (cfg.getVars());
    dbparams.insert(Database::dbparams_v::value_type(
      Tokens::instance().configTokens(Tokens::database)->getToken(Tokens::DBT_type),
      cfg.getVal(Tokens::DBT_type)));
    const std::string &strSpare = cfg.getVal(Tokens::DBT_spareconnections);
    int spare = Utils::StrmConvert(strSpare);
    if (spare < 0) { // by allowing 0 we can handle mode=init or maybe not
      throw BadValue("bad value for 'spare' " 
	+ ( strSpare.empty() ? "" : "'" + strSpare + "'"));
    }
    DatabaseMutexType & dbMutex = _callback.getDatabaseMutex();
    boost::shared_ptr<Database> db(new Database(dbMutex, dbparams, spare));
    const std::string & name = cfg.getVal(Tokens::DBT_name);
    if (dbs.find(name) != dbs.end()) {
      throw std::runtime_error("database named '" + name + "' already defined");
    }
    dbs.insert(typename DatabaseContainerType::value_type(name, db));
    tracer << "created db " <<  name << " industrialised" << std::endl;
    return name;
  }
};

#endif // DATABASECREATOR_H
