// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <unistd.h>

#include "handle_requests.h"
#include "hashtable.h"
#include "request.h"
#include "socket_server.h"
#include "sub_message_queue.h"
#include "sub_store.h"
#include "validate_user_input.h"

#define SHOW_LOGS 1
#define BUFFERSIZE 1024
#define MAX_CLIENTS 128

void runServer(int port) {
    int hashtable_shm_id = shmget(IPC_PRIVATE, sizeof(HashTable), 0644 | IPC_CREAT);
    int sub_store_shm_id = shmget(IPC_PRIVATE, sizeof(SubStore), 0644 | IPC_CREAT);
    int client_sockets_shm_id = shmget(IPC_PRIVATE, sizeof(int) * MAX_CLIENTS, 0644 | IPC_CREAT);
    int server_lock_shm_id = shmget(IPC_PRIVATE, sizeof(int), 0644 | IPC_CREAT);
    if (hashtable_shm_id == -1 || sub_store_shm_id == -1 || client_sockets_shm_id == -1) {
        perror("The segment could not be created!");
        exit(1);
    }

    HashTable *hash_table = create_shared_hashtable(hashtable_shm_id);
    SubStore *sub_store = create_shared_sub_store(sub_store_shm_id);
    int *server_is_locked_by_transaction = (int *) shmat(server_lock_shm_id, NULL, 0);
    *server_is_locked_by_transaction = 0;
    int sub_queue_id = create_sub_message_queue(12345);


    int listening_socket;

    listening_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listening_socket < 0) {
        fprintf(stderr, "Socket could not be created\n");
        exit(-1);
    }

    int option = 1;
    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    int bind_result = bind(listening_socket, (struct sockaddr *) &server, sizeof(server));
    if (bind_result < 0) {
        fprintf(stderr, "Socket could not be bound!\n");
        exit(-1);
    }

    int lrt = listen(listening_socket, 5);
    if (lrt < 0) {
        fprintf(stderr, "Socket could not be set to listen\n");
        exit(-1);
    }

    handleClientConnections(listening_socket, hash_table, sub_store, server_is_locked_by_transaction, sub_queue_id);

    close(listening_socket);
    destroy_shared_hashtable(hashtable_shm_id, hash_table);
    destroy_shared_sub_store(sub_store_shm_id, sub_store);
}

void handleClientConnections(int listening_socket, HashTable *keyValStore, SubStore *subStore,
                             int *server_is_locked_by_transaction, int sub_queue_id) {
    int client_socket;
    char client_request_buffer[BUFFERSIZE];
    struct sockaddr_in client;
    socklen_t client_len;

    while (1) {
        if (SHOW_LOGS) {
            printf("Waiting for incoming socket connection..\n");
        }

        client_socket = accept(listening_socket, (struct sockaddr *) &client, &client_len);


        if (fork() == 0) {
            int pid = getpid();

            char *connected = "Welcome:\r\n";
            if (SHOW_LOGS) {
                printf("INFO: client-%d connected\n", pid);
            }
            send(client_socket, connected, strlen(connected), 0);


            int notification_handler = fork();
            if (notification_handler == 0) {
                while (1) {
                    send_new_notifications(sub_queue_id, pid, client_socket);
                }
            }

            ssize_t received_client_bytes;
            while ((received_client_bytes = readUntilNewLine(client_socket, client_request_buffer, BUFFERSIZE)) > 0) {
                Request client_request;
                memset(client_request.body, 0, strlen(client_request.body));
                memset(client_request.response, 0, strlen(client_request.response));
                strncpy(client_request.body, client_request_buffer, strlen(client_request_buffer));
                client_request.key_value_store = keyValStore;
                client_request.subscriber_store = subStore;
                client_request.sub_queue_id = sub_queue_id;
                client_request.client_socket = client_socket;
                client_request.client_pid = getpid();
                client_request.server_is_locked_by_transaction = server_is_locked_by_transaction;

                sanitizeUserInput(client_request.body);
                if (isValidateFormat(client_request)) {
                    requestHandler(client_request);
                }
            }

            if (SHOW_LOGS) {
                printf("INFO: Client-%d disconnected\n", pid);
            }

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