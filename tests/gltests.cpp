/**   $Id: gltests.cpp 64 2010-08-12 21:58:53Z mimo $   **/


//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include "../TestSuite/Suite.h"
#include <string>
#include <cstddef>
#include <memory>

// c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <deque>

// /c
// #include <gc/leak_detector.h>

// #include "zthread/Thread.h"
// #include "zthread/ThreadedExecutor.h"
// #include "zthread/AtomicCount.h"
// #include "zthread/Mutex.h"
// #include "zthread/Guard.h"
// #include "zthread/Condition.h"

#include "defensive.h"
#include "filedescriptor.h"
// #include "blockingaccept.h"
// #include "blockingacceptnoqueue.h"
#include "tcplistener.h"
// #include "prethreadedserver.h"
//#include "singleacceptthreadpool.h"

// #include "requesthandler.h"
#include "tokenlist.h"
#include "parser.h"
#include "tokens.h"
#include "postfixreader.h"

#include <boost/threadpool.hpp>
// #include <boost/thread/mutex.hpp>
// /*#include <boost/smart_ptr.hpp>*/

#include "core.h"
#include "greylist.h"
#include "dbdatasource.h"
#include "ipaddress.h"
#include "exactmatch.h"
// #include "cache.h"
#include "tmplcore.h"
#include "patternmatch.h"
#include "cachehandler.h"
#include "sharedcache.h"
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <readwriteable.h>
#include "typedefs.h"
#include "cachedexactmatch.h"

using namespace std;
using namespace TestSuite;
using namespace boost::threadpool;
using namespace Templates;
// using namespace Greylist;

class GlTests : public Test {
  public:
    void run() {
	StderrLog logger;
	//   SysLog logger;
	boost::shared_ptr<Database> db(new Database("localhost","root","","greylist", &logger));
	if (!db->Connected()) {
	  printf("Database not connected - exiting\n");
	  exit(-1);
	}
	boost::shared_ptr<DBDataSourceType> dbsource(new DBDataSourceType(db, "triplet"));
// 	Greylist *gl = new Greylist(&dbsource, Greylist::reverse, true, 1, 3); 
	boost::shared_ptr<GreylistPolicy> gl( 
	  new GreylistPolicy(dbsource, Greylist::normal, true, 1, 3)); 
// 	Greylist *gl = new Greylist(&dbsource, Greylist::weak, true, 1, 3); 
	// 	Triplet triplet("127.0.0.1", "me@thisp'lace.com", "null@t61", "", "localhost");
	IPAddress adr("216.145.54.171", "", "");
	cout << adr.getReverseName() << endl;
	cout << adr.getResolveError() << endl;
	cout << adr.getShortReverseName() << endl;
	cout << adr.getNumericIdx(0) << endl;
	cout << adr.getNumericIdx(1) << endl;
	cout << adr.getNumericIdx(2) << endl;
	cout << adr.getNumericIdx(3) << endl;
	Triplet triplet("216.145.54.171", "someuser@yahoo.com", "someuser@mydomain.com",
			 "mrout1.yahoo.com", "mrout1.yahoo.com");
// 	triplet.add(Tokens::client_address, "216.145.54.171");
// 	triplet.add(Tokens::sender, "someuser@yahoo.com");
// 	triplet.add(Tokens::recipient, "someuser@mydomain.com");
// 	triplet.add(Tokens::reverse_client_name, "mrout1.yahoo.com");
	Core::Extra extra;
	gl->decide(triplet, extra);
	Triplet triplet1("216.145.54.171", "someuser@yahoo.com", "someuser@mydomain.com",
			 "", "");
	gl->decide(triplet1, extra);

	boost::shared_ptr<DBDataSourceType> dbwlrecipient(
	  new DBDataSourceType(db, "recipient"));
	
	boost::shared_ptr<ExactMatchMatcher> exact( 
	  new ExactMatchMatcher(dbwlrecipient, "address", Triplet::recipient, "comment"));
	bool match = exact->match(triplet1, extra);
	if (match) {
	  cout << "matched!" << endl;
	}



	boost::shared_ptr<DBDataSourceType> dbwlpattern(new DBDataSourceType(db, "pattern"));

// 	TmplDataSource<Cache> dbwlcache(&dbwlpattern, "(unused) recipient");
// 	TmplMatcher<PatternMatch> *pattern = 
// 	  new TmplMatcher<PatternMatch>(&dbwlcache, "expression", Triplet::triplet_string, "comment");
	boost::shared_ptr<PatternMatchMatcher> pattern(
	  new PatternMatchMatcher(dbwlpattern, "expression", Triplet::triplet_string, "comment"));
	match = pattern->match(triplet1, extra);
	if (match) {
	  cout << "pattern matched!" << endl;
	}
	
// 	sleep(1);
	Core glcore;
//	glcore.push_back(pattern, gl); 
	
	Triplet triplet2("192.94.73.1", 
			  "someuser@yahoo.com", 
			  "someuser@mydomain.com",
			  "", "" );
			  
	
/*	Core::ActionType action = glcore.process(triplet2);
	for(int i=0; i < 10; i++) {
	  cout << "Action=";
	  switch(action) {
	    case Core::dunno:
	      cout << "dunno" << endl;
	      break;
	    case Core::defer:
	      cout << "defer" << endl;
	      break;
	    case Core::cont:
	      cout << "cont" << endl;
	      break;
	  }
	  action = glcore.process(triplet2);
	}*/
	    //       FileDescriptor stdin(0);
//       TCPListener *listener = new TCPListener("", "4096");
//       test_(listener != 0);
//       SingleAcceptThreadPool<PfRequestHandler>* server = 
// 	new SingleAcceptThreadPool<PfRequestHandler>(listener, 50, 10, 100, 5000);
//       Thread t(server);
//       t.wait();
//       server->run();
//       cout << "server finished"<<endl;
//       delete server;
//       Task t(&server);
    }
};

