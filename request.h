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
    int client_socket;
    int client_pid;
} Request;

void send_response(Request client_request);

#endif //BSVS_SS23_PRAK_REQUEST_H
