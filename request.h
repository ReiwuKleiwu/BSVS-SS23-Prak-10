#ifndef BSVS_SS23_PRAK_REQUEST_H
#define BSVS_SS23_PRAK_REQUEST_H

#include "hashtable.h"
#include "sub_store.h"

#define REQUESTSIZE 1024
#define RESPONSESIZE 1024

typedef struct Request {
    char body[REQUESTSIZE];
    char response[RESPONSESIZE];
    HashTable *key_value_store;
    SubStore *subscriber_store;
    int sub_queue_id;
    int client_socket;
    int client_pid;
    int *server_is_locked_by_transaction;
} Request;

void send_response(Request client_request);

#endif //BSVS_SS23_PRAK_REQUEST_H
