#include <iostream>
#include <thread>
#include <mutex>
#include "../includes/engine.h"
#include "../includes/util.h"
#include "../includes/clientio.h"
#include "../includes/Stats.h"

using namespace std;

mutex stats_mutex;
mutex db_mutex;

vector<MMStruct> mmVector(1);
int mmi = 0;
int bport = 5000;
int qport = 4000;

Stats qStats;

void run() {
    cout << "Main Server started... Listening on port " << bport << endl;  // Prints the listening port
    SOCKET sock = init_connection();
    string buffer;
    int actual = 0;                 // Index of the last client in the array
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
            SOCKADDR_IN csin = {0, 0, 0, 0};
            size_t sinsize = sizeof(csin);
            int csock = accept(sock, (SOCKADDR *) &csin, (socklen_t *) &sinsize);
            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }

            max = csock > max ? csock : max;    // returns the new max fd

            FD_SET(csock, &rdfs);

            Client c;
            c.sock = csock;
            c.login = buffer;
            clients[actual] = c;
            actual++;
            stats_mutex.lock();
            qStats.onlinePlayers++;
            qStats.totalPlayers++;
            stats_mutex.unlock();
        } else {
            int k = 0;
            for (k = 0; k < actual; k++) {
                if (FD_ISSET(clients[k].sock, &rdfs)) { // A client is talking
                    Client client = clients[k];
                    buffer = read_client(client.sock);
                    if (buffer[buffer.length() - 1] == '\n')
                        buffer[buffer.length() - 1] = '\0';
                    if (buffer.empty()) {                       // A client disconnected
                        cout << "Client on socket " << client.sock << " disconnected" << endl;
                        client_disconnected(clients, k, &actual);
                    } else if (startsWith(buffer, "BAUTH")) {
                        p_BAUTH(&clients[k], buffer);
                    } else if (startsWith(buffer, "BPING")) {
                        p_BPING(clients[k]);
                    } else if (startsWith(buffer, "BCLASSESR")) {
                        p_BCLASSESR(&clients[k]);
                    } else if (startsWith(buffer, "BCLASSESC")) {
                        p_BCLASSESC(&clients[k], buffer);
                    } else if (startsWith(buffer, "BSPELLSR")) {
                        p_BSPELLSR(&clients[k]);
                    } else if (startsWith(buffer, "BSPELLSC")) {
                        p_BSPELLSC(&clients[k], buffer);
                    } else if (startsWith(buffer, "BMAKE")) {
                        p_BMAKE(&clients[k]);
                    } else if (startsWith(buffer, "BUNMAKE")) {
                        p_BUNMAKE(&clients[k]);
                    } else if (startsWith(buffer, "BFIGHT")) {
                        p_BFIGHT(clients[k], buffer);
                    } else if (startsWith(buffer, "BLOGOUT")) {
                        p_BLOGOUT(clients, k, &actual);
                    } else if (startsWith(buffer, "BFALL")) {
                        p_BFALL(&clients[k]);
                    } else if (buffer == "close") {
                        clear_clients(clients, actual);
                        end_connection(sock);
                        return;
                    } else if (startsWith(buffer, "BNAME")) {
                        p_BNAME(&clients[k], buffer);
                    } else if (buffer == "AUTH?") {
                        if (clients[k].is_auth)
                            send_message_to_client(clients[k], "AUTH," + clients[k].login);
                        else
                            send_message_to_client(clients[k], "NOT AUTH");
                    }
                    break;
                }
            }
        }
    }

    clear_clients(clients, actual);
    end_connection(sock);
}

void query() {
    cout << "Query Server started... Listening on port " << qport << endl;
    SOCKET sock = init_query_connection();
    string buffer;
    int actual = 0;                 // Index of the last client in the array
    int max = sock;
    QClient clients[MAX_CLIENTS];    // An array for all clients

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
            SOCKADDR_IN csin = {0, 0, 0, 0};
            size_t sinsize = sizeof(csin);
            int csock = accept(sock, (SOCKADDR *) &csin, (socklen_t *) &sinsize);
            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }

            max = csock > max ? csock : max;    // returns the new max fd

            FD_SET(csock, &rdfs);

            QClient c;
            c.sock = csock;
            clients[actual] = c;
            actual++;
            stats_mutex.lock();
            qStats.onlineQueries++;
            stats_mutex.unlock();
        } else {
            int k = 0;
            for (k = 0; k < actual; k++) {
                if (FD_ISSET(clients[k].sock, &rdfs)) { // A client is talking
                    QClient client = clients[k];
                    buffer = read_client(client.sock);
                    if (buffer[buffer.length() - 1] == '\n')
                        buffer[buffer.length() - 1] = '\0';
                    if (buffer.empty()) {                       // A client disconnected
                        cout << "QClient on socket " << client.sock << " disconnected" << endl;
                        qclient_disconnected(clients, k, &actual);
                    } else if (startsWith(buffer, "bonjour")) {
                        send_message_to_client(client, "cc sava ?");
                    } else if (buffer == "close") {
                        clear_clients(clients, actual);
                        end_connection(sock);
                        return;
                    }
                    break;
                }
            }
        }
    }

    clear_clients(clients, actual);
    end_connection(sock);
}

int main(int argc, char **argv) {
    if (argc > 1)   // If an argument is passed
        bport = (int) strtol(argv[1], nullptr, 10);    // The number given as an argument becomes the listening port of the main server
    thread qThread(query);   // Creates the thread for the query server and starts it
    run();  // Starts the main server
    qThread.join();  // Joins the query thread so the program doesn't stop while at least one of the servers is running
    return EXIT_SUCCESS;
}
