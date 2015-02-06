/// $Id: parser.h 39 2009-09-22 23:25:30Z mimo $
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

#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include <iostream>
#include "tokenlist.h"

template <class ReaderType, class WriterType>
class Parser
{
  const TokenList *_tokens;
public:
  Parser(const TokenList* t) : _tokens(t) { }
  void parse(ReaderType &reader, WriterType &writer, bool variablemode=false) {
    TokenList::RequiredArray required;
    std::string line;
    std::string::size_type read;
    _tokens->copyRequired(required);
    while((read = reader.getline(line)) != std::string::npos) {
      if (read == -1) {
	break;
      }
//       std::cout << line << std::endl;
      std::string::size_type eqpos = line.find('=');
      if(eqpos == std::string::npos) {
	continue;
      }
      const std::string key = line.substr(0, eqpos);
      int tokenIdx = _tokens->parse(key, required);
      if (tokenIdx == -1) {
// 	std::cout << line << std::endl;
	if (variablemode) {
	  const std::string val = line.substr(eqpos+1, std::string::npos);
	  writer.add(key, val);
	}
	continue;
      }
      const std::string val = line.substr(eqpos+1, std::string::npos);
      writer.add(tokenIdx, val);
    }
    if (required.empty()) {
      return;
    }
    if (_tokens->haveDefaults()) {
      while(!required.empty()) {
	TokenList::RequiredArray::iterator itr = required.begin();
	const std::string & val = _tokens->getDefault((*itr).first);
// 	std::cout << (*itr).first << " " << val << std::endl;
	if (val.empty()) {
	  break;
	}
	writer.add((*itr).first, val);
	required.erase(itr);
      }
      if (required.empty()) {
	return;
      }
    }
    if (reader.isEof()) { //hmmm
      return;
    }
    std::string missing("Parser missing:");
    for(TokenList::RequiredArray::const_iterator itr=required.begin(); 
      itr != required.end(); ++itr) {
      missing += " " + _tokens->getToken((*itr).first);
    }
    throw std::runtime_error(missing);
  }
};

#endif // PARSER_H
