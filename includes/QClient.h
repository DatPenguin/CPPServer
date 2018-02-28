#ifndef CPPSERVER_QCLIENT_H
#define CPPSERVER_QCLIENT_H

#include <iostream>

class QClient {
public:
	explicit QClient(int socket);

	explicit QClient();

	int sock;
};


#endif //CPPSERVER_QCLIENT_H
