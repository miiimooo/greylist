/// $Id: dbi_mysql.cpp 64 2010-08-12 21:58:53Z mimo $
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <dbi/dbi.h>
#include <dbi/dbi-dev.h>
#include "Database.h"
#include "Query.h"
#include "dbierror.h"

using namespace std;
using namespace boost;
#if 0
int main() {
  dbi_initialize(0);
  for (int i=0; i < 100; ++i) {
    cout << i << endl; 
    dbi_conn con = dbi_conn_new("mysql");
    dbi_conn_set_option(con, "username", "root");
    dbi_conn_set_option(con, "dbname", "greylist");
    if (dbi_conn_connect(con) != 0) {
      cerr << "dbi_conn_connect() failed" << endl;
    }
    
    dbi_conn_close(con);
  }
//   dbi_conn_t *pcon = (dbi_conn_t*) con;
//   cout << pcon->current_db << endl;
//   free(pcon->current_db);
//   pcon->current_db = 0;
  dbi_shutdown();
  return 0;
}
#endif
#if 1
/// force a duplicate key error 1062: Duplicate entry '1-1' for key 'id'
int main()
{
	Database::Mutex mutex;
// 	exit(-1);
	shared_ptr<Database> db(new Database(mutex, "localhost","root","","greylist"));
	if (!db->Connected())
	{
		printf("Database not connected - exiting\n");
		exit(-1);
	}
	Query q(db);

        q.execute("DROP TABLE IF EXISTS `dbitest`");
        cout << "Result ("<< q.GetLastQuery() << "): " << q.GetError() << endl;
        q.free_result();
        q.execute("CREATE TABLE `greylist`.`dbitest` ( " 
                "`id` INT( 11 ) NOT NULL ,"
                "`data` INT( 11 ) NOT NULL"
                ") ENGINE = MYISAM ;");
        cout << "Result ("<< q.GetLastQuery() << "): " << q.GetError() << endl;
        q.free_result();
        
        q.execute("ALTER TABLE `dbitest` ADD UNIQUE (`id` , `data`);");
        cout << "Result ("<< q.GetLastQuery() << "): " << q.GetError() << endl;
        q.free_result();

        q.execute("INSERT INTO `dbitest` VALUES(1,1)");
        cout << "Result ("<< q.GetLastQuery() << "): " << q.GetError() << endl;
        q.free_result();
        try {
          q.execute("INSERT INTO `dbitest` VALUES(1,1)");
          cout << "Result ("<< q.GetLastQuery() << "): " << q.GetError() << endl;
          q.free_result();
        } catch(DBIError &e) {
          cout << "caught DBIError: " << e.what() << endl;
          cout << e.getDBIErrorNumber() << endl;
        }
        // 	cout << q.GetError() << endl;

//	q.execute("delete from user");
//	q.execute("insert into user values(1,'First Person')");
//	q.execute("insert into user values(2,'Another Person')");

	q.get_result("select * from triplet");
	cout << "numrows: " << q.num_rows() << endl;
// 	cout << q.GetError() << endl;
	while (q.fetch_row())
	{
// 		long id = q.getval("id");
// 		std::string table = q.getstr("table");
		long id = q.getulonglongval(1); 
		
		std::string sender = q.getstr(3);
		cout << "id\t" << id << "\tsender\t" << sender << endl;
// 		cout << q.GetError() << endl;
		//printf("User#%ld: %s\n", num, name.c_str() );
	}
	q.free_result();
}

#endif