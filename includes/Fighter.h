/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#ifndef CPPSERVER_FIGHTER_H
#define CPPSERVER_FIGHTER_H

#include <iostream>
#include "Spell.h"


class Fighter {
public:
	explicit Fighter();

	std::string toString();

	void applySpell(Spell s);

	void applyOpponentsSpell(Spell s);

	bool isDead();

	void fallen();

	void resetFighter();

	void getDamage(int dmg);

private:
	int hp;
	int armor;
	int ballsNb;
	float ballsSpeed;
	int ballsSize;
	int buttonSize;
	int paddleSize;
	int paddleSpeed;
	std::string login;
};

#endif //CPPSERVER_FIGHTER_H
