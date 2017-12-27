/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include "includes/Client.h"

Client::Client(int socket) : sock(socket) {
    combatInfos = new Fighter();
}

std::string Client::toString() {
    // TODO Faire un vrai toString()
    return login;
}

Client::Client() {
    combatInfos = new Fighter();
}
