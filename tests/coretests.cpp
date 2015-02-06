/**   $Id: coretests.cpp 8 2009-08-23 03:21:36Z mimo $   **/
 
//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include "../TestSuite/Suite.h"
#include <string>
#include <cstddef>

#include "defensive.h"
#include "core.h"
#include "triplet.h"
#include "allmatcher.h"

using namespace std;
using namespace TestSuite;
#ifndef WHITELIST_H
class Whitelist {
  public:
    Module::ActionType process(Triplet &) {
      // log it
      return Module::dunno;
    }
};
#endif
class NoMatcher
{
  public:
    Module::ActionType process(class Triplet &) { 
      return Module::differ;
    }
};
class Pass {
  public:
    Module::ActionType process(class Triplet &) { 
      return Module::pass;
    }
};
class Defer {
  public:
    Module::ActionType process(class Triplet &) { 
      return Module::defer;
    }
};
typedef TmplModule<NoMatcher> NoMatcherModule;
typedef TmplModule<Pass> PassModule;
typedef TmplModule<Defer> DeferModule;

typedef TmplModule<Whitelist> WhitelistModule;
typedef TmplModule<AllMatcher> AllMatcherModule;


class CoreTests : public TestSuite::Test {
public:
  void run() {
//     auto_ptr<Module> apMod(new AllMatcherModule);
//     Core::ModuleType cmt(apMod, apMod);
/*    cout << "Legend: match=" << Module::match << " defer=" << Module::defer << " dunno=" << Module::dunno
      << " pass=" << Module::pass << endl; */
    Triplet t1("216.145.54.171", "someuser@mydomain.org", "someuser@yahoo.com", "", "smtp.yahoo.com");
    Core core;
//     cout << "(Pass, All), (Whitelist, No), (Defer, All)" << endl;
    core.push_back(Core::ContainerType::value_type(new PassModule, new AllMatcherModule));
    core.push_back(Core::ContainerType::value_type(new WhitelistModule, new NoMatcherModule));
    core.push_back(Core::ContainerType::value_type(new DeferModule, new AllMatcherModule));
    test_(core.process(t1) == Module::defer);
    
    core.clear();
    core.push_back(Core::ContainerType::value_type(new WhitelistModule, new AllMatcherModule));
    test_(core.process(t1) == Module::dunno);
    
    core.clear();
    for(int i=0; i<32768;i++) {
      core.push_back(Core::ContainerType::value_type(new PassModule, new AllMatcherModule));
    }
    test_(core.process(t1) == Module::dunno);
    core.push_back(Core::ContainerType::value_type(new WhitelistModule, new AllMatcherModule));
    test_(core.process(t1) == Module::dunno);
    test_(core.process(t1) == Module::dunno);
    test_(core.process(t1) == Module::dunno);
  }
};
 
int main() {
    Suite suite("Core Tests");
    suite.addTest(new CoreTests);
    suite.run();
    long nFail = suite.report();
    suite.free();
    return nFail;
}
