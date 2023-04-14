//
// Created by struc on 14.04.2023.
//

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef TESTRM_HASHTABLE_H
#define TESTRM_HASHTABLE_H

typedef struct _hash_table hash_table;

hash_table *hash_table_create(uint32_t size);
void hash_table_destroy(hash_table *ht);
void hash_table_print(hash_table *ht);
bool hash_table_upsert(hash_table *ht, const char *key, char *value);
char *hash_table_lookup(hash_table *ht, const char *key);
char *hash_table_delete(hash_table *ht, const char *key);

#endif //TESTRM_HASHTABLE_H
