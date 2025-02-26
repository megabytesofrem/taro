#include "hashtable.h"

Ht_Entry *ht_entry(char *key, void *value) {
    Ht_Entry *entry = (Ht_Entry *)malloc(sizeof(Ht_Entry));
    entry->key      = strdup(key);
    entry->value    = strdup(value);
    entry->next     = NULL;

    return entry;
}

Hashtable *hashtable_create() {
    Hashtable *ht = (Hashtable *)malloc(sizeof(Hashtable));
    ht->entries   = (Ht_Entry **)malloc(sizeof(Ht_Entry) * TABLE_SIZE);

    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->entries[i] = NULL;
    }

    return ht;
}

void hashtable_free(Hashtable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Ht_Entry *entry = ht->entries[i];
        while (entry != NULL) {
            Ht_Entry *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }

    free(ht->entries);
    free(ht);
}

void ht_set(Hashtable *ht, char *key, void *value) {
    unsigned int slot = fnv_hash(key) % TABLE_SIZE;

    Ht_Entry *entry = ht->entries[slot];
    if (entry == NULL) {
        // Empty bucket - insert here
        ht->entries[slot] = ht_entry(key, value);
        return;
    }

    Ht_Entry *prev;

    // Traverse the linked list
    while (entry != NULL) {
        // Key already exists - update the value
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }

        // Move to the next entry
        prev  = entry;
        entry = prev->next;
    }

    // Reached the end
    prev->next = ht_entry(key, value);
}

void ht_set_string(Hashtable *ht, char *key, char *value) {
    ht_set(ht, key, (void *)value);
}

void ht_set_int(Hashtable *ht, char *key, int value) { ht_set(ht, key, (void *)&value); }

void *ht_get(Hashtable *ht, char *key) {
    unsigned int slot = fnv_hash(key) % TABLE_SIZE;

    Ht_Entry *entry = ht->entries[slot];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        entry = entry->next;
    }

    // Did not find an entry
    return NULL;
}

char *ht_get_string(Hashtable *ht, char *key) { return (char *)ht_get(ht, key); }

int ht_get_int(Hashtable *ht, char *key) {
    // Cast to int* and then dereference
    return *(int *)ht_get(ht, key);
}
