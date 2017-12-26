/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include "includes/Fighter.h"

Fighter::Fighter(std::string login) {
    // TODO Requete BD pour peupler
}

Fighter::Fighter() {
    hp = 10;
    armor = 0;
    ballsNb = 1;
    ballsSpeed = 1;
    ballsSize = 1;
    buttonSize = 1;
    paddleSize = 1;
}
