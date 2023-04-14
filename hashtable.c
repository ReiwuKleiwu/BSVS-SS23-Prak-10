//
// Created by struc on 14.04.2023.
//

#include "hashtable.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

typedef struct entry {
    char *key;
    char *value;
    struct entry *next;
} entry;

struct _hash_table {
    uint32_t size;
    entry **elements;
};

// Implementation of Murmur hash for 64-bit size_t.
// https://stackoverflow.com/questions/7666509/hash-function-for-string
static size_t MurmurOAAT32(const char * key) {
    uint32_t h = 3323198485ul;
    for (;*key;++key) {
        h ^= *key;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

static size_t hash_table_index(hash_table *ht, const char* key) {
    size_t result = MurmurOAAT32(key) % ht->size;
    return result;
}

hash_table *hash_table_create(uint32_t size) {
    hash_table *ht = malloc(sizeof(*ht));
    ht->size = size;
    ht->elements = calloc(sizeof(entry*), ht->size);
    return ht;
}

void hash_table_destroy(hash_table *ht) {
    //clean up individual elements
    for(int i = 0; i < ht->size; i++) {
        while(ht->elements[i]) {
            entry *tmp = ht->elements[i];
            ht->elements[i] = ht->elements[i]->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        }
    }

    free(ht->elements);
    free(ht);
}

void hash_table_print(hash_table *ht) {
    printf("Start Table \n");
    for(int i = 0; i < ht->size; i++) {
        if(ht->elements[i] == NULL) continue;
        printf("\tIndex: %i\t\n", i);
        entry *tmp = ht->elements[i];
        while(tmp != NULL) {
            printf("\"%s\":\"%s\" -", tmp->key, tmp->value);
            tmp = tmp->next;
        }
        printf("\n");
    }
    printf("End Table \n\n");
}

bool hash_table_upsert(hash_table *ht, const char *key, char *value) {
    if(key == NULL || value == NULL || ht == NULL) return false;
    size_t index = hash_table_index(ht, key);


    //Update an entry
    if(hash_table_lookup(ht, key) != NULL) {
        entry *tmp = ht->elements[index];
        while(tmp != NULL && strcmp(tmp->key, key) != 0) {
            tmp = tmp->next;
        }

        free(tmp->value);
        tmp->value = strdup(value);
        return true;
    }

    //Create new entry
    entry *e = malloc(sizeof(*e));

    /* strdup offers same functionality in a single expression
    e->value = malloc(strlen(value) + 1);
    e->key = malloc(strlen(key) + 1);
    strcpy(e->key, key);
    strcpy(e->value, value);
     */

    e->key = strdup(key);
    e->value = strdup(value);

    //Insert entry
    e->next = ht->elements[index];
    ht->elements[index] = e;
    return true;
}

char *hash_table_lookup(hash_table *ht, const char *key) {
    if(key == NULL || ht == NULL) return NULL;
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elements[index];
    while(tmp != NULL && strcmp(tmp->key, key) != 0) {
        tmp = tmp->next;
    }
    if(tmp == NULL) return NULL;
    return tmp->value;
}
char *hash_table_delete(hash_table *ht, const char *key){
    if(key == NULL || ht == NULL) return NULL;
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elements[index];
    entry *prev = NULL;
    while(tmp != NULL && strcmp(tmp->key, key) != 0) {
        prev = tmp;
        tmp = tmp->next;
    }
    if(tmp == NULL) return NULL;
    if(prev == NULL) {
        //Delete from head of list
        ht->elements[index] = tmp->next;
    } else {
        // Delete elsewhere
        prev->next = tmp->next;
    }

    char *result = tmp->value;
    free(tmp->key);
    free(tmp);

    return result;
}

