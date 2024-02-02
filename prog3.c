#include "Dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_SIZE 100

void printSong(void *data) {
    KVPair *song = (KVPair *)data;
    printf("%s by %s\n", song->key, (char *)song->value);
}

// same as dictionary's strdup, but because we can't modify headers they have to be named differently
// so strdup has become p_strdup (p for prog)
char* p_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
} // algorithm for strdup detailed at https://stackoverflow.com/q/32944390/13947670

void seek_to_next_line( void )
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}

int main(void) {
    Dictionary* dict = dictionary_create(MAX_STR_SIZE, printSong);

    char filename[1000];
    scanf("%s", filename);
    
    FILE* dbFile;
    dbFile = fopen(filename, "r");
    char currentSong[1000];
	while (fgets(currentSong, sizeof(currentSong), dbFile)) {
        KVPair* pair = malloc(sizeof(KVPair));

        // token = artist
        char* token = strtok(currentSong, ",");
        pair->value = p_strdup(token);

        // token = song
        token = strtok(NULL, "\n");
        pair->key = p_strdup(++token);

        dictionary_insert(dict, pair);
    }
    fclose(dbFile);
    getchar();

    int run = 1; // 0 = not running, 1 = running

    while (run == 1) {
        int choice = -1;
        char buffer[100];
        char* s = fgets(buffer, 100, stdin);

        if (s != buffer) {
            printf("An error occurred!\n");
            continue;
        }
        
        int error = sscanf(buffer, "%i", &choice);

        if (error == 0 || choice > 8 || choice < 1) {
            // The string read is not a number!
            printf("Invalid choice! Please select a valid option.\n");
            continue;
        }

        if (choice == 1) {
            KVPair* pair = malloc(sizeof(KVPair));
            char key[150];
            char value[150];
		    scanf(" %145[^\n]", key);
            scanf(" %145[^\n]", value);
            
            pair->key = p_strdup(key);
            pair->value = p_strdup(value);
            dictionary_insert(dict, pair);
            seek_to_next_line();

        } else if (choice == 2) {
            char key[150];
		    scanf(" %145[^\n]", key);
            dictionary_delete(dict, key);
            seek_to_next_line();

        } else if (choice == 3) {
            char key[150];
            char newAuthor[150];
		    scanf(" %145[^\n]", key);
            scanf(" %145[^\n]", newAuthor);
            KVPair* elem = dictionary_find(dict, key);
            if (elem == NULL) {
                printf("Song not found!\n");
            } else {
                free(elem->value);
                elem->value = p_strdup(newAuthor);
            }
            seek_to_next_line();

        } else if (choice == 4) {
            char key[150];
		    scanf(" %145[^\n]", key);
            KVPair* elem = dictionary_find(dict, key);
            if (elem == NULL) {
                printf("Song not found!\n");
            } else {
                printf("Found song: %s by %s\n", elem->key, elem->value);
            }
            seek_to_next_line();

        } else if (choice == 5) {
            dictionary_print(dict);
        } else if (choice == 6) {
            getAuthorWithMostSongs(dict);
        } else if (choice == 7) {
            getMostFrequentWords(dict);
        } else if (choice == 8) {
            printf("Goodbye!\n");
            run=0;
        }
    }
    dictionary_destroy(dict);
    return 0;
}