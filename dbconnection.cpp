#include <iostream>
#include <pqxx/pqxx>
#include "includes/db.h"
#include "includes/server.h"

using namespace std;
using namespace pqxx;

void db_connect() {
    const char *sql;

    cout << "Connecting..." << endl;
    try {
        connection c(CONNECTION_STRING);
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