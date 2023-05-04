#include <stdbool.h>

#ifndef SUBTORE_H
#define SUBTORE_H

#define KEY_SIZE 32
#define VALUE_SIZE 128
#define TABLE_SIZE (1 << 20)
#define PID_SIZE 6
#define MAX_SUBSCRIBER_COUNT 128

typedef struct _Subscriber {
    char pid[PID_SIZE];
} Subscriber;

typedef struct _Entry {
    char key[KEY_SIZE];
    Subscriber subscriber[MAX_SUBSCRIBER_COUNT];
} SubEntry;

typedef struct _SubStore {
    SubEntry table[TABLE_SIZE];
} SubStore;

SubStore *create_shared_sub_store(int shm_id);
void destroy_shared_sub_store(int shm_id, SubStore *sub_store);
bool sub_store_upsert(SubStore *sub_store, const char *key, const char *value);
Subscriber *sub_store_lookup(SubStore *sub_store, const char *key);
bool sub_store_delete(SubStore *sub_store, const char *key, const char *value);
void sub_store_print(SubStore *sub_store);
void sub_store_print_collisions(SubStore *sub_store);

#endif // SUBTORE_H