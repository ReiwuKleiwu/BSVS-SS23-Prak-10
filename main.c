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

#define BUFFERSIZE 1024 // Größe des Buffers
#define PORT 5678
#define SHOW_LOGS 1


typedef enum {
    METHOD_UNKNOWN = 0,
    METHOD_GET,
    METHOD_PUT,
    METHOD_DELETE
} RequestMethod;

void requestHandler(char* request, char* res);
void methodHandler(RequestMethod method, const char* key, const char* value, char* res);
RequestMethod stringToRequestMethod(const char* method);
void handlePUT(const char* key, const char* value, char* res);
void handleGET(const char* key, char* res);
void handleDELETE(const char* key, char* res);

int main() {

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

            requestHandler(in, res);

            printf("%s\n", in);
            printf("Response: %s\n", res);

            write(client_socket, res, BUFFERSIZE);
            bytes_read = read(client_socket, in, BUFFERSIZE);
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

void requestHandler(char* request, char* res) {
    const char* method = strtok(request, ":");
    const char* key = strtok(NULL, ":");
    const char* value = strtok(NULL, ":");

    if(!(method && key)) return;

    if(SHOW_LOGS) {
        printf("The method used was: %s\n", method);
        printf("The key requested was: %s\n", key);
        printf("The value requested was: %s\n", value);
    }

    methodHandler(stringToRequestMethod(method), key, value, res);
}

void methodHandler(RequestMethod method, const char* key, const char* value, char* res) {
    switch (method) {
        case METHOD_GET:
            handleGET(key, res);
            break;
        case METHOD_PUT:
            handlePUT(key, value, res);
            break;
        case METHOD_DELETE:
            handleDELETE(key, res);
            break;
        default:
            printf("Unknown method\n");
            break;
    }
}

void handlePUT(const char* key, const char* value, char* res) {
    if(value == NULL) {
        if(SHOW_LOGS) printf("Value was NULL!");
    }
}

void handleGET(const char* key, char* res) {
    const int someValue = 0;

    res = "AMOGUS";

    //TODO: Get value from key/val storage
    //TODO: Exception-Handling
}

void handleDELETE(const char* key, char* res) {

    //TODO: Delete key/value pare from storage
    //TODO: Exception-Handling
}