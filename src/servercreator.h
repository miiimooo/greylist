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

#ifndef SERVERCREATOR_H
#define SERVERCREATOR_H

#include <iostream>
#include <boost/smart_ptr.hpp>
#include "tokens.h"
#include "serversettings.h"

template <
  class CallbackType,
  class ArgumentProviderType
>
class ServerCreator /** really just a copier **/
{
public:
  static std::string create(CallbackType & _callback, ArgumentProviderType & cfg,
			     ServerSettings &settings, int sectionId) {
    Tokens::SectionsTokensType stk = Tokens::SectionsTokensType (sectionId);
    cfg.copyVals(settings);
    return "(server)";
  }
};

#endif // SERVERCREATOR_H
