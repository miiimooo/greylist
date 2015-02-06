#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include "Database.h"
#include "Query.h"

using namespace std;

int main()
{
	Database db("localhost","root","","my_db");
	if (!db.Connected())
	{
		printf("Database not connected - exiting\n");
		exit(-1);
	}
	Query q(db);
// 	cout << q.GetError() << endl;

//	q.execute("delete from user");
//	q.execute("insert into user values(1,'First Person')");
//	q.execute("insert into user values(2,'Another Person')");

	q.get_result("select * from log");
	cout << "numrows: " << q.num_rows() << endl;
// 	cout << q.GetError() << endl;
	while (q.fetch_row())
	{
// 		long id = q.getval("id");
// 		std::string table = q.getstr("table");
		long id = q.getval(); 
		std::string table = q.getstr(5);
		cout << "id\t" << id << "\ttable\t" << table << endl;
// 		cout << q.GetError() << endl;
		//printf("User#%ld: %s\n", num, name.c_str() );
	}
	q.free_result();
}

