/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#ifndef CPPSERVER_FIGHTER_H
#define CPPSERVER_FIGHTER_H

#include <iostream>


class Fighter {
public:
    explicit Fighter(std::string login);
    explicit Fighter();

private:
    int hp;
    int armor;
    int ballsNb;
    float ballsSpeed;
    int ballsSize;
    int buttonSize;
    int paddleSize;
    std::string login;
};

#endif //CPPSERVER_FIGHTER_H
