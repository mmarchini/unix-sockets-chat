#ifndef _HASH
#define _HASH
#include "room.h"

typedef struct _HashTable{
    Room *room;
    struct _HashTable *next;
} HashTable;

HashTable *hashInit();

HashTable *hashInsert(HashTable *hash, char *value);

HashTable *hashSearch(HashTable *hash, char *value);

int hashCalculate(char *value);

//void hashPrint(HashTable *hash);

#endif
