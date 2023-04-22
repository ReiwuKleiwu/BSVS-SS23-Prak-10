#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/shm.h>
#include "hashtable.h"

static int collisions = 0;

HashTable *create_shared_hashtable(int shm_id) {
    void *shared_mem = shmat(shm_id, NULL, 0);
    if (shared_mem == (void *) -1) {
        perror("shmat Fehler aufgetreten");
        return NULL;
    }
    return (HashTable *) shared_mem;
}

void destroy_shared_hashtable(int shm_id, HashTable *hash_table) {
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
    return true;
}

char *hash_table_lookup(HashTable *hash_table, const char *key) {
    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(hash_table->table[index].key, "") != 0) {
        if (strncmp(hash_table->table[index].key, key, KEY_SIZE) == 0) {
            return hash_table->table[index].value;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    return NULL;
}

char *hash_table_delete(HashTable *hash_table, const char *key) {
    if (key == NULL || hash_table == NULL) return NULL;
    unsigned int index = hash_function(key);
    unsigned int original_index = index;

    while (strcmp(hash_table->table[index].key, "") != 0) {
        if (strncmp(hash_table->table[index].key, key, KEY_SIZE) == 0) {
            char* deletedValue = hash_table->table[index].value;
            memset(hash_table->table[index].key, 0, KEY_SIZE);
            memset(hash_table->table[index].value, 0, VALUE_SIZE);
            return deletedValue;
        }

        index = (index + 1) % TABLE_SIZE;
        if (index == original_index) break;
    }
    return NULL;
}

void hash_table_print(HashTable *hash_table) {
    printf("Start Table \n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(hash_table->table[i].key, "") == 0) continue;
        printf("\tIndex: %i\t\n", i);
        Entry *tmp = &hash_table->table[i];

        printf("\"%s\":\"%s\"", tmp->key, tmp->value);

        printf("\n");
    }
    printf("End Table \n\n");
}

void hash_table_print_collisions(HashTable *hash_table) {
    printf("Es gab %d Kollisionen.\n", collisions);
}