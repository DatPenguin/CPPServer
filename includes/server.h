#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
//typedef struct in_addr IN_ADDR;

#define PORT        5000
#define MAX_CLIENTS    100

#define BUF_SIZE    1024

#include "brickest.h"

void run();

int init_connection();

void end_connection(int sock);

std::string read_client(SOCKET sock);

void write_client(SOCKET sock, std::string buffer);

void
send_message_to_all_clients(Client *clients, Client client, int actual, std::string buffer, char from_server);

void
send_message_to_client(Client *clients, Client sender, Client receiver, int actual, std::string buffer,
                       char from_server);

void remove_client(Client *clients, int to_remove, int *actual);

void clear_clients(Client *clients, int actual);

void db_connect();

#endif /* guard */