/// $Id: tokens.h 51 2009-10-10 23:19:13Z mimo $
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

#ifndef TOKENS_H
#define TOKENS_H

#include <vector>
#include <iostream>
#include "singleton.h"
#include "tokenlist.h"

class Tokens : public Templates::Singleton<Tokens>

{
  friend class Templates::Singleton<Tokens>;
  
  TokenList _postfixTokens, _configSectionsTokens, _configModeTokens,
    _configColumnsTokens, _configBooleanTokens, _postfixResponseTokens,
    _logTokens;
  Tokens() {
    try { 
      /** postfix tokens - name, required **/
      _postfixTokens.addToken("client_address", true);
      _postfixTokens.addToken("sender", true);
      _postfixTokens.addToken("recipient", true);
      _postfixTokens.addToken("reverse_client_name", false);
      _postfixTokens.addToken("client_name", false); //verified client name
      
      _configColumnsTokens.addToken("client_address", false);
      _configColumnsTokens.addToken("sender", false);
      _configColumnsTokens.addToken("recipient", false);
      _configColumnsTokens.addToken("reverse_client_name", false);
      _configColumnsTokens.addToken("client_name", false);
      _configColumnsTokens.addToken("short_reverse_client_name", false);
      _configColumnsTokens.addToken("short_client_name", false);
      _configColumnsTokens.addToken("triplet_string", false);
      _configColumnsTokens.addToken("network", false);
      
      _configSectionsTokens.addToken("database", false);
      _configSectionsTokens.addToken("datasource", false);
      _configSectionsTokens.addToken("match", false);
      _configSectionsTokens.addToken("policy", false);
      _configSectionsTokens.addToken("server", false);
      
      _configModeTokens.addToken("reverse", false);
      _configModeTokens.addToken("weak", false);
      _configModeTokens.addToken("normal", false);

      TokenList database;
      database.addToken("name", true);
      database.addToken("type", true);
      database.addToken("spareconnections", true, "10");
      _configTokens.push_back(database);
      TokenList empty;
      _configTypeTokens.push_back(empty);
      
      TokenList datasource;
      datasource.addToken("name", true);
      datasource.addToken("type", true);
      datasource.addToken("table", false); /** only required for database **/
      datasource.addToken("database", false);
      _configTokens.push_back(datasource);
      TokenList dstypes;
      dstypes.addToken("db", false);
      _configTypeTokens.push_back(dstypes);
      
      TokenList match;
      match.addToken("name", true);
      match.addToken("type", true);
      match.addToken("datasource", false);
      match.addToken("lookup", false);
      match.addToken("match", false);
      match.addToken("return", false);
      _configTokens.push_back(match);
      TokenList matchtypes;
      matchtypes.addToken("exact match", false);
      matchtypes.addToken("pattern match", false);
      matchtypes.addToken("cached exact match", false);
      matchtypes.addToken("cached pattern match", false);
      matchtypes.addToken("all", false);
      _configTypeTokens.push_back(matchtypes);
      
      TokenList policy;
      policy.addToken("name", false);
      policy.addToken("type", true);
      policy.addToken("match", true);
      policy.addToken("datasource", false);
      policy.addToken("mode", true, "reverse");
      policy.addToken("weakbytes", true, "4");
      policy.addToken("timeout", false);
      _configTokens.push_back(policy);
      TokenList policytypes;
      policytypes.addToken("whitelist", false);
      policytypes.addToken("greylist", false);
      _configTypeTokens.push_back(policytypes);
      
      TokenList server;
      server.addToken("port", true, "4096");
      server.addToken("listen", true, "0.0.0.0");
      server.addToken("sparethreads", true, "2");
      server.addToken("maxthreads", true, "150");
      server.addToken("maxrequests", true, "0");
      server.addToken("cacherefresh", true, "600"); // seconds
      server.addToken("poolrefresh", true, "5"); // seconds
      server.addToken("deferwithstatus", true, "true");
      server.addToken("defer", true, "defer_if_permit Service is unavailable");
      server.addToken("dunno", true, "dunno");
      server.addToken("onerror", true, "dunno");
      server.addToken("log", true, "minimal");
      server.addToken("stats", true, "120"); // ten minutes if pool refresh is 5
      //       server.addToken("defertext", true, "Service is unavailable"); 

      _configTokens.push_back(server);
      
      _configBooleanTokens.addToken("false", false);
      _configBooleanTokens.addToken("true", false);
      
      _postfixResponseTokens.addToken("dunno", false);
      _postfixResponseTokens.addToken("defer_if_permit", false);
      
      _logTokens.addToken("minimal", false);
      _logTokens.addToken("full", false);
      
    } catch(std::exception &e) {
      std::cerr << "(Tokens::Tokens) something bad happened (Tokens::Tokens): " << e.what() << std::endl;
    }
  }
public:
//   static Tokens *instance();
  typedef enum {
    client_address,
    sender,
    recipient ,
    reverse_client_name,
    client_name,
    PostfixTokensType_min = client_address,
    PostfixTokensType_max = client_name
  } PostfixTokensType;
  const TokenList *postfixTokens() const { return &_postfixTokens; }
  typedef enum {
    database = 0,
    datasource,
    match,
    policy,
    server,
    SectionsTokensType_max = server
  } SectionsTokensType;
  const TokenList *configSectionsTokens() const { return &_configSectionsTokens; }
  const TokenList *configTokens(SectionsTokensType t) const { return &_configTokens.at(t); }
  const TokenList *configTypeTokens(SectionsTokensType t) const { return &_configTypeTokens.at(t); }
//   const std::string & qToken(int s, int i) const { return _configTokens.at(s)->getToken(i); }
  typedef enum {
    DBT_name,
    DBT_type,
    DBT_spareconnections
  } DatabaseTokensType;
  typedef enum {
    DST_name,
    DST_type,
    DST_table,
    DST_database
  } DataSourceTokensType;
  typedef enum {
    MT_name,
    MT_type,
    MT_datasource,
    MT_lookup,
    MT_match,    
    MT_return
  } MatchTokensType;
  typedef enum {	    // not pretty - mirrors triplet member type
    CT_client_address,
    CT_sender,
    CT_recipient,
    CT_reverse_client_name, // same as postfixTokens
    CT_client_name,
    CT_short_reverse_client_name,
    CT_short_client_name,
    CT_triplet_string,
    CT_network  
  } ColumnTokensType;  
  const TokenList *configColumnTokens() const { return &_configColumnsTokens; }
  typedef enum {
    PT_name,
    PT_type,
    PT_match,
    PT_datasource,
    PT_mode,
    PT_weakbytes,
    PT_timeout
  } PolicyTokensType;
  typedef enum {
    GT_reverse,
    GT_weak,
    GT_normal/*,
    GT_init  */  
  } GreylistTokensType;
  const TokenList *configGreylistTokens() const { return &_configModeTokens; }
  typedef enum {
    MTT_exact_match,
    MTT_pattern_match,
    MTT_cached_exact_match,
    MTT_cached_pattern_match,
    MTT_all
  } MatchTypeTokensType;
  typedef enum {
    DSTT_db
  } DataSourceTypeTokensType;
  typedef enum {
    PTT_whitelist,
    PTT_greylist
  } PolicyTypeTokensType;
  typedef enum {
    ST_port,
    ST_listen,
    ST_sparethreads,
    ST_maxthreads,
    ST_maxrequests,
    ST_cacherefresh,
    ST_poolrefresh,
    ST_deferwithstatus,
    ST_defer,
    ST_dunno,
    ST_onerror,
    ST_log,
    ST_statsevery
  } ServerTokensType;
  typedef enum {
    BT_false,
    BT_true
  } BooleanTokensType;
//   const TokenList *configMatchTypeTokens() const { return &_configMatchTypeTokens; }
  const TokenList *configBooleanTokens() const { return &_configBooleanTokens; }
  typedef enum {
    PFT_dunno,
    PFT_defer
  } PostfixResponseTokensType;
  const TokenList *postfixResponseTokens() const { return &_postfixResponseTokens; }
  typedef enum {
    LT_minimal,
    LT_full
  } LogTokensType;
  const TokenList *logTokens() const { return &_logTokens; }
  ~Tokens() { }
  
private:
  std::vector<TokenList> _configTokens, _configTypeTokens;
};

#endif // TOKENS_H
