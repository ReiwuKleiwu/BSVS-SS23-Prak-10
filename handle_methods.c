//
// Created by struc on 17.04.2023.
//

#include "handle_methods.h"
#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include <sys/socket.h>
#include <unistd.h>

void methodHandler(RequestMethod method, const char* key, const char* value, HashTable *keyValStore, char* res, int requestBufferSize, int socket_client) {
    switch (method) {
        case METHOD_GET:
            handleGET(key, keyValStore, res, requestBufferSize);
            break;
        case METHOD_PUT:
            handlePUT(key, value, keyValStore, res, requestBufferSize);
            break;
        case METHOD_DELETE:
            handleDELETE(key, keyValStore, res, requestBufferSize);
            break;
        case METHOD_QUIT:
            handleQUIT(res, requestBufferSize, socket_client);
            break;
        default:
            printf("Unknown method\n");
            break;
    }
}

void handlePUT(const char* key, const char* value, HashTable *keyValStore, char* res, int requestBufferSize) {
    if(value == NULL) {
        snprintf(res, requestBufferSize, "PUT operation: Value is null. Use PUT:KEY:VALUE\r\n");
        return;
    }
    if(key == NULL) {
        snprintf(res, requestBufferSize, "PUT operation: Key is null. Use PUT:KEY:VALUE\r\n");
        return;
    }

    if(hash_table_upsert(keyValStore, key, value)) {
        snprintf(res, requestBufferSize, "PUT operation: Key: \"%s\", Value: \"%s\" successfully inserted/updated.\r\n", key, value);
    } else {
        snprintf(res, requestBufferSize, "PUT operation: Error occurred while inserting Key: \"%s\", Value: \"%s\".\r\n", key, value);
    }

    //hash_table_print(keyValStore);
}

void handleGET(const char* key, HashTable *keyValStore, char* res, int requestBufferSize) {
    if(key == NULL) {
        snprintf(res, requestBufferSize, "GET operation: Key is null. Use GET:KEY\r\n");
        return;
    }

    char* value = hash_table_lookup(keyValStore, key);

    if(!value) {
        snprintf(res, requestBufferSize, "GET operation: Key: \"%s\" not found in the store.\r\n", key);
    } else {
        snprintf(res, requestBufferSize, "GET operation: Key: \"%s\", Value: \"%s\" found in the store.\r\n", key, value);
    }

    //hash_table_print(keyValStore);
}

void handleDELETE(const char* key, HashTable *keyValStore, char* res, int requestBufferSize) {
    if(key == NULL) {
        snprintf(res, requestBufferSize, "DELETE operation: Key is null. Use DELETE:KEY\r\n");
        return;
    }

    const char* deletedValue = hash_table_delete(keyValStore, key);

    if(!deletedValue) {
        snprintf(res, requestBufferSize, "DELETE operation: Key: \"%s\" not found in the store. No deletion occurred.\r\n", key);
    } else {
        snprintf(res, requestBufferSize, "DELETE operation: Key: \"%s\", Value: \"%s\" successfully deleted from the store.\r\n", key, deletedValue);
    }

    //hash_table_print(keyValStore);
}

void handleQUIT(char* res, int requestBufferSize, int socket_client) {
    snprintf(res, requestBufferSize, "Auf Wiedersehen!\r\n");
    send(socket_client, res, strlen(res), 0);
    shutdown(socket_client, SHUT_RDWR);
    close(socket_client);
}