/// $Id: system.h 62 2009-10-14 00:15:12Z mimo $
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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include <map>
#include <signal.h>
#include <syslog.h>
#include <boost/smart_ptr.hpp>
#include <boost/mpl/int.hpp>

#include "../config.h"
#include "defensive.h"
#include "triplet.h"
#include "core.h"
#include "tokenlist.h"
#include "tokens.h"
#include "parser.h"
#include "serversettings.h"
#include "typedefs.h"
#include "Database.h"
#include "server.h"
#include "tcplistener.h"
#include "databasecreator.h"
#include "dbdatasourcecreator.h"
#include "matchcreator.h"
#include "whitelistcreator.h"
#include "greylistcreator.h"
#include "servercreator.h"

volatile sig_atomic_t & setupSignalHandlers();

template <class Reader>
class System
{
  class ConfigWriter;
  class ConfigReader;
  typedef std::string (*SectionCreatorCallback)(System<Reader>&, ConfigWriter &, ServerSettings&, int);						   
  typedef std::string (*SubSectionCreatorCallback)(System<Reader>&, ConfigWriter &, boost::shared_ptr<Core>, int, int);						   
  typedef std::map<int,SectionCreatorCallback> SectionFactory;
  typedef std::map<std::pair<int,int>,SubSectionCreatorCallback> SubSectionFactory;
  SectionFactory _sectionFactory;
  SubSectionFactory _subSectionFactory;
  boost::shared_ptr<Core> _core;
  typedef std::map<std::string, boost::shared_ptr<Database> > DatabaseContainer;
  DatabaseContainer _databases;
  typedef std::map<std::string, boost::shared_ptr<DataSource> > DataSourceContainer;
  DataSourceContainer _datasources;
  typedef std::map<std::string, boost::shared_ptr<Core::Matcher> > MatchContainer;
  MatchContainer _matchers;
  Database::Mutex _dbiMutex;
  
  typedef DatabaseCreator<System<Reader>, ConfigWriter, DatabaseContainer, 
    Database::Mutex> DatabaseCreatorType;
  typedef DBDataSourceCreator<System<Reader>, ConfigWriter, DatabaseContainer, 
    DataSourceContainer, DBDataSourceType> DBDataSourceCreatorType;
  template<class MT>
  class TmplMatchCreator : public MatchCreator<System<Reader>, ConfigWriter, 
    DataSourceContainer, MatchContainer, MT> { };  
  
  typedef WhitelistCreator<System<Reader>, ConfigWriter, MatchContainer, 
    WhitelistPolicy>WhitelistPolicyCreator;
  typedef GreylistCreator<System<Reader>, ConfigWriter, DataSourceContainer,
    MatchContainer, GreylistPolicy>GreylistPolicyCreator;
    
public:
  System() {
    
    _sectionFactory.insert(typename SectionFactory::value_type(Tokens::database, &DatabaseCreatorType::create));
    // datasources
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::datasource, Tokens::DSTT_db), &DBDataSourceCreatorType::create));
    // matchers
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::match, Tokens::MTT_exact_match), 
      &TmplMatchCreator<ExactMatchMatcher>::create));
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::match, Tokens::MTT_pattern_match), 
      &TmplMatchCreator<PatternMatchMatcher>::create));
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::match, Tokens::MTT_cached_exact_match), 
      &TmplMatchCreator<CachedExactMatchMatcher>::create));
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::match, Tokens::MTT_cached_pattern_match), 
      &TmplMatchCreator<CachedPatternMatchMatcher>::create));
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::match, Tokens::MTT_all), 
      &TmplMatchCreator<AllMatchMatcher>::create));
    // policies  
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::policy, Tokens::PTT_whitelist), &WhitelistPolicyCreator::create));
    _subSectionFactory.insert(typename SubSectionFactory::value_type(
      std::pair<int,int>(Tokens::policy, Tokens::PTT_greylist), &GreylistPolicyCreator::create));

    _sectionFactory.insert(typename SectionFactory::value_type(Tokens::server, 
      &ServerCreator<System<Reader>, ConfigWriter>::create));
    // create the CacheRefresh singleton before anything that might use it to assure orderly shutdown
