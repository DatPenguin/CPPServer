#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <mutex>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
//typedef struct in_addr IN_ADDR;

#define DEBUG 0

//#define PORT        5000
#define MAX_CLIENTS    100

#define BUF_SIZE    1024

#include "brickest.h"
#include "QClient.h"
#include "Stats.h"

void run();

int init_connection();

int init_query_connection();

void end_connection(int sock);

void remove_client(Client *clients, int to_remove, int *actual);

void remove_qclient(QClient *clients, int to_remove, int *actual);

void clear_clients(Client *clients, int actual);

void clear_clients(QClient *clients, int actual);

#endif /* guard */
