#include "sub_store.h"

#ifndef BSVS_SS23_PRAK_SUB_MESSAGE_QUEUE_H
#define BSVS_SS23_PRAK_SUB_MESSAGE_QUEUE_H

#define MAX_PAYLOAD_SIZE 1024

typedef struct message {
    long type;
    char payload[MAX_PAYLOAD_SIZE];
} message;

int create_sub_message_queue(int key);

void send_notification_message(int queue_id, message sub_message);

void notify_on_event(int queue_id, SubStore *sub_store, char *notify_payload, char *event_key);

void send_new_notifications(int queue_id, int pid, int client_socket);

#endif //BSVS_SS23_PRAK_SUB_MESSAGE_QUEUE_H
