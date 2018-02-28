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

class Client {
public:
	explicit Client(int socket);

	explicit Client();

	bool isInMatchMaking();

	int sock;
	int mmIndex = -1;
	std::string login;
	std::vector<std::string> heroList;
	std::string selectedClass;
	Fighter combatInfos;
	std::vector<Spell> spellsList;
	std::vector<Spell> selectedSpells;
	bool is_auth = false;
};

#endif //CPPSERVER_PLAYER_H
