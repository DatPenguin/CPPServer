#include <iostream>
#include <cstring>
#include "server.h"

using namespace std;

void run() {
    SOCKET sock = init_connection();
    string buffer;
    int actual = 0;                 // Array index
    int max = sock;
    Client clients[MAX_CLIENTS];    // An array for all clients

    fd_set rdfs{};

    while (true) {
        int i = 0;

        // SELECT Params
        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(sock, &rdfs);
        for (i = 0; i < actual; i++)
            FD_SET(clients[i].sock, &rdfs); // Adding each client's socket

        if (select(max + 1, &rdfs, nullptr, nullptr, nullptr) == -1) {
            perror("select()");
            exit(errno);
        }

        if (FD_ISSET(STDIN_FILENO, &rdfs))  // If something happens on standard input
            break; // Stop process when typing on keyboard
        else if (FD_ISSET(sock, &rdfs)) {   // New connection
            SOCKADDR_IN csin = {0};
            size_t sinsize = sizeof(csin);
            int csock = accept(sock, (SOCKADDR *) &csin, (socklen_t *) &sinsize);
            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }

            if ((buffer = read_client(csock)) == "")   // After connecting, the client sends its name
                continue; // Disconnects

            max = csock > max ? csock : max;    // returns the new max fd

            FD_SET(csock, &rdfs);

            Client c;
            c.sock = csock;
            c.name = buffer;
            clients[actual] = c;
            actual++;
        } else {
            int k = 0;
            for (k = 0; k < actual; k++) {
                if (FD_ISSET(clients[k].sock, &rdfs)) { // A client is talking
                    Client client = clients[k];
                    buffer = read_client(clients[k].sock);
                    if (buffer.empty()) {                       // A client disconnected
                        closesocket(clients[k].sock);
                        remove_client(clients, k, &actual);
                        buffer = client.name;
                        buffer += " disconnected !";
                        send_message_to_all_clients(clients, client, actual, buffer, 1);
                    } else if (buffer == "send") {
                        send_message_to_client(clients, clients[k], clients[k], actual, "Destination ? ", 1);
                        buffer = read_client(clients[k].sock);
                        if (buffer.length()) {
                            Client dest = {0};
                            int j;
                            for (j = 0; j < actual; j++) {
                                if (clients[j].name == buffer) {
                                    dest = clients[j];
                                    break;
                                } else
                                    dest.name = "NULL";
                            }
                            if (dest.name != "NULL") {
                                send_message_to_client(clients, clients[k], clients[k], actual, "Message ? ", 1);
                                buffer = read_client(clients[k].sock);
                                if (buffer.length())
                                    send_message_to_client(clients, clients[k], dest, actual, buffer, 0);
                            } else
                                send_message_to_client(clients, clients[k], clients[k], actual,
                                                       "No client found with this pseudo.", 1);
                        }
                    } else
                        send_message_to_all_clients(clients, client, actual, buffer, 0);
                    break;
                }
            }
        }
    }

    clear_clients(clients, actual);
    end_connection(sock);
}

void clear_clients(Client *clients, int actual) {
    int i = 0;
    for (i = 0; i < actual; i++)
        closesocket(clients[i].sock);
}

void remove_client(Client *clients, int to_remove, int *actual) {
    memmove(clients + to_remove, clients + to_remove + 1,
            (*actual - to_remove - 1) * sizeof(Client));  // We remove the client from the array
    (*actual)--;    // Reducing the number of clients
}

int init_connection() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);  // Initializing the socket : AF_INET = IPv4, SOCK_STREAM : TCP
    SOCKADDR_IN sin = {0};

    if (sock == INVALID_SOCKET) {   // if socket() returned -1
        perror("socket()");
        exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if (bind(sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR) {
        perror("bind()");
        exit(errno);
    }

    if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
        perror("listen()");
        exit(errno);
    }

    return sock;
}

void end_connection(int sock) {
    closesocket(sock);
}

int main() {
    run();
    return EXIT_SUCCESS;
}
