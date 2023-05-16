#ifndef BSVS_SS23_PRAK_HANDLE_METHODS_H
#define BSVS_SS23_PRAK_HANDLE_METHODS_H

#include "hashtable.h"
#include "handle_requests.h"
#include "sub_store.h"

void methodHandler(RequestMethod method, const char* key, const char* value, Request client_request);
void handlePUT(const char* key, const char* value, Request client_request);
void handleGET(const char* key, Request client_request);
void handleDELETE(const char* key, Request client_request);
void handleSUB(const char* key, Request client_request);
void handleUNSUB(const char* key, Request client_request);
bool is_subscribed(SubStore* subscriber_store, const char *key, int client_pid);
void handleQUIT(Request client_request);


#endif //BSVS_SS23_PRAK_HANDLE_METHODS_H