/*    _sectionFactory.insert(typename SectionFactory::value_type(Tokens::datasource, 
	std::mem_fun_ref(System<Reader>::subSectionCreate)));*/
  }
  DatabaseContainer & getDatabaseContainer() { return _databases; }
  DataSourceContainer & getDataSourceContainer() { return _datasources; }
  MatchContainer & getMatchContainer() { return _matchers; }
  Database::Mutex & getDatabaseMutex() { return _dbiMutex; }
  
  int run(Reader & reader) {
    ::openlog(PACKAGE_NAME, 0, LOG_MAIL);
    ::syslog(LOG_INFO, "%s started (release:%s built: %s %s) %s", 
	      PACKAGE_STRING, RELEASE_VERSION, __DATE__,__TIME__, 
	      PACKAGE_INFO);
    volatile sig_atomic_t & globalSignal = setupSignalHandlers();
    ServerSettings settings;
    boost::shared_ptr<Core> core(new Core);
    boost::shared_ptr<Server> srv;
    try {
      srv = compileConfig(reader, core, settings, globalSignal, false);
    } catch(std::exception &e) {
      std::cerr << "Error in startup: \n" << e.what() << std::endl;
      ::syslog(LOG_ERR, "Error in startup: %s", e.what()); 
      return -1;
    }
    bool bRun = true;
    while (bRun) {
      int socketerror = srv->run();
      if (!globalSignal) {
	std::string errstr(strerror(socketerror));
	throw std::runtime_error("Got unexepected error: " + errstr 
	+ "(" + Utils::StrmConvert(socketerror) + ")"); 
      }
      if (globalSignal == SIGQUIT) {
	std::cout << "got SIGQUIT, shutting down" << std::endl;
	::syslog(LOG_INFO, "got SIGQUIT, shutting down"); 
	return 0;
      }
      std::cout << "got SIGHUP ("<< globalSignal << "), reloading" << std::endl;
      ::syslog(LOG_INFO, "got SIGHUP, reloading"); 
      try {
	ServerSettings newSettings;
	boost::shared_ptr<Core> newCore(new Core);
	boost::shared_ptr<Server> newSrv;
// 	reader.push_back("port=81");
	reader.reload();
	newSrv = compileConfig(reader, newCore, newSettings, globalSignal, false);
	settings = newSettings;
	core = newCore;
	srv = newSrv;
	globalSignal = 0;
	continue;
      } catch(std::exception &e) {
	std::cerr << "Error in new configuration: \n" << e.what() 
	  << "\nRetrying with previous configuration" << std::endl;
	::syslog(LOG_ERR, "Error in new configuration: %s", e.what()); 
      }
      try {
	srv = compileConfig(reader, core, settings, globalSignal, true); // skip parsing config
      } catch(std::exception &e) {
	std::cerr << "Error in configuration, exiting: \n" << e.what() << std::endl;
	::syslog(LOG_ERR, "Error loading previous configuration: %s", e.what()); 
	return -2;
      }
      globalSignal = 0;
    }
  }
  boost::shared_ptr<Server> compileConfig(Reader & reader, boost::shared_ptr<Core> core,
		      ServerSettings &settings, volatile sig_atomic_t & globalSignal, 
		      bool skipConfig) {
    if (!skipConfig) {
      ConfigReader cfgReader(reader);
      parseConfigurationSections(cfgReader, core, 
				Tokens::instance().configSectionsTokens(), 
				settings);
    }
    int dws = Tokens::instance().configBooleanTokens()->findToken(settings[Tokens::ST_deferwithstatus]);
    bool deferwithstatus =  (dws == Tokens::BT_false ? false : true);  
    int defer = Tokens::instance().postfixResponseTokens()->findToken(settings[Tokens::ST_defer]);
    deferwithstatus = (defer == Tokens::PFT_dunno ? false : true); // no point if replying with dunno
    int loglevel = Tokens::instance().logTokens()->findToken(settings[Tokens::ST_log]);
    loglevel = (loglevel == -1 ? Tokens::LT_full : loglevel);
//     std::for_each(settings.begin(), settings.end(), &System<Reader>::dumpContainerWithTokens<ServerSettings>);
    boost::shared_ptr<TCPListener> listener (
      new TCPListener(settings[Tokens::ST_listen], settings[Tokens::ST_port]));
    listener->open();
    boost::shared_ptr<Server> server (
      new Server(globalSignal, listener, core, 
		  Utils::StrmConvert<int>(settings[Tokens::ST_sparethreads]), 
		  Utils::StrmConvert<int>(settings[Tokens::ST_maxthreads]), 
		  Utils::StrmConvert<int>(settings[Tokens::ST_poolrefresh]),
		  Utils::StrmConvert<int>(settings[Tokens::ST_cacherefresh]),		  
		  Utils::StrmConvert<int>(settings[Tokens::ST_maxrequests]), 
		  deferwithstatus, settings[Tokens::ST_defer], settings[Tokens::ST_dunno], 
		  settings[Tokens::ST_onerror], loglevel, 
		  Utils::StrmConvert<int>(settings[Tokens::ST_statsevery])));
    return server;
  }
  
  Core::ActionType testCore(Triplet &triplet) {
    Core::Extra extra;
    return _core->process(triplet, extra);
  }
  void parseConfigurationSections(ConfigReader &reader, 
      boost::shared_ptr<Core> core, const TokenList * sections, 
      ServerSettings &settings) {
    std::string line;
    int res = 0; 
    while(res != -1) {
      reader.switchSectionMode(false);
      res = reader.getline(line);
      if (res == -1) {
	break;
      }
      std::string::size_type left = line.find("[");
      std::string::size_type right = line.find("]");
      if ((left == std::string::npos) || (right == std::string::npos)) {
	 std::cerr << "ignored: " << line << std::endl;
	 continue;
      }
      std::string section = line.substr(left+1, right-left-1);
//       std::cout << section <<  " " << right-left << std::endl;
      int tid = sections->findToken(section); 
      if (tid == -1) {
	std::cerr << "unknown section: " << section << std::endl;
	continue;
      }
      reader.switchSectionMode(true);
      Tokens::SectionsTokensType stkid = Tokens::SectionsTokensType(tid);
      ConfigWriter cfgWriter;
      Parser<ConfigReader,ConfigWriter> parser(Tokens::instance().configTokens(stkid));
      parser.parse(reader, cfgWriter, true); /// true to enable variable mode
      typename SectionFactory::iterator sfitr = _sectionFactory.find(tid);
      if (sfitr != _sectionFactory.end()) {
	std::string sectionName = (*(*sfitr).second)(*this, cfgWriter, settings, tid);
	tracer << "created section '" << sectionName << "'" << std::endl;
	continue;
      }
      const std::string &typeStr = cfgWriter.getVal(Tokens::DST_type); // ehm
      if (typeStr.empty()) {
	throw UnknownConfig("Missing type in section [" 
	  + Tokens::instance().configSectionsTokens()->getToken(tid) + "]");
      }
      int typeId = Tokens::instance().configTypeTokens(stkid)->findToken(typeStr);
      if (typeId == -1) {
	throw BadType("Unknown type in section '" 
	+ Tokens::instance().configSectionsTokens()->getToken(tid) + "' "
	+ "'" + typeStr + "'");
      }
      typename SubSectionFactory::iterator itr = 
	_subSectionFactory.find(std::pair<int,int>(tid, typeId));
      if (itr == _subSectionFactory.end()) {
	throw std::logic_error("No sub section handler defined in section '"
	+ Tokens::instance().configSectionsTokens()->getToken(tid) + "' "
	+ "for type '" + typeStr + "'");
      }
      std::string sectionName = (*(*itr).second)(*this, cfgWriter, core, tid, typeId);
      tracer << "created section '" << sectionName << "'" << std::endl;
    }
    // TODO insert database to tester function here
    // TODO check for unused database, datasources, matchers via shared ptrs
    _matchers.clear();
    _datasources.clear();
    _databases.clear();
  }
  void checkForToken(const ConfigWriter & cfgWriter, Tokens::SectionsTokensType tid, 
		      unsigned s,
		      const std::string &sn) {
    if ((cfgWriter.getVal(s)).empty()) {
      throw MissingValue("missing value for '" 
      + Tokens::instance().configTokens(Tokens::SectionsTokensType(tid))->getToken(s)
      + "' in section ["
      + Tokens::instance().configSectionsTokens()->getToken(tid) + "]" 
      + (sn.empty() ? "" : " named '" + sn + "'" ));
    }
  }
  template <class DatabaseContainerType>
  typename DatabaseContainerType::const_iterator checkForRef(const ConfigWriter & cfgWriter, 
    const DatabaseContainerType &container, Tokens::SectionsTokensType sid, 
    Tokens::SectionsTokensType tid, unsigned s, const std::string &sn) {
    checkForToken(cfgWriter, tid, s, sn);
    typename DatabaseContainerType::const_iterator itr = container.find(cfgWriter.getVal(s));
    if (itr == container.end()) {
      std::string known;
      for (itr = container.begin(); itr != container.end(); ++itr) {
	if (itr != container.begin()) {
	  known += ", ";
	}
	known += "'" + (*itr).first + "'";
      }
      throw UnknownReference("unknown reference to " 
      + Tokens::instance().configSectionsTokens()->getToken(sid) + " '"
      + cfgWriter.getVal(s) 
      + "' in section ["
      + Tokens::instance().configSectionsTokens()->getToken(tid) + "]" 
      + (sn.empty() ? "" : " named '" + sn + "'" ) 
      + ", defined: " + (known.empty() ? "(none)" : known) );
    }
//     typename DatabaseContainerType::mapped_type ds(
//       new NewType((*itr).second, cfgWriter.getVal(t)));
    return itr;
  }
  
