#include "../includes/QClient.h"

using namespace std;

QClient::QClient(int socket) {
	sock = socket;
}

QClient::QClient() = default;
