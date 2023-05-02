#ifndef BSVS_SS23_PRAK_HANDLE_METHODS_H
#define BSVS_SS23_PRAK_HANDLE_METHODS_H

#include "hashtable.h"
#include "handle_requests.h"
#include "subStore.h"

void methodHandler(RequestMethod method, const char* key, const char* value, HashTable *keyValStore, SubStore *subStore, char* res, int responseBufferSize, int socket_client);
void handlePUT(const char* key, const char* value, HashTable *keyValStore, char* res, int responseBufferSize);
void handleGET(const char* key, HashTable *keyValStore, char* res, int responseBufferSize);
void handleDELETE(const char* key, HashTable *keyValStore, char* res, int responseBufferSize);
void handleSUB(const char* key, SubStore *subStore, char* res, int responseBufferSize);
void handleUNSUB(const char* key, SubStore *subStore, char* res, int responseBufferSize);
void handleQUIT(char* res, int responseBufferSize, int socket_client);


#endif //BSVS_SS23_PRAK_HANDLE_METHODS_H
