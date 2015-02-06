/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef PIDFILE_H
#define PIDFILE_H

#include <string>
#include <iostream>
#include <fstream>

class PidFile {
  std::string _name;
  bool _created;
public:
   PidFile(const std::string &name) : _name(name), _created(false) {
     std::ofstream pidfile(name);
     if (!pidfile) {
       std::cerr << "WARNING: failed to create the pidfile '" << name << "'"
         << std::endl;                                                                      
       return;
     }
     ///TODO this should probably lock but we leave all that to the initscript
     pidfile << getpid();
     _created = true;
   }
   ~PidFile() {
      if (_created) {
        _created = false;
        unlink(_name);
      }
   }
};

#endif // PIDFILE_H
