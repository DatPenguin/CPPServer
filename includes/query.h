#ifndef CPPSERVER_QUERY_H
#define CPPSERVER_QUERY_H

#include "QClient.h"
#include "clientio.h"
#include <iostream>

void send_info_to_client(QClient client, std::string infoLabel, int info);

#endif //CPPSERVER_QUERY_H
