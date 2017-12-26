/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include "includes/Player.h"

Player::Player(int socket) : socket(socket) {
    combatInfos = new Fighter();
}

std::string Player::toString() {
    // TODO Faire un vrai toString()
    return login;
}
