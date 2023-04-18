/*******************************************************************************

  Ein TCP-Echo-Server als iterativer Server: Der Server schickt einfach die
  Daten, die der Client schickt, an den Client zurück.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "hashtable.h"
#include "handle_requests.h"
#include "validate_user_input.h"

#define BUFFERSIZE 1024 // Größe des Buffers
#define PORT 5678
#define SHOW_LOGS 1

int readUntilNewLine(int socket_client, char *buf, int len);

int main() {
    const int tablesize = (1 << 20);
    hash_table *keyValStore = hash_table_create(tablesize);

    int listening_socket; // Rendevouz-Descriptor
    int client_socket; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char clientRequest[BUFFERSIZE + 1]; // Daten vom Client an den Server
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen
    listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int bind_result = bind(listening_socket, (struct sockaddr *) &server, sizeof(server));
    if (bind_result < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(listening_socket, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while(1) {
        if(SHOW_LOGS) {
            printf("Waiting for incoming socket connection..\n");
        }

        // Verbindung eines Clients wird entgegengenommen
        client_socket = accept(listening_socket, (struct sockaddr *) &client, &client_len);

        if(SHOW_LOGS) {
            printf("Socket connected to server!\n");
            const char res[] = "Willkommen: \r\n";
            send(client_socket, res, strlen(res), 0);
        }
        
        // Daten vom Client empfangen
        ssize_t received_bytes;
        while ((received_bytes = readUntilNewLine(client_socket, clientRequest, BUFFERSIZE)) > 0) {

            char serverResponse[BUFFERSIZE];

            char* validatedRequest = validateFormat(clientRequest);

            if(validatedRequest == NULL) {
                strcpy(serverResponse, "Der Befehl muss mit PUT:, GET: oder DELETE: beginnen und das richtige Format haben. \r\n");
                send(client_socket, serverResponse, strlen(serverResponse), 0);
                continue;
            }
            requestHandler(validatedRequest, keyValStore, serverResponse, BUFFERSIZE);

            // Antwort senden
            send(client_socket, serverResponse, strlen(serverResponse), 0);
        }
        close(client_socket);
    }

    // Rendevouz Descriptor schließen
    close(listening_socket);
}

/*
int readUntilNewLine(int socket_client, char *buf, int len) {
    int total_read = 0, bytes_read;
    char *s = buf;

    while (total_read < len - 1) {
        bytes_read = recv(socket_client, s, 1, 0);
        if (bytes_read <= 0 || *s == '\n' || *s == '\r') break;
        s += bytes_read;
        total_read += bytes_read;
    }

    *s = '\0'; // Null-Terminator hinzufügen
    return (bytes_read < 0) ? bytes_read : total_read;
}
 */

int readUntilNewLine(int socket_client, char *buf, int len) {
    int total_bytes_read = 0;
    int bytes_read;
    char tmp;

    // Ensure the buffer is initially empty
    memset(buf, 0, len);

    // Read data from the client socket until a newline is encountered
    while ((bytes_read = recv(socket_client, &tmp, 1, 0)) > 0 && total_bytes_read < len - 1) {
        if (tmp == '\r') {
            break;
        }

        buf[total_bytes_read++] = tmp;
    }

    // Add null-terminator to the end of the buffer
    buf[total_bytes_read] = '\0';

    // Return the number of bytes read, or -1 if an error occurred
    return bytes_read > 0 ? total_bytes_read : -1;
}