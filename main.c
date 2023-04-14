/*******************************************************************************

  Ein TCP-Echo-Server als iterativer Server: Der Server schickt einfach die
  Daten, die der Client schickt, an den Client zurück.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "hashtable.h"

#define BUFFERSIZE 1024 // Größe des Buffers
#define PORT 5678
#define SHOW_LOGS 1


typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_GET,
    METHOD_PUT,
    METHOD_DELETE
} RequestMethod;

void requestHandler(char* request, hash_table *keyValStore, char* res);
void methodHandler(RequestMethod method, const char* key, const char* value, hash_table *keyValStore, char* res);
RequestMethod stringToRequestMethod(const char* method);
void handlePUT(const char* key, const char* value, hash_table *keyValStore, char* res);
void handleGET(const char* key, hash_table *keyValStore, char* res);
void handleDELETE(const char* key, hash_table *keyValStore, char* res);
void removeTrailingNewline(char* str);

int main() {
    const int tablesize = (1 << 20);
    hash_table *keyValStore = hash_table_create(tablesize);

    int listening_socket; // Rendevouz-Descriptor
    int client_socket; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFFERSIZE + 1]; // Daten vom Client an den Server
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
            printf("Socket connected to server!");
        }

        // Lesen von Daten, die der Client schickt
        bytes_read = read(client_socket, in, BUFFERSIZE);

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (bytes_read > 0) {

            in[bytes_read] = '\0';

            char res[BUFFERSIZE];

            requestHandler(in, keyValStore, res);

            write(client_socket, res, strlen(res));
            bytes_read = read(client_socket, in, BUFFERSIZE);
            strcpy(res, "");
        }

        close(client_socket);
    }

    // Rendevouz Descriptor schließen
    close(listening_socket);
}

RequestMethod stringToRequestMethod(const char* method) {
    if (strcmp(method, "GET") == 0) {
        return METHOD_GET;
    } else if (strcmp(method, "PUT") == 0) {
        return METHOD_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return METHOD_DELETE;
    } else {
        return METHOD_UNKNOWN;
    }
}

void removeTrailingNewline(char* str) {
    str[strcspn(str, "\n")] = 0;
}

void requestHandler(char* request, hash_table *keyValStore, char* res) {
    char* method = strtok(request, ":");
    char* key = strtok(NULL, ":");
    char* value = strtok(NULL, ":");

    if(!(method && key)) return;

    removeTrailingNewline(method);
    removeTrailingNewline(key);
    if (value) {
        removeTrailingNewline(value);
    }

    if(SHOW_LOGS) {
        printf("The method used was : %s\n", method);
        printf("The key requested was: %s\n", key);
        printf("The value requested was: %s\n", value);
    }

    methodHandler(stringToRequestMethod(method), key, value, keyValStore, res);
}

void methodHandler(RequestMethod method, const char* key, const char* value, hash_table *keyValStore, char* res) {
    switch (method) {
        case METHOD_GET:
            handleGET(key, keyValStore, res);
            break;
        case METHOD_PUT:
            handlePUT(key, value, keyValStore, res);
            break;
        case METHOD_DELETE:
            handleDELETE(key, keyValStore, res);
            break;
        default:
            printf("Unknown method\n");
            break;
    }
}

void handlePUT(const char* key, const char* value, hash_table *keyValStore, char* res) {
    if(value == NULL) {
        if(SHOW_LOGS) printf("Value was NULL!");
    }

    if(hash_table_upsert(keyValStore, key, value)) {
        snprintf(res, BUFFERSIZE, "PUT operation: Key: \"%s\", Value: \"%s\" successfully inserted/updated.\n", key, value);
    } else {
        snprintf(res, BUFFERSIZE, "PUT operation: Error occurred while inserting Key: \"%s\", Value: \"%s\".\n", key, value);
    }

    if(SHOW_LOGS) hash_table_print(keyValStore);
}

void handleGET(const char* key, hash_table *keyValStore, char* res) {
    char* value = hash_table_lookup(keyValStore, key);

    if(!value) {
        snprintf(res, BUFFERSIZE, "GET operation: Key: \"%s\" not found in the store.\n", key);
    } else {
        snprintf(res, BUFFERSIZE, "GET operation: Key: \"%s\", Value: \"%s\" found in the store.\n", key, value);
    }

    if(SHOW_LOGS) hash_table_print(keyValStore);
}

void handleDELETE(const char* key, hash_table *keyValStore, char* res) {
    const char* deletedValue = hash_table_delete(keyValStore, key);

    if(!deletedValue) {
        snprintf(res, BUFFERSIZE, "DELETE operation: Key: \"%s\" not found in the store. No deletion occurred.\n", key);
    } else {
        snprintf(res, BUFFERSIZE, "DELETE operation: Key: \"%s\", Value: \"%s\" successfully deleted from the store.\n", key, deletedValue);
    }

    if(SHOW_LOGS) hash_table_print(keyValStore);
}