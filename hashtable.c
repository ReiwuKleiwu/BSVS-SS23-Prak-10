#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/shm.h>
#include "hashtable.h"
#include <semaphore.h>

static int collisions = 0;

HashTable *create_shared_hashtable(int shm_id) {
    void *shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (void *) -1) {
        perror("shmat error occurred");
        return NULL;
    }
    sem_init(&((HashTable *) shared_mem)->lock, 0, 1);
    return (HashTable *) shared_mem;
}

void destroy_shared_hashtable(int shm_id, HashTable *hash_table) {
    sem_destroy(&hash_table->lock);
    shmdt(hash_table);
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

unsigned int hash_function(const char *str) {
    size_t hash = MurmurOAAT32(str);
    return hash % TABLE_SIZE;
}

bool hash_table_upsert(HashTable *hash_table, const char *key, const char *value) {
    if (key == NULL || value == NULL || hash_table == NULL) return false;

    sem_wait(&hash_table->lock);

    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    if (strcmp(hash_table->table[index].key, "") != 0) collisions++;

    while (strcmp(hash_table->table[index].key, "") != 0 &&
           strncmp(hash_table->table[index].key, key, KEY_SIZE) != 0) {
        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) return false;
    }

    strncpy(hash_table->table[index].key, key, KEY_SIZE);
    strncpy(hash_table->table[index].value, value, VALUE_SIZE);
    sem_post(&hash_table->lock);
    return true;
}

char *hash_table_lookup(HashTable *hash_table, const char *key) {
    if (hash_table == NULL || key == NULL) return NULL;
    sem_wait(&hash_table->lock);

    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(hash_table->table[index].key, "") != 0) {
        if (strncmp(hash_table->table[index].key, key, KEY_SIZE) == 0) {
            sem_post(&hash_table->lock);
            return hash_table->table[index].value;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    sem_post(&hash_table->lock);
    return NULL;
}

bool hash_table_delete(HashTable *hash_table, const char *key) {
    if (key == NULL || hash_table == NULL) return NULL;
    sem_wait(&hash_table->lock);
    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(hash_table->table[index].key, "") != 0) {
        if (strncmp(hash_table->table[index].key, key, KEY_SIZE) == 0) {
            memset(hash_table->table[index].key, 0, KEY_SIZE);
            memset(hash_table->table[index].value, 0, VALUE_SIZE);
            return true;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    sem_post(&hash_table->lock);
    return NULL;
}

void hash_table_print(HashTable *hash_table) {
    sem_wait(&hash_table->lock);
    printf("Start Table \n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(hash_table->table[i].key, "") == 0) continue;
        printf("\tIndex: %i\t\n", i);
        Entry *tmp = &hash_table->table[i];

        printf("\"%s\":\"%s\"", tmp->key, tmp->value);

        printf("\n");
    }
    printf("End Table \n\n");
    sem_post(&hash_table->lock);
}

void hash_table_print_collisions(HashTable *hash_table) {
    printf("You got %d Collisions.\n", collisions);
}