class CacheTests : public Test {
  public:
    void run() {
      StderrLog logger;
      //   SysLog logger;
      boost::shared_ptr<Database> db(new Database("localhost","root","","greylist", &logger));
      if (!db->Connected()) {
	printf("Database not connected - exiting\n");
	exit(-1);
      }
      boost::shared_ptr<DBDataSourceType> dbsource(new DBDataSourceType(db, "triplet"));
      boost::shared_ptr<GreylistPolicy> gl( 
	new GreylistPolicy(dbsource, Greylist::normal, true, 1, 3)); 
      Triplet triplet("216.145.54.171", "someuser@yahoo.com", "someuser@mydomain.com",
		       "mrout1.yahoo.com", "mrout1.yahoo.com");
      Core::Extra extra;
//       boost::shared_ptr<DBDataSourceType> cachesource(new DBDataSourceType(db, "recipient"));
      boost::shared_ptr<DBDataSourceType> dbwlrecipient(
	new DBDataSourceType(db, "recipient"));
      
      
      boost::shared_ptr<ExactMatchMatcher> exact(
	new ExactMatchMatcher(dbwlrecipient, "address", Triplet::recipient, "comment"));
      bool match = exact->match(triplet, extra);
      if (match) {
	cout << "matched!" << endl;
      }
      // 	sleep(1);
      Core glcore;
//       glcore.push_back(exact, gl); 
      
      Triplet triplet2("192.94.73.1", 
			"someuser@yahoo.com", 
			"someuser@mydomain.com",
			"", "");
			
			
/*      Core::ActionType action = glcore.process(triplet2);
      cout << "Action=";
      switch(action) {
	case Core::dunno:
	  cout << "dunno" << endl;
	  break;
	case Core::defer:
	  cout << "defer" << endl;
	  break;
	case Core::cont:
	  cout << "cont" << endl;
	  break;
      }*/
    }
};
void pr_action(Core::ActionType a) {
  cout << "Action=";
  switch(a) {
    case Core::dunno:
      cout << "dunno";
      break;
    case Core::defer:
      cout << "defer";
      break;
    case Core::cont:
      cout << "cont";
      break;
  }
  cout  << endl;
}
class SharedCacheTests : public Test {
  public:
    struct TestCache : public SharedCache {
      DataSource *_ds;
      std::string _text;
      TestCache(DataSource *ds) : _ds(ds) {}
      struct Factory {
	TestCache *createCache(DataSource *ds) {
	  cout << "creating cache " << ds << endl;
	  return new TestCache(ds);
	}
      };
      ~TestCache() {
	cout << "destroying cache " << _ds << ": " << _text << endl;
      }
    };
    struct someData {
      someData() { cout << "creating " << __PRETTY_FUNCTION__ << endl; }
      ~someData() { cout << "destrying " << __PRETTY_FUNCTION__ << endl; }
      string _text;
    };
    struct someCache {
      someCache(int id=0) : _id(id) { cout << "creating " << __PRETTY_FUNCTION__ << " " << _id << endl; }
      ~someCache() { cout << "destrying " << __PRETTY_FUNCTION__ << " " << _id << endl; }
      string _text;
      int _id;
    };
    struct AThread {
      Core *_core;
      int _id;
      AThread(int id, Core *c) : _core(c), _id(id) { }
      void run() {
	typedef boost::minstd_rand base_generator_type;
	
	base_generator_type generator(42u);
	boost::uniform_int<> degen_dist(10, 10);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > deg(generator, degen_dist);
	for(int i = 0; i<100; i++) {
	  Core::Extra extra;
	  Triplet triplet("216.145.54.171", "someuser@yahoo.com", "someuser@mydomain.com",
			   "mrout1.yahoo.com", "mrout1.yahoo.com");
	  boost::this_thread::sleep(boost::posix_time::milliseconds(deg()));
	  if (i%5 == 0) {
// 	    boost::shared_ptr<someCache> p = CacheHandler<someCache>::instance().get(0);
// 	    if (p) {
// 	      p->_text = "shared cache yeah baby"; //not good - should have a lock on p
// 	    }
// 	    R/*/*WLock*/*/able<someData>::WLock wtl(_rw);
// 	    RWLockable<someData>::PtrType ptr = _rw->get();
// 	    ptr->_text = "still kicking after " + Utils::StrmConvert(i);
// 	    wtl->_text = "still kicking after " + Utils::StrmConvert(i);
// 	    SharedCache::WriterLock wl(p);
// 	    p->_text = "still kicking after " + Utils::StrmConvert(i);
	  }
	  if (i%10 == 0) {
// 	    boost::shared_ptr<someCache> sptrCache(new someCache(i));
// 	    CacheHandler<someCache>::instance().insert(0, sptrCache);
// 	    cout << "use count " << sptrCache.use_count() << endl;
	  }
// 	  RWLockable<someData>::RLock rdl(_rw);
// 	  RWLockable<someData>::RLock *rdl = _rw->createRLock();
// 	  RWLockable<someData>::PtrType ptr = _rw->get();
// 	  ptr->_text = "still kicking after " + Utils::StrmConvert(i);
	  // 	  SharedCache::ReaderLock rl(p);
	  cout << _id << " " << i << " " << " " 
	    << /*rdl->_text << */" sleep: " << deg() << endl;
	  _core->process(triplet, extra);
	}
	
      }
    };
    void run() {
      
//       RWLockable<someData> *rwTest = new RWLockable<someData>(new someData);
//       vector<RWLockable<someData> > vRW;
//       RWLockable<someData> rwTest(new someData);
//       RWLockable<someData>::RLock rl(&rwTest);
      StderrLog logger;
      //   SysLog logger;
      Database::Mutex dbMutex;
      
      boost::shared_ptr<Database> db(new Database(dbMutex, "localhost","root","","greylist", &logger));
      if (!db->Connected()) {
	printf("Database not connected - exiting\n");
	exit(-1);
      }
      boost::shared_ptr<Database> dbrec(new Database(dbMutex, "localhost","root","","greylist", &logger));
      if (!db->Connected()) {
	printf("Database not connected - exiting\n");
	exit(-1);
      }
      
/*      boost::shared_ptr<someCache> sptrCache(new someCache(0));
      CacheHandler<someCache>::instance().insert(0, sptrCache);*/
      
      boost::shared_ptr<DBDataSourceType> dsrec(new DBDataSourceType(dbrec, "recipient"));
      cout << dsrec.use_count() << endl;
//       boost::shared_ptr<someCache> sptrCache1(new someCache(1));
//       CacheHandler<someCache>::instance().insert(dsrec.get(), sptrCache1);
//       boost::shared_ptr<ExactMatchMatcher> exact(
// 	new ExactMatchMatcher(dsrec, "address", Triplet::recipient, "comment"));

      boost::shared_ptr<CachedExactMatchMatcher> exact(
	new CachedExactMatchMatcher(dsrec, "address", Triplet::recipient, "comment"));
	
	
      boost::shared_ptr<DBDataSourceType> dspat(new DBDataSourceType(dbrec, "pattern"));
/*      boost::shared_ptr<PatternMatchMatcher> pattern(
	new PatternMatchMatcher(dspat, "expression", Triplet::triplet_string, "comment"));*/
      boost::shared_ptr<CachedPatternMatchMatcher> pattern(
	new CachedPatternMatchMatcher(dspat, "expression", Triplet::triplet_string, "comment"));
	
      
      boost::shared_ptr<DBDataSourceType>  dsgl(new DBDataSourceType(db, "triplet"));
      boost::shared_ptr<GreylistPolicy>  gl(new GreylistPolicy(dsgl, Greylist::normal, true, 1, 3)); 
      
      boost::shared_ptr<Core> glcore(new Core);

      glcore->push_back(pattern, gl); 
      
      boost::threadpool::pool pool(50);
      for(int i=0; i < pool.size() ; i++) {
	boost::shared_ptr<AThread> pth(new AThread(i, glcore.get()));
	schedule(pool, boost::bind(&AThread::run, pth));
      }
      pool.wait();
      cout << gl.use_count() << endl;

    }
};
      

int main(int argc, char *argv[]) {
  Suite suite("Greylist Tests");
//   suite.addTest(new PoolTest(atoi(argv[1])));
//   suite.addTest(new ParamListTest);
//   suite.addTest(new GlTests);
//   suite.addTest(new CacheTests);
  suite.addTest(new SharedCacheTests);
//   suite.addTest(new SelectTest);
  //   suite.addTest(new Chatserver);
  suite.run();
  long nFail = suite.report();
  suite.free();
//   CHECK_LEAKS();
  return nFail;
}
