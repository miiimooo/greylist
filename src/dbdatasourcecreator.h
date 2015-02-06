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

#ifndef DBDATASOURCECREATOR_H
#define DBDATASOURCECREATOR_H
#include <iostream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"

#include "tokens.h"
#include "dbdatasource.h"
#include "Database.h"

template <
  class CallbackType,
  class ArgumentProviderType,
  class DatabaseContainerType,
  class DataSourceContainerType,
  class DBDataSourceType
>
class DBDataSourceCreator
{
public:
  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg, 
			     boost::shared_ptr<Core>, int sectionId, int typeId) {
    const std::string & name = cfg.getVal(Tokens::DST_name);
    DatabaseContainerType & dbs = _callback.getDatabaseContainer(); 
    typename DatabaseContainerType::const_iterator itr = _callback.checkForRef(cfg, 
           dbs, Tokens::database, Tokens::SectionsTokensType(sectionId),
           Tokens::DST_database, name);
    _callback.checkForToken(cfg, Tokens::SectionsTokensType(sectionId), Tokens::DST_table, name);
    boost::shared_ptr<DataSource> ds(new DBDataSourceType((*itr).second, cfg.getVal(Tokens::DST_table)));
    DataSourceContainerType & dss = _callback.getDataSourceContainer();
    if (dss.find(name) != dss.end()) {
      throw std::runtime_error("datasource named '" + name + "' already defined");
    }
    dss.insert(typename DataSourceContainerType::value_type(name, ds));
    tracer << "created db ds " <<  name << " table " << cfg.getVal(Tokens::DST_table) 
      << " database " << cfg.getVal(Tokens::DST_database) << std::endl;
    return name;
  }
};

#endif // DBDATASOURCECREATOR_H
