#include "handle_methods.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include <sys/socket.h>
#include <unistd.h>
#include "sub_message_queue.h"

void methodHandler(RequestMethod method, const char *key, const char *value, Request client_request) {
    switch (method) {
        case METHOD_GET:
            handleGET(key, client_request);
            break;
        case METHOD_PUT:
            handlePUT(key, value, client_request);
            break;
        case METHOD_DELETE:
            handleDELETE(key, client_request);
            break;
        case METHOD_QUIT:
            handleQUIT(client_request);
            break;
        case METHOD_SUB:
            handleSUB(key, client_request);
            break;
        case METHOD_UNSUB:
            handleUNSUB(key, client_request);
            break;
        default:
            printf("Unknown method\n");
            break;
    }
}

void handlePUT(const char *key, const char *value, Request client_request) {
    if (value == NULL) {
        snprintf(client_request.response, RESPONSESIZE, "PUT operation: Value is null. Use PUT:KEY:VALUE\r\n");
        return;
    }
    if (key == NULL) {
        snprintf(client_request.response, RESPONSESIZE, "PUT operation: Key is null. Use PUT:KEY:VALUE\r\n");
        return;
    }

    if (hash_table_upsert(client_request.key_value_store, key, value)) {
        snprintf(client_request.response, RESPONSESIZE,
                 "PUT operation: Key: \"%s\", Value: \"%s\" successfully inserted/updated.\r\n", key, value);
    } else {
        snprintf(client_request.response, RESPONSESIZE,
                 "PUT operation: Error occurred while inserting Key: \"%s\", Value: \"%s\".\r\n", key, value);
    }

    char notify_message[MAX_PAYLOAD_SIZE];
    snprintf(notify_message, MAX_PAYLOAD_SIZE, "Client_%d performed PUT-Operation on Key: \"%s\" with Value: \"%s\".\r\n", client_request.client_pid, key, value);

    notify_on_event(client_request.sub_queue_id, client_request.subscriber_store, notify_message, key);
    hash_table_print(client_request.key_value_store);
    send_response(client_request);
}

void handleGET(const char *key, Request client_request) {
    if (key == NULL) {
        snprintf(client_request.response, RESPONSESIZE, "GET operation: Key is null. Use GET:KEY\r\n");
        return;
    }

    char *value = hash_table_lookup(client_request.key_value_store, key);

    if (!value) {
        snprintf(client_request.response, RESPONSESIZE, "GET operation: Key: \"%s\" not found in the store.\r\n", key);
    } else {
        snprintf(client_request.response, RESPONSESIZE,
                 "GET operation: Key: \"%s\", Value: \"%s\" found in the store.\r\n", key,
                 value);
    }

    hash_table_print(client_request.key_value_store);
    send_response(client_request);
}

void handleDELETE(const char *key, Request client_request) {
    if (key == NULL) {
        snprintf(client_request.response, RESPONSESIZE, "DELETE operation: Key is null. Use DELETE:KEY\r\n");
        return;
    }

    bool deleted = hash_table_delete(client_request.key_value_store, key);

    if (!deleted) {
        snprintf(client_request.response, RESPONSESIZE,
                 "DELETE operation: Key: \"%s\" not found in the store. No deletion occurred.\r\n", key);
    } else {
        snprintf(client_request.response, RESPONSESIZE,
                 "DELETE operation: Key: \"%s\" successfully deleted from the store.\r\n",
                 key);
    }
    char notify_message[MAX_PAYLOAD_SIZE];
    snprintf(notify_message, MAX_PAYLOAD_SIZE, "Client_%d performed DELETE-Operation on Key: \"%s\".\r\n", client_request.client_pid, key);

    notify_on_event(client_request.sub_queue_id, client_request.subscriber_store, notify_message, key);
    hash_table_print(client_request.key_value_store);
    send_response(client_request);
}

void handleSUB(const char *key, Request client_request) {
    if (is_subscribed(client_request.subscriber_store, key, client_request.client_pid)) {
        snprintf(client_request.response, RESPONSESIZE,
                 "SUB operation: Failed to subscribe to key \"%s\" (already subscribed).\r\n", key);
    } else {
        sub_store_upsert(client_request.subscriber_store, key, client_request.client_pid);
        snprintf(client_request.response, RESPONSESIZE, "SUB operation: Successfully subscribed to key \"%s\".\r\n", key);
    }

    sub_store_print(client_request.subscriber_store);
    send_response(client_request);
}

void handleUNSUB(const char *key, Request client_request) {
    if (is_subscribed(client_request.subscriber_store, key, client_request.client_pid)) {
        sub_store_delete(client_request.subscriber_store, key, client_request.client_pid);
        snprintf(client_request.response, RESPONSESIZE, "UNSUB operation: Successfully unsubscribed from key \"%s\".\r\n", key);
    } else {
        snprintf(client_request.response, RESPONSESIZE,
                 "UNSUB operation: Failed to unsubscribe from key \"%s\" (not subscribed).\r\n", key);
    }

    sub_store_print(client_request.subscriber_store);
    send_response(client_request);
}


void handleQUIT(Request client_request) {
    snprintf(client_request.response, RESPONSESIZE, "See you soon!\r\n");
    send(client_request.client_socket, client_request.response, strlen(client_request.response), 0);
    shutdown(client_request.client_socket, SHUT_RDWR);
    close(client_request.client_socket);
}



bool is_subscribed(SubStore* subscriber_store, const char* key, int client_pid) {
    Subscriber *subscribed_processes = sub_store_lookup(subscriber_store, key);

    if (!subscribed_processes) return false;

    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (subscribed_processes[i].pid == client_pid) {
            return true;
        }
    }
    return false;
}
