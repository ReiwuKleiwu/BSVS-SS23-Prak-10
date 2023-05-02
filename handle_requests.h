#ifndef BSVS_SS23_PRAK_HANDLE_REQUESTS_H
#define BSVS_SS23_PRAK_HANDLE_REQUESTS_H

#include "hashtable.h"
#include "subStore.h"

typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_GET,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_SUB,
    METHOD_UNSUB,
    METHOD_QUIT
} RequestMethod;

void requestHandler(char* request, HashTable *keyValStore, SubStore *subStore, char* res, int responseBufferSize, int socket_client);
RequestMethod stringToRequestMethod(const char* method);

#endif //BSVS_SS23_PRAK_HANDLE_REQUESTS_H
