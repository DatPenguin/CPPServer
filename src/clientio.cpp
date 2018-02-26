#include <iostream>
#include <algorithm>
#include "../includes/clientio.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace pqxx;

extern vector<MMStruct> mmVector;
extern int mmi;

void send_message_to_client(Client client, string buffer) {
    write_client(client.sock, buffer);
    cout << "Sent " << buffer << " to socket " << client.sock << endl;

}

void send_message_to_client(QClient client, string buffer) {
    write_client(client.sock, buffer);
    cout << "Sent " << buffer << " to socket " << client.sock << endl;

}

void send_message_to_client(Client *client, string buffer) {
    write_client(client->sock, buffer);
    cout << "Sent " << buffer << " to socket " << client->sock << endl;

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
    string out = buf, out1;

    if (out.empty())
        return "";
    out1 = out;
    cout << "Received " << out << " from socket " << sock << endl;
    boost::erase_all(out, "'");
    boost::erase_all(out, "\"");
    if (out1 != out)
        cout << out1 << endl << out << endl;
    return out;
}

/// Protocol

void p_BPING(Client client) {
    p_BPONG(client);
}

void p_BPONG(Client client) {
    send_message_to_client(client, "BPONG");
}

void p_BAUTH(Client *client, string buffer) {
    string login, pwd;
    vector<string> v = split(buffer, ',');
    if (v.size() < 3)
        return;
    login = v[1];
    pwd = v[2];

    p_BLOGIN(client, login, pwd);
}

void p_BLOGIN(Client *client, string login, string pwd) {
    string db_pwd, sql;

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM player_account WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.size() != 1) {
            send_message_to_client(*client, "BLOGIN,2");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci)
            db_pwd = ci[1].as<string>();

        if (db_pwd != pwd) {
            send_message_to_client(*client, "BLOGIN,1");
            return;
        }

        send_message_to_client(*client, "BLOGIN,0");
        client->is_auth = true;
        client->login = login;

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BCLASSESR(Client *client) {
    if (!client->is_auth)
        return;
    p_BCLASSESA(client);
}

void p_BCLASSESA(Client *client) {
    string sql, classes = "BCLASSESA", login = client->login;

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT * FROM has_class WHERE login LIKE '" + login + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.empty()) {
            send_message_to_client(*client, "BCLASSESA,NULL");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci) {
            classes += "," + ci[0].as<string>();
            client->heroList.push_back(ci[0].as<string>());
        }
        send_message_to_client(*client, classes);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BCLASSESC(Client *client, std::string buffer) {
    vector<string> cl = split(buffer, ',');
    if (cl.size() > 1) {
        client->selectedClass = cl[1];
        p_BCLASSESACK(*client);
    }
}

void p_BCLASSESACK(Client client) {
    send_message_to_client(client, "BCLASSESACK");
}

void p_BSPELLSR(Client *client) {
    p_BSPELLSA(client, client->selectedClass);
}

void p_BSPELLSA(Client *client, string selectedClass) {
    string sql, spells = "BSPELLSA";

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "SELECT spell_name FROM applicable WHERE c_type LIKE '" + selectedClass + "'";
        nontransaction n(c);
        result r(n.exec(sql));

        if (r.empty()) {
            send_message_to_client(client, "BSPELLSA,NULL");
            return;
        }

        for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci) {
            for (unsigned int i = 0; i < ci.size(); i++)
                spells += "," + ci[i].as<string>();
        }

        send_message_to_client(client, spells);

    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
}

void p_BSPELLSC(Client *client, string chosenList) {
    vector<string> cl = split(chosenList, ',');
    client->spellsList.clear();
    for (const string &spell : cl) {
        auto s = Spell(spell);
        if (!s.getSpellName().empty())
            client->spellsList.push_back(s);
    }
    if (!client->spellsList.empty())
        p_BSPELLSACK(*client);
}

void p_BSPELLSACK(Client client) {
    send_message_to_client(client, "BSPELLSACK");
}

void p_BMAKE(Client *client) {
    if (mmVector.back().c1 != nullptr && mmVector.back().c2 == nullptr &&
        mmVector.back().c1->sock != client->sock) {    // Si la dernière MMStruct a une place libre
        mmVector.back().c2 = client;        // On remplit cette place avec le client qui cherche une partie
        client->mmIndex = mmVector.back().index;
        p_BMATCH(*(mmVector.back().c1), *(mmVector.back().c2));
    } else if (client->mmIndex == -1) {
        MMStruct ms = {client, nullptr, ++mmi}; // Sinon, on crée une nouvelle MMStruct et on y ajoute le client
        mmVector.push_back(ms);                 // On l'ajoute au vector
        client->mmIndex = mmVector.back().index;
        p_BWAIT(*client);                    // Et on envoie un BWAIT au client
    } else if (mmVector[client->mmIndex].c1 != nullptr && mmVector[client->mmIndex].c2 != nullptr)
        return;
    else
        p_BWAIT(*client);
}

void p_BUNMAKE(Client *client) {
    if (mmVector.back().c1 == client) {
        mmVector.pop_back();
        mmi--;
    }
}

void p_BWAIT(Client client) {
    send_message_to_client(client, "BWAIT");
}

void p_BMATCH(Client p1, Client p2) {
    send_message_to_client(p1, "BMATCH," + p1.combatInfos.toString() + ";" + p1.selectedClass + ";" + p1.login + "," +
                               p2.combatInfos.toString() + ";" + p2.selectedClass + ";" + p2.login);
    send_message_to_client(p2, "BMATCH," + p2.combatInfos.toString() + ";" + p2.selectedClass + ";" + p2.login + "," +
                               p1.combatInfos.toString() + ";" + p1.selectedClass + ";" + p1.login);
}

