#include <iostream>
#include "includes/clientio.h"

using namespace std;
using namespace pqxx;

void
send_message_to_all_clients(Client *clients, Client sender, int actual, const string buffer, char from_server) {
    int i = 0;
    string message;
    message[0] = 0;
    for (i = 0; i < actual; i++)
        if (sender.sock != clients[i].sock) {
            if (from_server == 0)
                message = sender.login + " : ";
            message += buffer;
            write_client(clients[i].sock, message);
        }

    cout << "Broadcasted \"" << message << "\" to all" << endl;
}

void
send_message_to_client(Client *clients, Client sender, Client receiver, int actual, std::string buffer) {
    int i = 0;
    string message;
    for (i = 0; i < actual; i++) {
        if (receiver.login == clients[i].login) {
            message += buffer;
            write_client(clients[i].sock, message);

            cout << "Sent " << message << " to " << clients[i].login << endl;
        }
    }

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

void command_send(Client clients[], int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "Destination ? ");
    string buffer = read_client(clients[k].sock);
    if (buffer.length()) {
        Client dest = {0};
        int j;
        for (j = 0; j < actual; j++) {
            if (clients[j].login == buffer) {
                dest = clients[j];
                break;
            } else
                dest.login = "NULL";
        }
        if (dest.login != "NULL") {
            send_message_to_client(clients, clients[k], clients[k], actual, "Message ? ");
            buffer = read_client(clients[k].sock);
            if (buffer.length())
                send_message_to_client(clients, clients[k], dest, actual, buffer);
        } else
            send_message_to_client(clients, clients[k], clients[k], actual, "No client found with this pseudo.");
    }
}

/// Protocol

void p_BPING(Client clients[], int k, int actual) {
    p_BPONG(clients, k, actual);
}

void p_BPONG(Client clients[], int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BPONG");
}

void p_BAUTH(Client clients[], int k, int actual, string buffer) {
    string login, pwd;
    vector<string> v = split(buffer, ',');
    if (v.size() < 3) {
        send_message_to_client(clients, clients[k], clients[k], actual, "T'es qu'une grosse merde");
        return;
    }
    login = v[1];
    pwd = v[2];

    p_BLOGIN(clients, k, actual, login, pwd);
}

void p_BLOGIN(Client clients[], int k, int actual, string login, string pwd) {
    string db_pwd, sql;

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM player_account WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.size() != 1) {
            send_message_to_client(clients, clients[k], clients[k], actual, "BLOGIN,2");
            return;
        }

        for (result::const_iterator c = r.begin(); c != r.end(); ++c)
            db_pwd = c[1].as<string>();

        if (db_pwd != pwd) {
            send_message_to_client(clients, clients[k], clients[k], actual, "BLOGIN,1");
            return;
        }

        send_message_to_client(clients, clients[k], clients[k], actual, "BLOGIN,0");
        clients[k].is_auth = true;
        clients[k].login = login;

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BCLASSESR(Client clients[], int k, int actual, string buffer) {
    vector<string> a = split(buffer, ',');

    p_BCLASSESA(clients, k, actual, a[1]);
}

void p_BCLASSESA(Client clients[], int k, int actual, string login) {
    string sql, classes = "BCLASSESA";

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM has_class WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.size() < 1) {
            send_message_to_client(clients, clients[k], clients[k], actual, "BCLASSESA,NULL");
            return;
        }

        for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
            classes += "," + c[0].as<string>();
        }

        send_message_to_client(clients, clients[k], clients[k], actual, classes);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BSPELLSR(Client *clients, int k, int actual, string buffer) {
    // TODO Interroger la BD pour avoir les sorts disponibles pour un login
    vector<string> a;
    if (split(buffer, ',').size() < 2)
        return;
    p_BSPELLSA(clients, k, actual, a[1]);
}

void p_BSPELLSA(Client *clients, int k, int actual, std::string login) {
    string sql, spells = "BSPELLSA";

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM spell WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.size() < 1) {
            send_message_to_client(clients, clients[k], clients[k], actual, "BSPELLSA,NULL");
            return;
        }

        for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
            for (int i = 0; i < c.size(); i++)
                spells += "," + c[i].as<string>();
        }

        send_message_to_client(clients, clients[k], clients[k], actual, spells);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BSPELLSC(Client *clients, int k, int actual, std::string chosenList) {
    vector<string> cl = split(chosenList, ',');
    for (const string &spell : cl) {
        auto s = Spell(spell);
        if (s.getSpellName() != "")
            s.print();
    }

    p_BSPELLSACK(clients, k, actual);
}

void p_BSPELLSACK(Client *clients, int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BSPELLSACK");
}

void p_BMAKE(Client *clients, int k, int actual) {
    p_BWAIT(clients, k, actual);
}

void p_BWAIT(Client *clients, int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BWAIT");
}

void p_BMATCH(Client *clients, int k, int actual, Player p1, Player p2) {
    send_message_to_client(clients, clients[k], clients[k], actual, p1.toString() + "," + p2.toString());
}

void p_BFIGHT(Client *clients, int k, int actual, std::string buffer) {
    vector<string> spell = split(buffer, ',');
    if (spell.size() > 1)
        cout << "Used spell" << spell[1] << endl;
}

void p_BREF(Client *clients, int k, int actual, Player p1, Player p2) {
    send_message_to_client(clients, clients[k], clients[k], actual, p1.toString() + "," + p2.toString());
}

void p_BWIN(Client *clients, int k, int actual) {
    // TODO Calculer le nouvel elo du joueur et lui envoyer
    send_message_to_client(clients, clients[k], clients[k], actual, "BWIN");
}

void p_BLOSE(Client *clients, int k, int actual) {
    // TODO Calculer le nouvel elo du joueur et lui envoyer
    send_message_to_client(clients, clients[k], clients[k], actual, "BLOSE");
}

void p_BLOGOUT(Client *clients, int k, int actual) {
    // TODO Gérer la déconnexion du client
    p_BBYE(clients, k, actual);
}

void p_BBYE(Client *clients, int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BBYE");
}

void p_BTIMEDOUT(Client *clients, int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BBYE");
}

void p_BFALL(Client *clients, int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "LOL T NUL");
}
