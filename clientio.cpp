#include <iostream>
#include "includes/clientio.h"

using namespace std;
using namespace pqxx;

void
send_message_to_client(Client *clients, Client receiver, std::string buffer) {
    int i = 0;
    write_client(receiver.sock, buffer);
    cout << "Sent " << buffer << " to " << clients[i].login << endl;

}

void write_client(SOCKET sock, const string buffer) {
    if (send(sock, buffer.c_str(), buffer.length(), 0) < 0) {
        perror("send()");
        exit(errno);
    }
}

string read_client(SOCKET sock) {
    int n = 0;
    char buf[BUF_SIZE];

    if ((n = (int) recv(sock, buf, BUF_SIZE - 1, 0)) < 0) {
        perror("recv()");
        return "";  // If recv error we disconnect the client
    }

    buf[n] = 0;
    string out = buf;

    if (out.empty())
        return "";
    cout << "Received " << out << " from socket " << sock << endl;
    return out;
}

/// Protocol

void p_BPING(Client clients[], int k) {
    p_BPONG(clients, k);
}

void p_BPONG(Client clients[], int k) {
    send_message_to_client(clients, clients[k], "BPONG");
}

void p_BAUTH(Client clients[], int k, string buffer) {
    string login, pwd;
    vector<string> v = split(buffer, ',');
    if (v.size() < 3) {
        send_message_to_client(clients, clients[k], "T'es qu'une grosse merde");
        return;
    }
    login = v[1];
    pwd = v[2];

    p_BLOGIN(clients, k, login, pwd);
}

void p_BLOGIN(Client clients[], int k, string login, string pwd) {
    string db_pwd, sql;

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM player_account WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.size() != 1) {
            send_message_to_client(clients, clients[k], "BLOGIN,2");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci)
            db_pwd = ci[1].as<string>();

        if (db_pwd != pwd) {
            send_message_to_client(clients, clients[k], "BLOGIN,1");
            return;
        }

        send_message_to_client(clients, clients[k], "BLOGIN,0");
        clients[k].is_auth = true;
        clients[k].login = login;

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BCLASSESR(Client clients[], int k, string buffer) {
    vector<string> a = split(buffer, ',');

    p_BCLASSESA(clients, k, a[1]);
}

void p_BCLASSESA(Client clients[], int k, string login) {
    string sql, classes = "BCLASSESA";

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM has_class WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.empty()) {
            send_message_to_client(clients, clients[k], "BCLASSESA,NULL");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci) {
            classes += "," + ci[0].as<string>();
            clients[k].heroList.push_back(ci[0].as<string>());
        }
        send_message_to_client(clients, clients[k], classes);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BSPELLSR(Client *clients, int k, string buffer) {
    // TODO Interroger la BD pour avoir les sorts disponibles pour un login
    vector<string> a;
    if (split(buffer, ',').size() < 2)
        return;
    p_BSPELLSA(clients, k, a[1]);
}

void p_BSPELLSA(Client *clients, int k, std::string login) {
    string sql, spells = "BSPELLSA";

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM spell WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.empty()) {
            send_message_to_client(clients, clients[k], "BSPELLSA,NULL");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci) {
            for (unsigned int i = 0; i < ci.size(); i++)
                spells += "," + ci[i].as<string>();
        }

        send_message_to_client(clients, clients[k], spells);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}   // WAIT Implémentation en attente de MAJ BD

void p_BSPELLSC(Client *clients, int k, std::string chosenList) {
    vector<string> cl = split(chosenList, ',');
    for (const string &spell : cl) {
        auto s = Spell(spell);
        if (!s.getSpellName().empty())
            clients[k].spellsList.push_back(s);
    }
    p_BSPELLSACK(clients, k);
}

void p_BSPELLSACK(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "BSPELLSACK");
}

void p_BMAKE(Client *clients, int k) {
    p_BWAIT(clients, k);
}

void p_BWAIT(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "BWAIT");
}

void p_BMATCH(Client *clients, int k, Client p1, Client p2) {
    send_message_to_client(clients, clients[k], p1.toString() + "," + p2.toString());
}

void p_BFIGHT(Client *clients, int k, std::string buffer) {
    vector<string> buf = split(buffer, ',');
    if (buf.size() > 1) {
        string spell = buf[1];
        cout << "Used spell : " << Spell(spell).toString() << endl;
    }
} // TODO Revoir les arguments de la fonction

void p_BREF(Client *clients, int k, Client p1, Client p2) {
    send_message_to_client(clients, clients[k], p1.toString() + "," + p2.toString());
}

void p_BWIN(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "BWIN");
} // TODO Calculer le nouvel elo du joueur et lui envoyer

void p_BLOSE(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "BLOSE");
} // TODO Calculer le nouvel elo du joueur et lui envoyer

void p_BLOGOUT(Client *clients, int k, int *actual) {
    p_BBYE(clients, k, actual);
} // TODO Gérer la déconnexion du client

void p_BBYE(Client *clients, int k, int *actual) {
    send_message_to_client(clients, clients[k], "BBYE");
    clients[k].is_auth = false;
    remove_client(clients, k, actual);
}

void p_BTIMEDOUT(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "BBYE");
}

void p_BFALL(Client *clients, int k) {
    send_message_to_client(clients, clients[k], "LOL T NUL");
} // TODO Ajouter une fonction permettant de faire perdre un PV au joueur
