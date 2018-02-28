#include "../includes/query.h"

using namespace std;

void send_info_to_client(QClient client, string infoLabel, int info) {
	string buffer;

	buffer += infoLabel + " : " + to_string(info);

	send_message_to_client(client, buffer);
}