void p_BFIGHT(Client client, string buffer) {
    if (!client.isInMatchMaking())
        return;

    vector<string> buf = split(buffer, ',');
    MMStruct *ms = &(mmVector[client.mmIndex]);
    if (buf.size() <= 1)
        return;
    Spell s(buf[1]);
    if (s.isBadSpell())
        return;
    cout << "Used spell : " << s.toString() << endl;

    if (ms->c1->sock == client.sock) {
        ms->c1->combatInfos.applySpell(s);
        ms->c2->combatInfos.applyOpponentsSpell(s);
    } else {
        ms->c2->combatInfos.applySpell(s);
        ms->c1->combatInfos.applyOpponentsSpell(s);
    }

    if (ms->c1->combatInfos.isDead()) {
        p_BWIN(ms->c2);
        p_BLOSE(ms->c1);
        mmVector.erase(mmVector.begin() + ms->index);

        ms->c1->combatInfos.resetFighter();
        ms->c2->combatInfos.resetFighter();

        ms->c1->mmIndex = -1;
        ms->c2->mmIndex = -1;

        mmi--;

    } else if (ms->c2->combatInfos.isDead()) {
        p_BWIN(ms->c1);
        p_BLOSE(ms->c2);
        mmVector.erase(mmVector.begin() + ms->index);

        ms->c1->combatInfos.resetFighter();
        ms->c2->combatInfos.resetFighter();

        ms->c1->mmIndex = -1;
        ms->c2->mmIndex = -1;

        mmi--;
    } else
        p_BREF(*ms->c1, *ms->c2);
}

void p_BREF(Client p1, Client p2) {
    send_message_to_client(p1, "BREF," + p1.combatInfos.toString() + "," + p2.combatInfos.toString());
    send_message_to_client(p2, "BREF," + p2.combatInfos.toString() + "," + p1.combatInfos.toString());
}

void p_BWIN(Client *client) {
    //send_message_to_client(*client, "BWIN," + ++(client->elo));
    send_message_to_client(*client, "BWIN");
}   // TODO Lorsque l'elo sera fait, décommenter la ligne

void p_BLOSE(Client *client) {
    //send_message_to_client(*client, "BLOSE," + --(client->elo));
    send_message_to_client(*client, "BLOSE");
}

void p_BLOGOUT(Client *clients, int k, int *actual) {
    p_BBYE(clients, k, actual);
}

void p_BBYE(Client *clients, int k, int *actual) {
    send_message_to_client(clients[k], "BBYE");
    clients[k].is_auth = false;
    remove_client(clients, k, actual);
}

void p_BFALL(Client *client) {
    if (!client->isInMatchMaking())
        return;

    client->combatInfos.fallen();

    MMStruct *ms = &mmVector[client->mmIndex];

    if (ms->c1->combatInfos.isDead()) {
        p_BWIN(ms->c2);
        p_BLOSE(ms->c1);
        mmVector.erase(mmVector.begin() + ms->index);

        ms->c1->combatInfos.resetFighter();
        ms->c2->combatInfos.resetFighter();

        ms->c1->mmIndex = -1;
        ms->c2->mmIndex = -1;

    } else if (ms->c2->combatInfos.isDead()) {
        p_BWIN(ms->c1);
        p_BLOSE(ms->c2);
        mmVector.erase(mmVector.begin() + ms->index);

        ms->c1->combatInfos.resetFighter();
        ms->c2->combatInfos.resetFighter();

        ms->c1->mmIndex = -1;
        ms->c2->mmIndex = -1;
    } else
        p_BREF(*ms->c1, *ms->c2);
}

void p_BNAME(Client *client, string buffer) {
    if (!client->is_auth) {
        cerr << "BNAME : Client not AUTH" << endl;
        send_message_to_client(*client, "NOT AUTH");
        return;
    }

    vector<string> buf = split(buffer, ',');
    if (buf.size() <= 1)
        return;

    string sql, name = buf[1];

    try {
        connection c(CONNECTION_STRING);
        if (!c.is_open())
            cerr << "Can't open the database" << endl;

        sql = "UPDATE player_info SET p_name='" + name + "' WHERE login LIKE '" + client->login + "'";
        nontransaction n(c);
        n.exec(sql);

    } catch (const exception &e) {
        cerr << e.what() << endl;
        return;
    }

    p_BNAMEACK(*client);
}

void p_BNAMEACK(Client client) {
    send_message_to_client(client, "BNAMEACK");
}

void client_disconnected(Client *clients, int k, int *actual) {
    MMStruct *ms = &mmVector[clients[k].mmIndex];

    if (clients[k].isInMatchMaking()) {
        if (ms->c1 != nullptr && ms->c2 != nullptr) {   // Si le combat est en cours
            if (ms->c1->sock == clients[k].sock)        // On fait perdre le client qui s'est déconnecté
                p_BWIN(ms->c2);
            else
                p_BWIN(ms->c1);
        } else if (ms->c1->sock == clients[k].sock)   // Si le client est en file d'attente, on l'enlève du matchmaking
            p_BUNMAKE(&clients[k]);
    }
    closesocket(clients[k].sock);
    remove_client(clients, k, actual);
}

void qclient_disconnected(QClient *clients, int k, int *actual) {
    closesocket(clients[k].sock);
    remove_qclient(clients, k, actual);
}