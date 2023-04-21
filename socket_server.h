//
// Created by struc on 20.04.2023.
//

#ifndef BSVS_SS23_PRAK_SOCKET_SERVER_H
#define BSVS_SS23_PRAK_SOCKET_SERVER_H

#include <netinet/in.h>
#include "hashtable.h"

void handleClientConnections(int listening_socket, HashTable *keyValStore);
int readUntilNewLine(int socket_client, char *buf, int len);

#endif //BSVS_SS23_PRAK_SOCKET_SERVER_H
