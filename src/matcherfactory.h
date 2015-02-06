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

#ifndef MATCHERFACTORY_H
#define MATCHERFACTORY_H

#include "tokens.h"
#include "core.h"

class MatcherFactory
{
public:
  typedef Core::Matcher* (*CreateMatcherCallback)();
private:
  typedef std::map<int, CreateMatcherCallback> CallbackMap;
public:
  // Returns 'true' if registration was successful
  bool add(int matcherId, CreateMatcherCallback CreateFn);
  // Returns 'true' if the ShapeId was registered before
  bool remove(int matcherId);
  Core::Matcher* Create(int matcherId);
private:
  CallbackMap callbacks_;
};

#endif // MATCHERFACTORY_H
