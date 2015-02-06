/***************************************************************************
 *   Copyright (C) 2004 by Michael Moritz                                  *
 *   mimo@restoel.net                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef __PMATCHER_H 
#define __PMATCHER_H

#include <string>
//getnameinfo
#include <netdb.h>
//
#include "regexcpp.h"
#include "triplet.h"

class PatternMatcher
{
public:
	PatternMatcher(const Triplet& triplet) 
	: 	_regex(),_bHaveHostname(false),
		_triplet(triplet)
	{
		_tripletkey = "s=" + triplet.getSender().substr(1,triplet.getSender().size()-2)+"\n";
		_tripletkey += "r=" + triplet.getRecipient().substr(1, triplet.getRecipient().size()-2)+"\n";
		_tripletkey += "c=" + triplet.getClientAddress().substr(1, triplet.getClientAddress().size()-2)+"\n";
	}
	~PatternMatcher() {}
	bool match(const std::string& pattern) throw (std::exception)
	{
		//check if the pattern contains the host (h=) token
		if( !_bHaveHostname && (pattern.find("h=") != std::string::npos) ) {
			_bHaveHostname = true;
			_tripletkey += "h=" + _triplet.getReverseClientName() + "\n";
		}
		//remove leading and trailing quote added in constructor...
		//std::cout << "pattern: " << pattern << std::endl;
		_regex.setPattern(pattern);
		if(_regex.match(_tripletkey)) {
			//std::cout << "MATCH   " << _tripletkey << std::endl;
			return true;
		}
		//std::cout << "nomatch " << _tripletkey << std::endl;
		return false;
	}
	void reset() { _bHaveHostname = false; }
private:
	RegEx _regex;//REG_ICASE (REG_EXTENDED|REG_ICASE)
	bool _bHaveHostname;
	const Triplet& _triplet;
	std::string _tripletkey;
};
#endif
