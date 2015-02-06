/**   $Id: core.h 53 2009-10-11 21:34:25Z mimo $   **/
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

#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <vector>
#include <boost/smart_ptr.hpp>
#include "utils.h"
#include "defensive.h"
// #include "triplet.h"
// #include "greylist.h"
// #include <memory>
// #include "module.h"

class Core 
{
public:
  class Extra {
    std::string _comment, _prefix;
    unsigned long _diff, _count;
  public:
    Extra() : _comment(), _prefix(), _diff(0), _count(0) { }
    void setComment(const std::string& c) { _comment = c; }
    const std::string& getComment() const { return _comment; }
    void setPrefix(const std::string &p) { _prefix = p; }
    const std::string& getPrefix() const { return _prefix; }
    unsigned long getDiff() const { return _diff; }
    unsigned long getCount() const { return _count; }
    void set(const std::string & p, unsigned long c, unsigned long t) {
      _prefix = p;
      _count = c;
      _diff = t;
    }
  };
  class Matcher {
  public:
    virtual bool match(class Triplet &, Extra &) = 0;
    virtual ~Matcher() { }
  };
  typedef enum {
    dunno, /* stop processing and return dunno */
    defer, /* stop processing and return defer_if_permit */
    cont,   /* continue processing */
    error
  }ActionType;
  class Policy {
  public:
    virtual ActionType decide(class Triplet &, Extra &) = 0;
    virtual ~Policy() { }
  };
  typedef std::pair<boost::shared_ptr<Matcher>, boost::shared_ptr<Policy> >ModuleType;
  typedef std::vector<ModuleType> ContainerType;
  Core() : _chain() { }
  ~Core() { tracer << "core destroyed" << std::endl; }
  /** process a triplet and return its status **/
  ActionType process(class Triplet &, Extra &);
  /** **/
/*  void push_back(const ContainerType::value_type &r) {
    _chain.push_back(r);
  }*/
//   void push_back(Matcher *m, Policy *p) {
  void push_back(boost::shared_ptr<Matcher> m, boost::shared_ptr<Policy> p) {
    _chain.push_back(ContainerType::value_type(m, p));
  }
  size_t size() const { return _chain.size(); }
  //   void clear();
protected:
  ContainerType _chain;
  
};

#endif // CORE_H
