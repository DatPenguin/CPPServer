/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include <sstream>
#include "../includes/Fighter.h"

using namespace std;

Fighter::Fighter() {
	hp = 10;
	armor = 0;
	ballsNb = 1;
	ballsSpeed = 1;
	ballsSize = 1;
	buttonSize = 1;
	paddleSize = 1;
	paddleSpeed = 1;
}

string Fighter::toString() {
	stringstream ss;
	ss << hp << ";" << armor << ";" << ballsNb << ";" << ballsSpeed << ";" << ballsSize << ";" << buttonSize << ";"
	   << paddleSize;
	return ss.str();
}

void Fighter::applySpell(Spell s) {
	getDamage(s.damages);
	ballsSpeed *= s.ballSpeed;
	ballsNb += s.ball_multiplication;
	paddleSize *= s.paddleSize;
	paddleSpeed *= s.paddleSpeed;

	if (armor + s.shield >= 3)
		armor = 3;
	else
		armor += s.shield;
}

void Fighter::applyOpponentsSpell(Spell s) {
	getDamage(s.damages_o);
	ballsSpeed *= s.ballSpeed_o;
	ballsNb += s.ball_multiplication_o;
	paddleSize *= s.paddleSize_o;
	paddleSpeed *= s.paddleSpeed_o;

	if (armor + s.shield_o >= 3)
		armor = 3;
	else
		armor += s.shield_o;
}

bool Fighter::isDead() {
	return (hp <= 0);
}

void Fighter::fallen() {
	getDamage(1);
}

void Fighter::resetFighter() {
	hp = 10;
	armor = 0;
	ballsNb = 1;
	ballsSpeed = 1;
	ballsSize = 1;
	buttonSize = 1;
	paddleSize = 1;
	paddleSpeed = 1;
}

void Fighter::getDamage(int dmg) {
	if (armor >= dmg)
		armor -= dmg;
	else if (armor > 0) {
		int tmp_dmg = dmg - armor;
		armor = 0;
		hp -= tmp_dmg;
	} else
		hp -= dmg;
}
