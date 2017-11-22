#include <iostream>
#include <pqxx/pqxx>
#include "server.h"

using namespace std;
using namespace pqxx;

void db_connect() {
    const char *sql;

    cout << "Connecting..." << endl;
    try {
        connection c("user=pguser password=password host=192.168.1.17 port=5432 dbname=postgres");
        if (c.is_open())
            cout << "Success !" << endl;
        else
            cout << "Can't open the database" << endl;

        sql = "SELECT * FROM tabletest";
        nontransaction n(c);
        result r(n.exec(sql));

        for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
            cout << "ID = " << c[0].as<int>() << "\t";
            cout << "Nom = " << c[1].as<string>() << endl;
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}