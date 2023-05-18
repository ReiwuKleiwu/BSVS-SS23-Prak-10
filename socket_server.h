#ifndef BSVS_SS23_PRAK_SOCKET_SERVER_H
#define BSVS_SS23_PRAK_SOCKET_SERVER_H

#include <netinet/in.h>
#include "hashtable.h"
#include "sub_store.h"

void runServer(int port);

static int create_listening_socket(int port);

static void handleClientConnections(int listening_socket, HashTable *keyValStore, SubStore *subStore,
                                    int *server_is_locked_by_transaction, int sub_queue_id);

static int readUntilNewLine(int socket_client, char *buf, int len);

static void process_client_request(int client_socket, char *client_request_buffer, ssize_t received_client_bytes,
                                   HashTable *keyValStore, SubStore *subStore);

#endif //BSVS_SS23_PRAK_SOCKET_SERVER_H
