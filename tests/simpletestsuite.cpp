 
//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include "../TestSuite/Suite.h"
#include <string>
#include <cstddef>

// #include "TrimTest.h"
using namespace std;
using namespace TestSuite;
 
inline std::string trim(const std::string& s) {
  if(s.length() == 0)
    return s;
  std::size_t beg = s.find_first_not_of(" \a\b\f\n\r\t\v");
  std::size_t end = s.find_last_not_of(" \a\b\f\n\r\t\v");
  if(beg == std::string::npos) // No non-spaces
    return "";
  return std::string(s, beg, end - beg + 1);
}

class TrimTest : public TestSuite::Test {
  enum {NTESTS = 11};
  static std::string s[NTESTS];
public:
  void testTrim() {
    test_(trim(s[0]) == "abcdefghijklmnop");
    test_(trim(s[1]) == "abcdefghijklmnop");
    test_(trim(s[2]) == "abcdefghijklmnop");
    test_(trim(s[3]) == "a");
    test_(trim(s[4]) == "ab");
    test_(trim(s[5]) == "abc");
    test_(trim(s[6]) == "a b c");
    test_(trim(s[7]) == "a b c");
    test_(trim(s[8]) == "a \t b \t c");
    test_(trim(s[9]) == "");
    test_(trim(s[10]) == "");
  }
  void run() {
    testTrim();
  }
};
// Initialize static data
std::string TrimTest::s[TrimTest::NTESTS] = {
  " \t abcdefghijklmnop \t ",
  "abcdefghijklmnop \t ",
  " \t abcdefghijklmnop",
  "a", "ab", "abc", "a b c",
  " \t a b c \t ", " \t a \t b \t c \t ",
  "\t \n \r \v \f",
  "" // Must also test the empty string
}; ///:~
 
int main() {
    Suite suite("String Tests");
//     suite.addTest(new StringStorageTest);
//     suite.addTest(new SieveTest);
//     suite.addTest(new FindTest);
//     suite.addTest(new RparseTest);
    suite.addTest(new TrimTest);
//     suite.addTest(new CompStrTest);
    suite.run();
    long nFail = suite.report();
    suite.free();
    return nFail;
}
/* Output:
s1 = 62345
s2 = 12345
Suite "String Tests"
====================
Test "StringStorageTest":
   Passed: 2   Failed: 0
Test "SieveTest":
   Passed: 50  Failed: 0
Test "FindTest":
   Passed: 9   Failed: 0
Test "RparseTest":
   Passed: 8   Failed: 0
Test "TrimTest":
   Passed: 11  Failed: 0
Test "CompStrTest":
   Passed: 8   Failed: 0
*/ 
