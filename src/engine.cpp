#include "../includes/engine.h"

using namespace std;

extern mutex stats_mutex;
extern int bport;
extern int qport;
extern Stats qStats;

void clear_clients(Client *clients, int actual) {
	int i = 0;
	for (i = 0; i < actual; i++)
		closesocket(clients[i].sock);
}

void clear_clients(QClient *clients, int actual) {
	int i = 0;
	for (i = 0; i < actual; i++)
		closesocket(clients[i].sock);
}

void remove_client(Client *clients, int to_remove, int *actual) {
	memmove(clients + to_remove, clients + to_remove + 1,
	        (*actual - to_remove - 1) * sizeof(Client));  // We remove the client from the array
	(*actual)--;    // Reducing the number of clients
	stats_mutex.lock();
	qStats.onlinePlayers--;
	stats_mutex.unlock();
}

void remove_qclient(QClient *clients, int to_remove, int *actual) {
	memmove(clients + to_remove, clients + to_remove + 1,
	        (*actual - to_remove - 1) * sizeof(Client));  // We remove the client from the array
	(*actual)--;    // Reducing the number of clients
	stats_mutex.lock();
	qStats.onlineQueries--;
	stats_mutex.unlock();
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
	sin.sin_port = htons((uint16_t) bport);
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

int init_query_connection() {
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
	sin.sin_port = htons((uint16_t) qport);
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