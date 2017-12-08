#include "includes/spell.h"

using namespace std;
using namespace pqxx;

Spell::Spell(const string name) {
    const char *sql;

    cout << "Connecting..." << endl;
    try {
        connection c(CONNECTION_STRING);
        if (c.is_open())
            cout << "Success !" << endl;
        else
            cout << "Can't open the database" << endl;

        sql = "SELECT * FROM spell";
        nontransaction n(c);
        result r(n.exec(sql));

        for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
            if (c[0].as<string>() != name)
                continue;
            spellName = c[0].as<string>();
            ballSpeed = c[1].as<int>();
            ballSpeed_o = c[2].as<int>();
            paddleSize = c[3].as<int>();
            paddleSize_o = c[4].as<int>();
            paddleSpeed = c[5].as<int>();
            paddleSpeed_o = c[6].as<int>();
            damages = c[7].as<int>();
            damages_o = c[8].as<int>();
            shield = c[9].as<int>();
            ball_multiplication = c[10].as<int>();
            ball_multiplication_o = c[11].as<int>();
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void Spell::print() {
    cout << spellName << "," << ballSpeed << "," << ballSpeed_o << "," << paddleSize << "," << paddleSize_o << ","
         << paddleSpeed << "," << paddleSpeed_o << "," << damages << "," << damages_o << "," << shield << ","
         << ball_multiplication << "," << ball_multiplication_o << endl;
}
