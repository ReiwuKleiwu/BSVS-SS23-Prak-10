//
// Created by struc on 17.04.2023.
//

#ifndef BSVS_SS23_PRAK_HANDLE_METHODS_H
#define BSVS_SS23_PRAK_HANDLE_METHODS_H

#include "hashtable.h"
#include "handle_requests.h"

void methodHandler(RequestMethod method, const char* key, const char* value, HashTable *keyValStore, char* res, int requestBufferSize, int socket_client);
void handlePUT(const char* key, const char* value, HashTable *keyValStore, char* res, int requestBufferSize);
void handleGET(const char* key, HashTable *keyValStore, char* res, int requestBufferSize);
void handleDELETE(const char* key, HashTable *keyValStore, char* res, int requestBufferSize);
void handleQUIT(char* res, int requestBufferSize, int socket_client);


#endif //BSVS_SS23_PRAK_HANDLE_METHODS_H
