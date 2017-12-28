/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include <sstream>
#include "includes/Fighter.h"

using namespace std;

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

string Fighter::toString() {
	stringstream ss;
	ss << hp << ";" << armor << ";" << ballsNb << ";" << ballsSpeed << ";" + ballsSize << ";" << buttonSize + ";" << paddleSize;
	return ss.str();
}
