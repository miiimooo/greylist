/// $Id: tmplcore.h 33 2009-09-22 00:40:50Z mimo $
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

#ifndef TMPLCORE_H
#define TMPLCORE_H

template <class BackendType>
class TmplMatcher : public Core::Matcher {
  BackendType _backend;
public:
  TmplMatcher() : _backend() { }
  TmplMatcher(boost::shared_ptr<class DataSource> ds, const std::string& column, Triplet::MemberType m, /**/
	       const std::string& returnfield)  
	       : _backend(ds, column, m, returnfield) { }
  virtual bool match(class Triplet& t, Core::Extra &e) {
    return _backend.match(t, e);
  }
  virtual ~TmplMatcher() { }
  static Core::Matcher * create(boost::shared_ptr<class DataSource> ds, 
				 const std::string& column, Triplet::MemberType m,
				 const std::string& returnfield) {
    return new TmplMatcher<BackendType>(ds, column, m, returnfield);
  }
};
class Greylist;
template <class BackendType>
class TmplPolicy : public Core::Policy {
  BackendType _backend;
public:
  TmplPolicy() : _backend() { }
  TmplPolicy(const std::string & name) : _backend(name) { }
  TmplPolicy(boost::shared_ptr<class DataSource> ds, Greylist::ModeType m, bool v, unsigned t, unsigned w) /**/
  : _backend(ds, m, v, t, w) { }
  virtual Core::ActionType decide(class Triplet &t, Core::Extra &e) {
    return _backend.decide(t, e);
  }
  virtual ~TmplPolicy() { }
};

#endif // TMPLCORE_H
