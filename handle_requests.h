//
// Created by struc on 17.04.2023.
//

#ifndef BSVS_SS23_PRAK_HANDLE_REQUESTS_H
#define BSVS_SS23_PRAK_HANDLE_REQUESTS_H

#include "hashtable.h"

typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_GET,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_QUIT
} RequestMethod;

void requestHandler(char* request, HashTable *keyValStore, char* res, int requestBufferSize, int socket_client);
RequestMethod stringToRequestMethod(const char* method);

#endif //BSVS_SS23_PRAK_HANDLE_REQUESTS_H
