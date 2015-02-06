/**
 **	IError.h
 **
 **	Published / author: 2004-06-11 / grymse@alhem.net
 **/

/*
Copyright (C) 2004  Anders Hedstrom

This program is made available under the terms of the GNU GPL.

If you would like to use this program in a closed-source application,
a separate license agreement is available. For information about 
the closed-source license agreement for this program, please
visit http://www.alhem.net/sqlwrapped/license.html and/or
email license@alhem.net.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _IERROR_H
#define _IERROR_H

#include <string>

#ifdef MYSQLW_NAMESPACE
namespace MYSQLW_NAMESPACE {
#endif


class Database;
class Query;
// class Database::Mutex;
/** Log class interface. */
class IError
{
public:
//   IError(Database::Mutex& m)/* : m_mutex(m_mutex), m_b_use_mutex(false) {}*/;
//   IError()/* : m_mutex(m_mutex), m_b_use_mutex(false) */{}
	virtual void error(Database&,const std::string&) = 0;
	virtual void error(Database&,Query&,const std::string&) = 0;
private:
//   Database::Mutex &m_mutex;
//   bool m_b_use_mutex;

};


#ifdef MYSQLW_NAMESPACE
} // namespace MYSQLW_NAMESPACE {
#endif

#endif // _IERROR_H