//   template<class CoreTypeFactory, class ContainerType>
//   void create(ConfigReader &reader, boost::shared_ptr<Core> core, ContainerType &container) {
    
//   }
private:  
  class ConfigReader {
    std::vector<std::string> _lines;
    std::vector<std::string>::iterator _itr;
    bool _sectionMode;
  public:
    ConfigReader(Reader &reader) : _lines(), _itr() {
      std::string line;
      while(reader.getline(line) != -1) {
	_lines.push_back(line);
      }
      _itr = _lines.begin();
    }
    int getline(std::string &line) {
      if (_itr == _lines.end() ) {
	return -1;
      }
      line = (*_itr);
      
      if (_sectionMode && (line.at(0) == '[') ) {
	return -1;
      }
      int len = (*_itr).length();
      ++_itr;
      return len;
    }
    void previousline() {
      if (_itr == _lines.begin()) {
	throw std::out_of_range("moving beyond start of configuration");
      }
      --_itr;
    }
    // in section mode we consider a line beginning with [ as eof
    void switchSectionMode(bool b) {
      _sectionMode = b;
    }
    bool isEof() const { return (_itr == _lines.end() ); }
    
  };
  class ConfigWriter {
    std::map<unsigned, std::string> _map;
    std::map<std::string, std::string> _vars;
    const std::string _strNull;
  public:
    ConfigWriter() : _map(), _vars(), _strNull() { }
    void add(unsigned u, const std::string & str) {
//       std::cout << u <<  " " << str << std::endl;
      _map.insert(std::map<unsigned, std::string>::value_type(u, str));
    }
    void add(const std::string & key, const std::string & str) {
//       std::cout << key <<  " " << str << std::endl;
      _vars.insert(std::map<std::string, std::string>::value_type(key, str));
    }
    const std::string & getVal(unsigned u) const {
      std::map<unsigned, std::string>::const_iterator itr = _map.find(u);
      return ( itr == _map.end() ? _strNull : (*itr).second);
    }
    const std::map<std::string, std::string> & getVars() const { return _vars; }
    template<class T>
    void copyVals(T &dest) const {
      dest.insert(_map.begin(), _map.end());
    }
  };
  template<class T>
  static void dumpContainerWithTokens(typename T::value_type &s) {
    std::cout << "#" << s.first << " " << s.second << std::endl;
  }
};

#endif // SYSTEM_H
