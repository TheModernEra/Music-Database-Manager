#include "List.h"
#include "HashTable.h"
#include "Dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dictionary {
    int slots;
    int size;
    ListPtr *hash_table;
} Dictionary;

// Helper function to find a node with a specific key in a list and return its index.
// Returns -1 if the key is not found.
static int find_key_index(ListPtr L, char *key) {
    for (int i = 0; i < lengthList(L); i++) {
        KVPair *pair = (KVPair *)getList(L, i);
        if (strcmp(pair->key, key) == 0) {
            return i;
        }
    }
    return -1;
}

char* strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
} // algorithm for strdup detailed at https://stackoverflow.com/q/32944390/13947670

Dictionary* dictionary_create(int hash_table_size, void (*dataPrinter)(void *data)) {
    Dictionary* dict = malloc(sizeof(Dictionary));
    dict->slots = hash_table_size;
    dict->size = 0;
    dict->hash_table = calloc(hash_table_size, sizeof(ListPtr));
    for (int i = 0; i < hash_table_size; i++) {
        dict->hash_table[i] = createList(dataPrinter);
    }
    return dict;
}

void dictionary_destroy(Dictionary *d) {
    for (int i = 0; i < d->slots; i++) {
        destroyList(&d->hash_table[i]);
    }
    free(d->hash_table);
    free(d);
}

bool dictionary_insert(Dictionary *D, KVPair *elem) {
    if (dictionary_find(D, elem->key) != NULL) {
        return false;
    }
    unsigned int index = ht_hash(elem->key, D->slots); // get the index for the hashed key
    if (appendList(D->hash_table[index], elem) == true) { // insert the KVPair into the LL at the index
        D->size++;
        return true;
    }
    return false; 
}

KVPair* dictionary_delete(Dictionary *D, char *key) {
    if (dictionary_find(D, key) == NULL) {
        return NULL;
    }
    for (int i = 0; i < D->slots; i++) { // loop through all slots
        if (D->hash_table[i]->length != 0) { // if LL at index isn't empty
            int index = find_key_index(D->hash_table[i], key);
            if (index != -1) {
                D->size--;
                return deleteList(D->hash_table[i], index);
            }
        }
    }
}

KVPair* dictionary_find(Dictionary *D, char *k) {
    for (int i = 0; i < D->slots; i++) { // loop through all slots
        if (D->hash_table[i]->length != 0) { // if LL at index isn't empty
            int index = find_key_index(D->hash_table[i], k);
            if (index != -1) {
                return getList(D->hash_table[i], index);
            }
        }
    }
    return NULL;
}

void dictionary_print(Dictionary *D) {
    for (int i = 0; i < D->slots; i++) {
        printList(D->hash_table[i]);
    }
}

void getAuthorWithMostSongs(Dictionary *D) {
    
    Dictionary* artistDict = dictionary_create(D->slots, D->hash_table[0]->dataPrinter);

    // read every song into a new dict, with artist as key and count as val
    // if artist not in dict, add artist as key and 0 as value
    // if artist already in dict, increase value by 1
    for (int i = 0; i < D->slots; i++) {
        if (D->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(D->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(D->hash_table[i], j);

                if (dictionary_find(artistDict, pair->value) == NULL) {
                    KVPair* artist = malloc(sizeof(KVPair));
                    artist->key = pair->value;
                    artist->value = (int){1};
                    dictionary_insert(artistDict, artist);
                } else {
                    KVPair* exists = dictionary_find(artistDict, pair->value);
                    exists->value++;
                }
            }
        }
    }

    // go through the new dict and find highest count in whole dict
    int highest = -1;
    for (int i = 0; i < artistDict->slots; i++) {
        if (artistDict->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(artistDict->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(artistDict->hash_table[i], j);
                int val = pair->value;
                if (val > highest) {
                    highest = val;
                }
            }
        }
    }
    
    // print all artists with highest count as their value
    for (int i = 0; i < artistDict->slots; i++) {
        if (artistDict->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(artistDict->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(artistDict->hash_table[i], j);
                int val = pair->value;
                if (val == highest) {
                    printf("%s: %d song(s)\n", pair->key, pair->value);
                }
            }
        }
    }

    dictionary_destroy(artistDict);
}

void getMostFrequentWords(Dictionary *D) {
    
    Dictionary* wordDict = dictionary_create(D->slots * 5, NULL);

    // go thru every song, splitting into words
    // if word isnt in wordDict, add to it
    // it it is, increase the count
    for (int i = 0; i < D->slots; i++) {
        if (D->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(D->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(D->hash_table[i], j);
                char buffer[105];
                strncpy(buffer, pair->key, sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                char* token = strtok(buffer, " ");
                
                while (token != NULL) {
                    if (dictionary_find(wordDict, token) == NULL) {
                        KVPair* word = malloc(sizeof(KVPair));
                        word->key = strdup(token);
                        word->value = (int){1};
                        dictionary_insert(wordDict, word);
                    } else {
                        KVPair* match = dictionary_find(wordDict, token);
                        match->value++;
                    }
                    token = strtok(NULL, " ");
                }
                memset(buffer, 0, sizeof(buffer));
            }
        }
    }

    int highest = -1;
    for (int i = 0; i < wordDict->slots; i++) {
        if (wordDict->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(wordDict->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(wordDict->hash_table[i], j);
                int val = pair->value;
                if (val > highest) {
                    highest = val;
                }
            }
        }
    }
    
    for (int i = 0; i < wordDict->slots; i++) {
        if (wordDict->hash_table[i]->length != 0) {
            for (int j = 0; j < lengthList(wordDict->hash_table[i]); j++) {
                KVPair* pair = (KVPair *)getList(wordDict->hash_table[i], j);
                int val = pair->value;
                if (val == highest) {
                    printf("%s: %d occurrence(s)\n", pair->key, pair->value);
                }
            }
        }
    }
  
    dictionary_destroy(wordDict);
}