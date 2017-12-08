#ifndef CPPSERVER_BRICKEST_H
#define CPPSERVER_BRICKEST_H

#include <string>
#include <iostream>
#include "fighter.h"
#include "spell.h"

typedef struct { // NOLINT
    SOCKET sock;
    std::string login;
} Client;

typedef struct {
    Client c1;
    Client c2;
} MMStruct;

#endif //CPPSERVER_BRICKEST_H
