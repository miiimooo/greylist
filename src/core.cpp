/**   $Id: core.cpp 41 2009-09-24 23:23:32Z mimo $   **/
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
#include <iostream>
#include <vector>
#include "defensive.h"
#include "triplet.h"
#include "core.h"
#include "allmatcher.h"
#include "datasource.h"
#include "utils.h"

using namespace std;

Core::ActionType Core::process(Triplet &triplet, Extra &extra) {
  for(ContainerType::const_iterator itr = _chain.begin(); itr != _chain.end(); ++itr) {
    if (! (*itr).first->match(triplet, extra)) { /// TODO error handling?
      // possibly check for lightweight errors??
      continue;
    }
    ActionType action = (*itr).second->decide(triplet, extra);
    if (action != Core::cont) {
      return action;
    }
  }
  return Core::dunno;
}

