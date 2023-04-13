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

void requestHandler(char* request);
void methodHandler(RequestMethod method, const char* key, const char* value);
RequestMethod stringToRequestMethod(const char* method);
int handlePUT(const char* key, const char* value);

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

            requestHandler(in);

            printf("sending back the %d bytes I received...\n", bytes_read);
            printf("%s\n", in);

            //write(client_socket, in, bytes_read);
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

void requestHandler(char* request) {
    const char* method = strtok(request, ":");
    const char* key = strtok(NULL, ":");
    const char* value = strtok(NULL, ":");

    if(!(method && key)) return;

    if(SHOW_LOGS) {
        printf("The method used was: %s\n", method);
        printf("The key requested was: %s\n", key);
        printf("The value requested was: %s\n", value);
    }

    methodHandler(stringToRequestMethod(method), key, value);
}

void methodHandler(RequestMethod method, const char* key, const char* value) {
    switch (method) {
        case METHOD_GET:
            printf("Handling GET method\n");
            break;
        case METHOD_PUT:
            handlePUT(key, value);
            break;
        case METHOD_DELETE:
            printf("Handling DELETE method\n");
            break;
        default:
            printf("Unknown method\n");
            break;
    }
}

int handlePUT(const char* key, const char* value) {
    if(value == NULL) {
        if(SHOW_LOGS) printf("Value was NULL!");
        return -1;
    }
    return 0;
}
