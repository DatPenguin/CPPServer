#ifndef CLIENT_H
#define CLIENT_H

typedef struct {
    SOCKET sock;
    std::string name;
} Client;

#endif /* guard */
