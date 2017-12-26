/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#ifndef CPPSERVER_PLAYER_H
#define CPPSERVER_PLAYER_H

#include <iostream>
#include <vector>
#include "Fighter.h"
#include "Spell.h"

class Player {
public:
    explicit Player(int socket);

    std::string toString();

private:
    int socket;
    std::string login;
    std::vector<std::string> heroList;
    std::string selectedHero;
    Fighter *combatInfos;
    std::vector<Spell> spellsList;
    std::vector<Spell> selectedSpells;
};


#endif //CPPSERVER_PLAYER_H
