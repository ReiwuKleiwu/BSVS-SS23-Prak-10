#include "sub_message_queue.h"
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>

int create_sub_message_queue(int key) {
    int queue_id = msgget(key, IPC_CREAT | 0644);
    return queue_id;
}

void send_notification_message(int queue_id, message sub_message) {
    printf("Dispatching message \"%s\" to client %ld.\n", sub_message.payload, sub_message.type);
    msgsnd(queue_id, &sub_message, MAX_PAYLOAD_SIZE, 0);
}

void notify_on_event(int queue_id, SubStore *sub_store, char *notify_payload, char *event_key) {
    Subscriber *subscribed_processes = sub_store_lookup(sub_store, event_key);

    if(subscribed_processes == NULL) return;

    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (subscribed_processes[i].pid == -1) continue;
        printf("Sub-Pid: %d\n", subscribed_processes[i].pid);

        message tmp_message;
        tmp_message.type = subscribed_processes[i].pid;
        memset(tmp_message.payload, 0, strlen(tmp_message.payload));
        strncpy(tmp_message.payload, notify_payload, strlen(notify_payload));

        send_notification_message(queue_id, tmp_message);
    }
}

void read_new_messages(int queue_id, int type) {
    message tmp_message;
    msgrcv(queue_id, &tmp_message, MAX_PAYLOAD_SIZE, type, IPC_NOWAIT);
    printf("Message: %s\n", tmp_message.payload);
}




