#include <iostream>
#include <algorithm>
#include "includes/clientio.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace pqxx;

extern vector<MMStruct> mmVector;
extern int mmi;

void send_message_to_client(Client client, string buffer) {
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
	if (v.size() < 3) {
		send_message_to_client(*client, "T'es qu'une grosse merde");
		return;
	}
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

void p_BCLASSESR(Client *client, string buffer) {
	vector<string> a = split(buffer, ',');

	p_BCLASSESA(client, a[1]);
}

void p_BCLASSESA(Client *client, string login) {
	string sql, classes = "BCLASSESA";

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

void p_BSPELLSR(Client *client, string buffer) {
	vector<string> className = split(buffer, ',');
	if (className.size() < 2)
		return;
	p_BSPELLSA(client, className[1]);
	client->selectedClass = className[1];
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
	p_BSPELLSACK(*client);
}

void p_BSPELLSACK(Client client) {
	send_message_to_client(client, "BSPELLSACK");
}

void p_BMAKE(Client *client) {
	if (mmVector.back().c1 != nullptr && mmVector.back().c2 == nullptr) {    // Si la dernière MMStruct a une place libre
		mmVector.back().c2 = client;        // On remplit cette place avec le client qui cherche une partie
		client->mmIndex = mmVector.back().index;
		p_BMATCH(*(mmVector.back().c1), *(mmVector.back().c2));
	} else {
		MMStruct ms = {client, nullptr, ++mmi}; // Sinon, on crée une nouvelle MMStruct et on y ajoute le client
		mmVector.push_back(ms);                 // On l'ajoute au vector
		client->mmIndex = mmVector.back().index;
		p_BWAIT(*client);                    // Et on envoie un BWAIT au client
	}
}

void p_BWAIT(Client client) {
	send_message_to_client(client, "BWAIT");
}

void p_BMATCH(Client p1, Client p2) {
	send_message_to_client(p1, "BMATCH," + p1.combatInfos.toString() + "," + p2.combatInfos.toString());
	send_message_to_client(p2, "BMATCH," + p2.combatInfos.toString() + "," + p1.combatInfos.toString());
}

void p_BFIGHT(Client client, string buffer) {
	vector<string> buf = split(buffer, ',');
	if (buf.size() > 1) {
		string spell = buf[1];
		cout << "Used spell : " << Spell(spell).toString() << endl;
	}
} // TODO Revoir les arguments de la fonction

void p_BREF(Client client, Client p1, Client p2) {
	send_message_to_client(client, p1.toString() + "," + p2.toString());
}

void p_BWIN(Client client) {
	send_message_to_client(client, "BWIN");
} // TODO Calculer le nouvel elo du joueur et lui envoyer

void p_BLOSE(Client client) {
	send_message_to_client(client, "BLOSE");
} // TODO Calculer le nouvel elo du joueur et lui envoyer

void p_BLOGOUT(Client *clients, int k, int *actual) {
	p_BBYE(clients, k, actual);
}

void p_BBYE(Client *clients, int k, int *actual) {
	send_message_to_client(clients[k], "BBYE");
	clients[k].is_auth = false;
	remove_client(clients, k, actual);
}

void p_BTIMEDOUT(Client client) {
	send_message_to_client(client, "BBYE");
}

void p_BFALL(Client client) {
	send_message_to_client(client, "LOL T NUL");
} // TODO Ajouter une fonction permettant de faire perdre un PV au joueur
