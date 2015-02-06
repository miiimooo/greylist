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

#ifndef SYSTEMERROR_H
#define SYSTEMERROR_H

#include <stdexcept>

struct SystemError : public std::runtime_error {
  SystemError(const std::string &s) : std::runtime_error(s) { }
};

struct UnknownReference : public SystemError {
  UnknownReference(const std::string &s) : SystemError(s) { }
};
struct MissingValue: public SystemError {
  MissingValue(const std::string &s) : SystemError(s) { }
};
struct BadType: public SystemError {
  BadType(const std::string &s) : SystemError(s) { }
};
struct BadValue: public SystemError {
  BadValue(const std::string &s) : SystemError(s) { }
};
struct UnknownConfig: public SystemError {
  UnknownConfig(const std::string &s) : SystemError(s) { }
};

#endif // SYSTEMERROR_H
