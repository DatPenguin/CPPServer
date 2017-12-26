#ifndef CPPSERVER_CLIENTIO_H
#define CPPSERVER_CLIENTIO_H

#include "server.h"
#include "util.h"
#include "Player.h"
#include <pqxx/pqxx>

std::string read_client(SOCKET sock);

void write_client(SOCKET sock, std::string buffer);

void
send_message_to_all_clients(Client *clients, Client client, int actual, std::string buffer, char from_server);

void
send_message_to_client(Client *clients, Client sender, Client receiver, int actual, std::string buffer);

void command_send(Client clients[], int k, int actual);

void p_BPING(Client clients[], int k, int actual);

void p_BPONG(Client clients[], int k, int actual);

void p_BAUTH(Client clients[], int k, int actual, std::string buffer);

void p_BLOGIN(Client clients[], int k, int actual, std::string login, std::string pwd);

void p_BCLASSESR(Client clients[], int k, int actual, std::string pseudo);

void p_BCLASSESA(Client clients[], int k, int actual, std::string login);

void p_BSPELLSR(Client clients[], int k, int actual, std::string className);

void p_BSPELLSA(Client clients[], int k, int actual, std::string login);

void p_BSPELLSC(Client clients[], int k, int actual, std::string chosenList);

void p_BSPELLSACK(Client clients[], int k, int actual);

void p_BMAKE(Client clients[], int k, int actual);

void p_BWAIT(Client clients[], int k, int actual);

void p_BMATCH(Client *clients, int k, int actual, Player p1, Player p2);

void p_BFIGHT(Client clients[], int k, int actual, std::string buffer);

void p_BREF(Client *clients, int k, int actual, Player p1, Player p2);

void p_BWIN(Client clients[], int k, int actual);

void p_BLOSE(Client clients[], int k, int actual);

void p_BLOGOUT(Client clients[], int k, int actual);

void p_BBYE(Client clients[], int k, int actual);

void p_BTIMEDOUT(Client clients[], int k, int actual);

void p_BFALL(Client clients[], int k, int actual);

#endif //CPPSERVER_CLIENTIO_H
