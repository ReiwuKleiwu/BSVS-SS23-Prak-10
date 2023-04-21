#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>

#include "hashtable.h"
#include "socket_server.h"

#define PORT 5678
#define SHOW_LOGS 1

int main() {

    int shm_id = shmget(IPC_PRIVATE, sizeof(HashTable), 0644 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    HashTable *hash_table = create_shared_hashtable(shm_id);

    int listening_socket; // Rendevouz-Descriptor
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

    /*
    for(int i = 0; i < 1000000; i++) {
        char str[7*4];
        sprintf(str, "%d", i);
        hash_table_upsert(hash_table, str, str);
    }
     */

    handleClientConnections(listening_socket, hash_table);

    // Rendevouz Descriptor schließen
    close(listening_socket);
    destroy_shared_hashtable(shm_id, hash_table);

    return 0;
}