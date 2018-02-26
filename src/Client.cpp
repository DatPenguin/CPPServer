/**
* Created by matteo on 24/12/17.
* Copyright (c) 2017-2018 The Dankest Production. All rights reserved.
*/

#include "../includes/Client.h"

using namespace std;

Client::Client(int socket) {
    sock = socket;
}

bool Client::isInMatchMaking() {
    return mmIndex != -1;
}

Client::Client() = default;
