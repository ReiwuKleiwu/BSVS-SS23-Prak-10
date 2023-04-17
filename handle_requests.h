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
    METHOD_DELETE
} RequestMethod;

void requestHandler(char* request, hash_table *keyValStore, char* res, int requestBufferSize);
RequestMethod stringToRequestMethod(const char* method);

#endif //BSVS_SS23_PRAK_HANDLE_REQUESTS_H
