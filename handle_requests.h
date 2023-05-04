#ifndef BSVS_SS23_PRAK_HANDLE_REQUESTS_H
#define BSVS_SS23_PRAK_HANDLE_REQUESTS_H

#include "hashtable.h"
#include "sub_store.h"
#include "request.h"

typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_GET,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_SUB,
    METHOD_UNSUB,
    METHOD_QUIT
} RequestMethod;

void requestHandler(Request client_request);
RequestMethod stringToRequestMethod(const char* method);

#endif //BSVS_SS23_PRAK_HANDLE_REQUESTS_H
