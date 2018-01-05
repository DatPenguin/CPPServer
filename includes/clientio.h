#ifndef CPPSERVER_CLIENTIO_H
#define CPPSERVER_CLIENTIO_H

#include "server.h"
#include "util.h"
#include "Client.h"
#include <pqxx/pqxx>

std::string read_client(SOCKET sock);

void write_client(SOCKET sock, std::string buffer);

void send_message_to_client(Client client, std::string buffer);

void send_message_to_client(Client *client, std::string buffer);

void p_BPING(Client client);

void p_BPONG(Client client);

void p_BAUTH(Client *client, std::string buffer);

void p_BLOGIN(Client *client, std::string login, std::string pwd);

void p_BCLASSESR(Client *client);

void p_BCLASSESA(Client *client);

void p_BCLASSESC(Client *client, std::string buffer);

void p_BCLASSESACK(Client client);

void p_BSPELLSR(Client *client);

void p_BSPELLSA(Client *client, std::string selectedClass);

void p_BSPELLSC(Client *client, std::string chosenList);

void p_BSPELLSACK(Client client);

void p_BMAKE(Client *client);

void p_BUNMAKE(Client *client);

void p_BWAIT(Client client);

void p_BMATCH(Client p1, Client p2);

void p_BFIGHT(Client client, std::string buffer);

void p_BREF(Client p1, Client p2);

void p_BWIN(Client *client);

void p_BLOSE(Client *client);

void p_BLOGOUT(Client *clients, int k, int *actual);

void p_BBYE(Client *clients, int k, int *actual);

void p_BTIMEDOUT(Client client);

void p_BFALL(Client *client);

#endif //CPPSERVER_CLIENTIO_H
