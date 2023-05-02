#ifndef BSVS_SS23_PRAK_SOCKET_SERVER_H
#define BSVS_SS23_PRAK_SOCKET_SERVER_H

#include <netinet/in.h>
#include "hashtable.h"
#include "subStore.h"

void handleClientConnections(int listening_socket, HashTable *keyValStore, SubStore *subStore);
int readUntilNewLine(int socket_client, char *buf, int len);

#endif //BSVS_SS23_PRAK_SOCKET_SERVER_H
