#ifndef TARO_HASHTABLE_H
#define TARO_HASHTABLE_H

/** Max capacity of the hash table */
#define TABLE_SIZE 1024

/** Prime and offset basis based on the FNV-1a Hash Algorithm */
#define FNV_PRIME 1099511628211UL
#define FNV_OFFSET_BASIS 14695981039346656037UL

#include <stdlib.h>
#include <string.h>

typedef struct Ht_Entry {
    char *key;
    void *value;

    struct Ht_Entry *next;
} Ht_Entry;

typedef struct Hashtable {
    Ht_Entry **entries;
} Hashtable;

static inline unsigned long fnv_hash(char *key) {
    // FNV-1a hash function
    unsigned long hash = FNV_OFFSET_BASIS;
    for (int i = 0; i < strlen(key); i++) {
        hash ^= key[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

Ht_Entry *st_entry(char *key, char *value);

Hashtable *hashtable_create();
void hashtable_free(Hashtable *ht);

void ht_set(Hashtable *ht, char *key, void *value);
void ht_set_string(Hashtable *ht, char *key, char *value);
void ht_set_int(Hashtable *ht, char *key, int value);

void *ht_get(Hashtable *ht, char *key);
char *ht_get_string(Hashtable *ht, char *key);
int ht_get_int(Hashtable *ht, char *key);

#endif
