#include <iostream>
#include "server.h"

using namespace std;

void
send_message_to_all_clients(Client *clients, Client sender, int actual, const string buffer, char from_server) {
    int i = 0;
    string message;
    message[0] = 0;
    for (i = 0; i < actual; i++)
        if (sender.sock != clients[i].sock) {
            if (from_server == 0)
                message = sender.name + " : ";
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
        if (receiver.name == clients[i].name) {
            if (!from_server) {
                message = "[" + sender.name + "]";
            } else {
                message = "[SERVER]";
            }
            message += buffer;
            write_client(clients[i].sock, message);

            cout << "Sent " << message << " to " << clients[i].name << endl;
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