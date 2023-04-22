#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "socket_server.h"
#include "validate_user_input.h"
#include "handle_requests.h"
#include "hashtable.h"

#define SHOW_LOGS 1
#define BUFFERSIZE 1024

void handleClientConnections(int listening_socket, HashTable *keyValStore) {
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


            ssize_t received_bytes;
            while ((received_bytes = readUntilNewLine(client_socket, clientRequest, BUFFERSIZE)) > 0) {
                char serverResponse[BUFFERSIZE];

                sanitizeUserInput(clientRequest);
                validateFormat(clientRequest, serverResponse);
                requestHandler(clientRequest, keyValStore, serverResponse, BUFFERSIZE, client_socket);

                send(client_socket, serverResponse, strlen(serverResponse), 0);
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