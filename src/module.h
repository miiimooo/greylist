/**   $Id: module.h 7 2009-08-23 03:17:04Z mimo $   **/
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

#ifndef MODULE_H
#define MODULE_H

class Module {
  public:
    typedef enum {
      dunno,
      defer,
      match,
      differ,
      pass
    }ActionType;
    
    Module() { };
    virtual ~Module() { };
    /** @return true  if matched (=whitelisted triplet)
    @return false not matched
    **/
    virtual ActionType process(class Triplet&) = 0;
    //   virtual bool check(const class Triplet& ) = 0;
};

template <class Backend>
class TmplModule : public Module {
  public:
//     TmplWlMod(class DB& db,
//               bool initmode,
//               const WLStaticDef& staticDef) throw(std::exception)
//         : _backend(db, initmode, staticDef) { }
    virtual ~TmplModule() {}
    virtual ActionType process(class Triplet& t) {
      return _backend.process(t);
    }
//     virtual bool check(const class Triplet& triplet) {
//       return _backend.check(triplet);
//     }
  private:
    Backend _backend;
};

#endif // MODULE_H
