//
// Created by struc on 17.04.2023.
//
#include "handle_requests.h"
#include "validate_user_input.h"
#include "handle_methods.h"
#include <string.h>
#include <stdio.h>

RequestMethod stringToRequestMethod(const char* method) {
    if (strcmp(method, "GET") == 0) {
        return METHOD_GET;
    } else if (strcmp(method, "PUT") == 0) {
        return METHOD_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return METHOD_DELETE;
    } else if (strcmp(method, "QUIT") == 0) {
        return METHOD_QUIT;
    }
    else {
        return METHOD_UNKNOWN;
    }
}

void requestHandler(char* request, hash_table *keyValStore, char* res, int requestBufferSize, int socket_client) {
    char* method = strtok(request, ":");
    char* key = strtok(NULL, ":");
    char* value = strtok(NULL, ":");

    if(method) removeWhitespaceChars(method);
    if(key) removeWhitespaceChars(key);

    printf("The method used was : %s\n", method);
    printf("The key requested was: %s\n", key);
    printf("The value requested was: %s\n", value);

    methodHandler(stringToRequestMethod(method), key, value, keyValStore, res, requestBufferSize, socket_client);
}