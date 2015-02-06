/// $Id: patternmatch.h 27 2009-09-15 19:37:26Z mimo $
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

#ifndef PATTERNMATCH_H
#define PATTERNMATCH_H

#include <iostream>
#include <memory>
#include <boost/smart_ptr.hpp>
#include "triplet.h"
#include "core.h"
#include "datasource.h"
#include "regexcpp.h"


class PatternMatch
{
  boost::shared_ptr<DataSource> _ds;
  const std::string _column;
  Triplet::MemberType _member;
  const std::string _returnfield;
//   RegEx _regex;//REG_ICASE (REG_EXTENDED|REG_ICASE)
  public:
    PatternMatch(boost::shared_ptr<DataSource> ds, const std::string& column, Triplet::MemberType m,
		const std::string& returnfield) 
		: _ds(ds), _column(column), _member(m), _returnfield(returnfield) { }
    virtual bool match(Triplet& triplet, Core::Extra& extra) {
      std::auto_ptr<DataSource::BaseQuery> query(_ds->createQuery());
      query->addReturnField(_column);
      query->addReturnField(_returnfield);
//       query->addWhere(_column, triplet.getMember(_member), "=");
      query->lookup();
      unsigned numrows = query->numRows();
      if (numrows == 0) {
	return false;
      }
      for(unsigned i=0;i < numrows;i++) {
	if (!query->fetchRow()) {
	  break;
	}
	const std::string pattern = query->getString(1);
	
	RegEx regex(pattern);
	if(regex.match(triplet.getMember(_member))) {
// 	  std::cout << "MATCH   " << triplet.getMember(_member) << std::endl;
// 	  const std::string comment = query->getString(2);
// 	  std::cout << "comment: " << comment << std::endl;
	  extra.setComment(query->getString(2));
	  return true;
	}
      }
      return false;	
    }
    virtual ~PatternMatch() { }
		
};

#endif // PATTERNMATCH_H
