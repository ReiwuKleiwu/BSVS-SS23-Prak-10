#include <string.h>
#include <stdio.h>
#include "handle_requests.h"
#include "validate_user_input.h"
#include "handle_methods.h"

RequestMethod stringToRequestMethod(const char *method) {
    if (strcmp(method, "GET") == 0) {
        return METHOD_GET;
    } else if (strcmp(method, "PUT") == 0) {
        return METHOD_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return METHOD_DELETE;
    } else if (strcmp(method, "SUB") == 0) {
        return METHOD_SUB;
    } else if (strcmp(method, "UNSUB") == 0) {
        return METHOD_UNSUB;
    } else if (strcmp(method, "BEG") == 0) {
        return METHOD_BEG;
    } else if (strcmp(method, "END") == 0) {
        return METHOD_END;
    } else if (strcmp(method, "QUIT") == 0) {
        return METHOD_QUIT;
    } else {
        return METHOD_UNKNOWN;
    }
}

void requestHandler(Request client_request) {
    char *method = strtok(client_request.body, ":");
    char *key = strtok(NULL, ":");
    char *value = strtok(NULL, ":");

    if (method) removeWhitespaceChars(method);
    if (key) removeWhitespaceChars(key);

    printf("The method used was : %s\n", method);
    printf("The key requested was: %s\n", key);
    printf("The value requested was: %s\n", value);

    methodHandler(stringToRequestMethod(method), key, value, client_request);
}