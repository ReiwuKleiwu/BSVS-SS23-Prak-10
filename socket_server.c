//
// Created by struc on 20.04.2023.
//

#include "socket_server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "validate_user_input.h"
#include "handle_requests.h"
#include "hashtable.h"


#define SHOW_LOGS 1
#define BUFFERSIZE 1024

void handleClientConnections(int listening_socket, hash_table *keyValStore) {
    int client_socket;
    char clientRequest[BUFFERSIZE + 1];
    struct sockaddr_in client;
    socklen_t client_len;


    while(1) {
        if(SHOW_LOGS) {
            printf("Waiting for incoming socket connection..\n");
        }

        client_socket = accept(listening_socket, (struct sockaddr *) &client, &client_len);

        if(SHOW_LOGS) {
            printf("Socket connected to server!\n");
            const char res[] = "Willkommen: \r\n";
            send(client_socket, res, strlen(res), 0);
        }

        ssize_t received_bytes;
        while ((received_bytes = readUntilNewLine(client_socket, clientRequest, BUFFERSIZE)) > 0) {

            char serverResponse[BUFFERSIZE];

            removeControlChars(clientRequest);
            validateFormat(clientRequest, serverResponse);
            requestHandler(clientRequest, keyValStore, serverResponse, BUFFERSIZE, client_socket);

            send(client_socket, serverResponse, strlen(serverResponse), 0);
        }
        close(client_socket);
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