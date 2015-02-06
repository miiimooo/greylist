/// $Id: query_quote_escape.cpp 21 2009-09-05 23:07:59Z mimo $
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include "dbiwrapped.h"

using namespace std;

#define TABLE_TRIPLETS "triplet"
#define ATTR_RECIPIENT "recipient"
#define ATTR_SENDER "sender"

int main() {
  string strSender, strRecipient, strClientAddress, strReverseClientName;
  bool bVerbose;
  
  bVerbose = true;
  strSender = "me@t%hisp'lace.com";
  strRecipient = "null@t61";
  strClientAddress = "127.0.0.1";
  strReverseClientName = "localhost";
  
  
//   cout << "SELECT id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
"ATTR_SENDER"=" << strSender << " AND "ATTR_RECIPIENT"=" << strRecipient << endl;

  StderrLog logger;
//   SysLog logger;
  boost::shared_ptr<Database> db(new Database("localhost","root","","greylist", &logger));
  if (!db->Connected()) {
    printf("Database not connected - exiting\n");
    exit(-1);
  }
  Query q(db);

//   q.get_result("SELECT id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
"ATTR_SENDER"='" + strSender + "' AND "ATTR_RECIPIENT"='" + strRecipient + "')");
  Query::Arguments sqlArgs;
  sqlArgs.push_back(strSender);
  sqlArgs.push_back(strRecipient);
  sqlArgs.push_back(StrmConvert(5));
  q.execute("TRUNCATE "TABLE_TRIPLETS);
  cout << "Query: " << q.GetLastQuery() << endl;
  q.free_result();
  q.executef("INSERT INTO "TABLE_TRIPLETS" ( \
"ATTR_SENDER","ATTR_RECIPIENT",count) VALUES(%s,%s,%d)", sqlArgs);
  cout << "Query: " << q.GetLastQuery() << endl;
  cout << "Inserted ID: " << q.insert_id() << endl;
  unsigned long long last_id = q.insert_id();
  q.free_result();
  q.get_resultf("SELECT "ATTR_SENDER","ATTR_RECIPIENT",id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
"ATTR_SENDER"=%s AND "ATTR_RECIPIENT"=%s)", sqlArgs);
//   q.executef("INSERT INTO "TABLE_TRIPLETS" ( \
"ATTR_SENDER","ATTR_RECIPIENT") VALUES(%s,%s)", sqlArgs);
  
  if (q.GetErrno()) {
    cerr << q.GetError() << endl;
  }
  int rows = q.num_rows();/*dbi_result_get_numrows(result);*/
  cout << "Query: " << q.GetLastQuery() << endl;
  cout << rows << " rows returned" << endl;
  if (rows != 1) {
    cerr << "Must return one row" << endl;
    exit(-1);
  }
  bool bFound = false;
  while (q.fetch_row()) {
    cout << q.getulonglongval(3) << "# " << q.getstr(1) << ", " << q.getstr(2) << ", " << q.getlongval(4) << endl;
    if (q.getstr(1) != strSender) {
      cerr << "senders don't match: " << q.getstr(1) << "!=" << strSender <<  endl;
      exit(-1);
    }
    if (q.getstr(2) != strRecipient) {
      cerr << "recipients don't match: " << q.getstr(2) << "!=" << strSender <<  endl;
      exit(-1);
    }
    if (q.getlongval(4) != 5) {
      cerr << "count doesn't match: " << q.getlongval(4) << "!=" << 5 <<  endl;
      exit(-1);
    }
    if (q.getulonglongval(3) != last_id) {
      cerr << "id doesn't match: " << q.getulonglongval(3) << "!=" << last_id <<  endl;
      exit(-1);
    }
    if (q.fetch_row()) {
      cerr << "fetch_row returns more rows than num_rows" << endl;
      exit(-1);
    }
    bFound = true;
  }
  if (!bFound) {
    cerr << "fetch_row returns fewer rows than num_rows" << endl;
    exit(-1);
  }
  q.free_result();
  return(0);
}