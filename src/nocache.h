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

#ifndef NOCACHE_H
#define NOCACHE_H

#include "datasource.h"
#include "utils.h"

class NoCache
{
  DataSource *_ds;
  DataSource::BaseQuery *_query;
  std::string _column;
public:
  NoCache(DataSource *ds) : _ds(ds), _query(0) { };
  void prepareQuery(const std::string &returnfield, const std::string &column) {
    Utils::zap(_query);
    _query = _ds->createQuery();
    if (!returnfield.empty()) {
      query->addReturnField(returnfield);
    }
    _column = column;
  }
  void executeQuery(std::string& arg) {
    if (!arg.empty()) {
      query->addWhere(_column, arg, "=");
    }
    query->lookup();
    if (query->numRows() == 0) {
      return false;
    }
    if (!_returnfield.empty()) {
      query->fetchRow();
      const std::string returned = query->getString(1);
      std::cout << returned << std::endl;
      /// TODO put it into extra
    }
    return true;
		     
};

#endif // NOCACHE_H
