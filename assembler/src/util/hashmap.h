#pragma once

typedef struct {
    int length;
    int capacity;
} Hash_Header;

#define INITIAL_CAPACITY 256

#define HASH_MAP(T) (T *)hash_map_init(sizeof(T), INITIAL_CAPACITY)

#define HASH_GET(h, s) ( \

void *hash_map_init(int item_size, int capacity);

// hash function
int hash(char *string);



