#include <iostream>
#include "includes/clientio.h"

using namespace std;

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
send_message_to_client(Client clients[MAX_CLIENTS], Client sender, Client receiver, int actual, const string buffer,
                       char from_server) {
    int i = 0;
    string message;
    for (i = 0; i < actual; i++) {
        if (receiver.login == clients[i].login) {
            if (!from_server) {
                message = "[" + sender.login + "]";
            } else {
                message = "[SERVER]";
            }
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
    send_message_to_client(clients, clients[k], clients[k], actual, "Destination ? ", 1);
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
            send_message_to_client(clients, clients[k], clients[k], actual, "Message ? ", 1);
            buffer = read_client(clients[k].sock);
            if (buffer.length())
                send_message_to_client(clients, clients[k], dest, actual, buffer, 0);
        } else
            send_message_to_client(clients, clients[k], clients[k], actual,
                                   "No client found with this pseudo.", 1);
    }
}

void p_BPING(Client clients[], int k, int actual) {
    p_BPONG(clients, k, actual);
}

void p_BPONG(Client clients[], int k, int actual) {
    send_message_to_client(clients, clients[k], clients[k], actual, "BPONG", 1);
}

void p_BAUTH(Client clients[], int k, int actual, string buffer) {
    string login, pwd;
    vector<string> v = split(buffer, ',');
    if (v.size() < 3) {
        send_message_to_client(clients, clients[k], clients[k], actual,
                               "T'es qu'une grosse merde", 1);
        return;
    }
    login = v[1];
    pwd = v[2];

    p_BLOGIN(clients, k, actual, login, pwd);
}

void p_BLOGIN(Client clients[], int k, int actual, string login, string pwd) {
    send_message_to_client(clients, clients[k], clients[k], actual,
                           "BLOGIN,Received login : " + login + ", received pwd : " + pwd, 1);
    // TODO Interroger la BD avec le login et le mdp et envoyer au client un entier en fonction de la réussite ou non de la connexion
}

void p_BCLASSESR(Client clients[], int k, int actual, string pseudo) {
    // TODO Interroger la BD pour avoir la liste des classes disponibles pour un pseudo donné
    vector<Spell> classList;
    p_BCLASSESA(clients, k, actual, classList);
}

void p_BCLASSESA(Client clients[], int k, int actual,
                 vector<Spell> classList) {    // TODO Remplacer vector<Spell> par vector<Class>
    send_message_to_client(clients, clients[k], clients[k], actual, "BCLASSESA,Classes disponibles :", 1);
}