#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/shm.h>
#include "subStore.h"


static int collisions = 0;

SubStore *create_shared_sub_store(int shm_id) {
    void *shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (void *) -1) {
        perror("shmat Fehler aufgetreten");
        return NULL;
    }

    return (SubStore *) shared_mem;
}

void destroy_shared_sub_store(int shm_id, SubStore *sub_store) {
    shmdt(sub_store);
    shmctl(shm_id, IPC_RMID, NULL);
}

static size_t MurmurOAAT32(const char *key) {
    uint32_t h = 3323198485ul;
    for (; *key; ++key) {
        h ^= *key;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

unsigned int hash_function2(const char *str) {
    size_t hash = MurmurOAAT32(str);
    return hash % TABLE_SIZE;
}

bool sub_store_upsert(SubStore *sub_store, const char *key, const char *value) {
    if (key == NULL || value == NULL || sub_store == NULL) return false;

    unsigned int index = hash_function2(key);
    unsigned int original_index = index;

    if (strcmp(sub_store->table[index].key, "") != 0) collisions++;

    while (strcmp(sub_store->table[index].key, "") != 0 &&
           strncmp(sub_store->table[index].key, key, KEY_SIZE) != 0) {
        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) return false;
    }

    Subscriber tmpSub;
    strncpy(tmpSub.pid, value, strlen(value));

    strncpy(sub_store->table[index].key, key, KEY_SIZE);

    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (strcmp(sub_store->table[index].subscriber[i].pid, value) == 0) {
            return false; // Already subscribed to event
        }
    }


    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (strcmp(sub_store->table[index].subscriber[i].pid, "") == 0) {
            sub_store->table[index].subscriber[i] = tmpSub;
            return true;
        }
    }

    // maximum amount of subscribers were reached
    return false;
}


Subscriber *sub_store_lookup(SubStore *sub_store, const char *key) {
    if (sub_store == NULL || key == NULL) return NULL;

    unsigned int index = hash_function2(key);
    unsigned int original_index = index;

    while (strcmp(sub_store->table[index].key, "") != 0) {
        if (strncmp(sub_store->table[index].key, key, KEY_SIZE) == 0) {
            return sub_store->table[index].subscriber;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    return NULL;
}

bool sub_store_delete(SubStore *sub_store, const char *key, const char *value) {
    if (key == NULL || sub_store == NULL) return NULL;
    unsigned int index = hash_function2(key);
    unsigned int original_index = index;

    while (strcmp(sub_store->table[index].key, "") != 0) {
        if (strncmp(sub_store->table[index].key, key, KEY_SIZE) == 0) {

            for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
                if (strcmp(sub_store->table[index].subscriber[i].pid, value) == 0) {
                    strncpy(sub_store->table[index].subscriber[i].pid, "", PID_SIZE);
                }
            }

            bool isEmpty = true;

            for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
                if (strcmp(sub_store->table[index].subscriber[i].pid, "") != 0) {
                    isEmpty = false;
                    break;
                }
            }

            if (isEmpty) {
                memset(sub_store->table[index].key, 0, KEY_SIZE);
                memset(sub_store->table[index].subscriber, 0, MAX_SUBSCRIBER_COUNT);
            }

            return true;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    return NULL;
}

void sub_store_print(SubStore *sub_store) {
    printf("Start Table \n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(sub_store->table[i].key, "") == 0) continue;
        printf("\tIndex: %i\t\n", i);
        SubEntry *tmp = &sub_store->table[i];

        printf("\"%s\": [ ", tmp->key);

        for (int j = 0; j < MAX_SUBSCRIBER_COUNT; j++) {
            if (strcmp(tmp->subscriber[j].pid, "") != 0) {
                printf("\"%s\" ", tmp->subscriber[j].pid);
            }
        }

        printf("]\n");
    }
    printf("End Table \n\n");
}

void sub_store_print_collisions(SubStore *sub_store) {
    printf("Es gab %d Kollisionen.\n", collisions);
}