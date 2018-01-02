#include <iostream>
#include <cstring>
#include "includes/server.h"
#include "includes/util.h"
#include "includes/clientio.h"

using namespace std;

vector<MMStruct> mmVector(1);
int mmi = 0;

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
		} else {
			int k = 0;
			for (k = 0; k < actual; k++) {
				if (FD_ISSET(clients[k].sock, &rdfs)) { // A client is talking
					Client client = clients[k];
					buffer = read_client(client.sock);
					if (buffer.empty()) {                       // A client disconnected
						cout << "Client on socket " << client.sock << " disconnected" << endl;
						closesocket(client.sock);
						remove_client(clients, k, &actual);
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
	int enable = 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);  // Initializing the socket : AF_INET = IPv4, SOCK_STREAM : TCP
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR");
		exit(errno);
	}

	SOCKADDR_IN sin = {0, 0, 0, 0};

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
	cout << "Server started... Listening on port " << PORT << "..." << endl;
	run();
	/*Spell s = Spell("quitte ou double");
	s.print();*/
	return EXIT_SUCCESS;
}
