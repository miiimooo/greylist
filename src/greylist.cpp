/// $Id: greylist.cpp 72 2011-08-10 21:39:36Z mimo $
#include <iostream>
// #include "greylist.h"
// #include "dbiwrapped.h"
#include "Query.h"
#include "Database.h"

using namespace std;
/*
request=smtpd_access_policy   
protocol_state=RCPT                           
protocol_name=ESMTP                           
client_address=127.0.0.1                      
client_name=localhost                         
reverse_client_name=localhost
helo_name=localhost
sender=me@thisplace.com
recipient=null@t61
recipient_count=0
queue_id=
instance=6a3c.4a3d21ed.25173.3d
size=0
etrn_domain=
stress=
sasl_method=
sasl_username=
sasl_sender=
ccert_subject=
ccert_issuer=
ccert_fingerprint=
encryption_protocol=
encryption_cipher=
encryption_keysize=0
*/
#define TABLE_TRIPLETS "triplet"
#define ATTR_RECIPIENT "recipient"
#define ATTR_SENDER "sender"

// void oldprocess() {
//   string strSender, strRecipient, strClientAddress, strReverseClientName;
//   bool bVerbose;
//   
//   bVerbose = true;
//   strSender = "me@thisp'lace.com";
//   strRecipient = "null@t61";
//   strClientAddress = "127.0.0.1";
//   strReverseClientName = "localhost";
//   
//   
// //   cout << "SELECT id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
// "ATTR_SENDER"=" << strSender << " AND "ATTR_RECIPIENT"=" << strRecipient << endl;
// 
//   StderrLog logger;
// //   SysLog logger;
//   Database db("localhost","root","","greylist", &logger);
//   if (!db.Connected()) {
//     printf("Database not connected - exiting\n");
//     exit(-1);
//   }
//   Query q(db);
// 
// //   q.get_result("SELECT id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
// "ATTR_SENDER"='" + strSender + "' AND "ATTR_RECIPIENT"='" + strRecipient + "')");
//   Query::Arguments sqlArgs;
//   sqlArgs.push_back(strSender);
//   sqlArgs.push_back(strRecipient);
//   q.execute("TRUNCATE "TABLE_TRIPLETS);
//   cout << "Query: " << q.GetLastQuery() << endl;
//   q.free_result();
//   q.executef("INSERT INTO "TABLE_TRIPLETS" ( \
// "ATTR_SENDER","ATTR_RECIPIENT") VALUES(%s,%s)", sqlArgs);
//   cout << "Query: " << q.GetLastQuery() << endl;
//   q.free_result();
//   q.get_resultf("SELECT id,count,uts FROM "TABLE_TRIPLETS" WHERE ( \
// "ATTR_SENDER"=%s AND "ATTR_RECIPIENT"=%s)", sqlArgs);
// //   q.executef("INSERT INTO "TABLE_TRIPLETS" ( \
// "ATTR_SENDER","ATTR_RECIPIENT") VALUES(%s,%s)", sqlArgs);
//   
//   if (q.GetErrno()) {
//     cerr << q.GetError() << endl;
//   }
//   int rows = q.num_rows();/*dbi_result_get_numrows(result);*/
//   if (rows == 0) {
//     
//   }
//   cout << "Query: " << q.GetLastQuery() << endl;
//   cout << rows << " rows returned" << endl;
//   q.free_result();
// }