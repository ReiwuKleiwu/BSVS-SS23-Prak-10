#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/shm.h>
#include "sub_store.h"
#include <semaphore.h>

static int collisions = 0;

SubStore *create_shared_sub_store(int shm_id) {
    void *shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (void *) -1) {
        perror("shmat Fehler aufgetreten");
        return NULL;
    }
    sem_init(&((SubStore *) shared_mem)->lock, 0, 1);
    return (SubStore *) shared_mem;
}

void destroy_shared_sub_store(int shm_id, SubStore *sub_store) {
    sem_destroy(&sub_store->lock);
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

static unsigned int hash_function(const char *str) {
    size_t hash = MurmurOAAT32(str);
    return hash % TABLE_SIZE;
}

bool sub_store_upsert(SubStore *sub_store, const char *key, int value) {
    if (key == NULL || value == NULL || sub_store == NULL) return false;

    sem_wait(&sub_store->lock);

    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    if (strcmp(sub_store->table[index].key, "") == 0) {
        for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
            sub_store->table[index].subscriber[i].pid = -1;
        }
    } else {
        collisions++;
    }

    while (strcmp(sub_store->table[index].key, "") != 0 &&
        strncmp(sub_store->table[index].key, key, KEY_SIZE) != 0) {
        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) {
            sem_post(&sub_store->lock);
            return false;
        }
    }

    Subscriber tmpSub;
    tmpSub.pid = value;

    strncpy(sub_store->table[index].key, key, KEY_SIZE);

    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (sub_store->table[index].subscriber[i].pid == value) {
            sem_post(&sub_store->lock);
            return false; // Already subscribed to event
        }
    }


    for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
        if (sub_store->table[index].subscriber[i].pid == -1) {
            sub_store->table[index].subscriber[i] = tmpSub;
            sem_post(&sub_store->lock);
            return true;
        }
    }
    sem_post(&sub_store->lock);

    // maximum amount of subscribers were reached
    return false;
}


Subscriber *sub_store_lookup(SubStore *sub_store, const char *key) {
    if (sub_store == NULL || key == NULL) return NULL;
    sem_wait(&sub_store->lock);

    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(sub_store->table[index].key, "") != 0) {
        if (strncmp(sub_store->table[index].key, key, KEY_SIZE) == 0) {
            sem_post(&sub_store->lock);
            return sub_store->table[index].subscriber;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    sem_post(&sub_store->lock);
    return NULL;
}

bool sub_store_delete(SubStore *sub_store, const char *key, int value) {
    if (key == NULL || sub_store == NULL) return NULL;

    sem_wait(&sub_store->lock);
    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(sub_store->table[index].key, "") != 0) {
        if (strncmp(sub_store->table[index].key, key, KEY_SIZE) == 0) {

            for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
                if (sub_store->table[index].subscriber[i].pid == value) {
                    sub_store->table[index].subscriber[i].pid = -1;
                }
            }

            bool isEmpty = true;

            for (int i = 0; i < MAX_SUBSCRIBER_COUNT; i++) {
                if (sub_store->table[index].subscriber[i].pid != -1) {
                    isEmpty = false;
                    break;
                }
            }

            if (isEmpty) {
                memset(sub_store->table[index].key, 0, KEY_SIZE);
                memset(sub_store->table[index].subscriber, 0, MAX_SUBSCRIBER_COUNT);
            }
            sem_post(&sub_store->lock);

            return true;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    sem_post(&sub_store->lock);
    return NULL;
}

void sub_store_print(SubStore *sub_store) {
    printf("Start Table \n");
    sem_wait(&sub_store->lock);
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(sub_store->table[i].key, "") == 0) continue;
        printf("\tIndex: %i\t\n", i);
        SubEntry *tmp = &sub_store->table[i];

        printf("\"%s\": [ ", tmp->key);

        for (int j = 0; j < MAX_SUBSCRIBER_COUNT; j++) {
            if (tmp->subscriber[j].pid != -1) {
                printf("\"%d\" ", tmp->subscriber[j].pid);
            }
        }

        printf("]\n");
    }
    sem_post(&sub_store->lock);
    printf("End Table \n\n");
}

void sub_store_print_collisions(SubStore *sub_store) {
    printf("Es gab %d Kollisionen.\n", collisions);
}