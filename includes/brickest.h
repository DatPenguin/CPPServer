#ifndef CPPSERVER_BRICKEST_H
#define CPPSERVER_BRICKEST_H

#include <string>
#include <iostream>
#include "Spell.h"

typedef struct { // NOLINT
    SOCKET sock;
    std::string login;
    bool is_auth = false;
} Client;

typedef struct {
    Client c1;
    Client c2;
} MMStruct;

#endif //CPPSERVER_BRICKEST_H
