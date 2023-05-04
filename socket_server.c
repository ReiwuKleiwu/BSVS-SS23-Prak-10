#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "socket_server.h"
#include "validate_user_input.h"
#include "handle_requests.h"
#include "hashtable.h"
#include "sub_store.h"
#include "request.h"

#define SHOW_LOGS 1
#define BUFFERSIZE 1024

void handleClientConnections(int listening_socket, HashTable *keyValStore, SubStore *subStore) {
    int client_socket;
    char clientRequest[BUFFERSIZE + 1];
    struct sockaddr_in client;
    socklen_t client_len;



    while(1) {
        if(SHOW_LOGS) {
            printf("Waiting for incoming socket connection..\n");
        }

        client_socket = accept(listening_socket, (struct sockaddr *) &client, &client_len);

        if (fork() == 0) {
            char *connected = "Welcome:\r\n";
            if(SHOW_LOGS) {
                printf("INFO: client connected\n");
            }
            send(client_socket, connected, strlen(connected), 0);


            int pid = getpid();

            printf("Process ID: %d\n", pid);

            ssize_t received_client_bytes;
            while ((received_client_bytes = readUntilNewLine(client_socket, client_request_buffer, BUFFERSIZE)) > 0) {
                Request client_request;
                strncpy(client_request.body, client_request_buffer, strlen(client_request_buffer));
                client_request.key_value_store = keyValStore;
                client_request.subscriber_store = subStore;
                client_request.client_socket = client_socket;
                client_request.client_pid = getpid();

                sanitizeUserInput(client_request.body);
                validateFormat(client_request);
                requestHandler(client_request);
            }

            if(SHOW_LOGS) {
                printf("INFO: client disconnected\n");
            }
            exit(0);
        } else {
            close(client_socket);
        }
    }
}

int readUntilNewLine(int socket_client, char *buf, int len) {
    int total_bytes_read = 0;
    int bytes_read;
    char tmp;

    memset(buf, 0, len);

    while ((bytes_read = recv(socket_client, &tmp, 1, 0)) > 0 && total_bytes_read < len - 1) {
        if (tmp == '\r') {
            break;
        }

        buf[total_bytes_read++] = tmp;
    }

    buf[total_bytes_read] = '\0';

    return bytes_read > 0 ? total_bytes_read : -1;
}