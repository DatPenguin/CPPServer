#ifndef CPPSERVER_CLIENTIO_H
#define CPPSERVER_CLIENTIO_H

#include "server.h"
#include "util.h"

std::string read_client(SOCKET sock);

void write_client(SOCKET sock, std::string buffer);

void
send_message_to_all_clients(Client *clients, Client client, int actual, std::string buffer, char from_server);

void
send_message_to_client(Client *clients, Client sender, Client receiver, int actual, std::string buffer,
                       char from_server);

void command_send(Client clients[], int k, int actual);

void p_BPING(Client clients[], int k, int actual);

void p_BPONG(Client clients[], int k, int actual);

void p_BAUTH(Client clients[], int k, int actual, std::string buffer);

void p_BLOGIN(Client clients[], int k, int actual, std::string login, std::string pwd);

void p_BCLASSESR(Client clients[], int k, int actual, std::string pseudo);

void p_BCLASSESA(Client clients[], int k, int actual, std::vector<Spell> classList);

#endif //CPPSERVER_CLIENTIO_H
