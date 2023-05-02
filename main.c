#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>

#include "hashtable.h"
#include "subStore.h"
#include "socket_server.h"

#define PORT 5678
#define SHOW_LOGS 1

int main() {
    int hashtable_shm_id = shmget(IPC_PRIVATE, sizeof(HashTable), 0644 | IPC_CREAT);
    int sub_store_shm_id = shmget(IPC_PRIVATE, sizeof(SubStore), 0644 | IPC_CREAT);
    if (hashtable_shm_id == -1 || sub_store_shm_id == -1) {
        perror("The segment could not be created!");
        exit(1);
    }

    HashTable *hash_table = create_shared_hashtable(hashtable_shm_id);
    SubStore *sub_store = create_shared_sub_store(sub_store_shm_id);


    int listening_socket; // Rendevouz-Descriptor

    // Socket erstellen
    listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket < 0) {
        fprintf(stderr, "Socket could not be created\n");
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
    if (bind_result < 0) {
        fprintf(stderr, "Socket could not be bound!\n");
        exit(-1);
    }

    // Socket lauschen lassen
    int lrt = listen(listening_socket, 5);
    if (lrt < 0) {
        fprintf(stderr, "Socket could not be set to listen\n");
        exit(-1);
    }

    // Warten auf Verbindungen
    handleClientConnections(listening_socket, hash_table, sub_store);

    // Rendevouz Descriptor schließen
    close(listening_socket);

    destroy_shared_hashtable(hashtable_shm_id, hash_table);
    destroy_shared_sub_store(sub_store_shm_id, sub_store);

    return 0;
}