#include <stdbool.h>

#ifndef HASHTABLE_H
#define HASHTABLE_H

#define KEY_SIZE 32
#define VALUE_SIZE 128
#define TABLE_SIZE (1 << 20)

typedef struct Entry {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} Entry;

typedef struct HashTable {
    Entry table[TABLE_SIZE];
} HashTable;

HashTable *create_shared_hashtable(int shm_id);
void destroy_shared_hashtable(int shm_id, HashTable *hash_table);
unsigned int hash_function(const char *str);
bool hash_table_upsert(HashTable *hash_table, const char *key, const char *value);
char *hash_table_lookup(HashTable *hash_table, const char *key);
char *hash_table_delete(HashTable *hash_table, const char *key);
void hash_table_print(HashTable *hash_table);
void hash_table_print_collisions(HashTable *hash_table);

#endif // HASHTABLE_H